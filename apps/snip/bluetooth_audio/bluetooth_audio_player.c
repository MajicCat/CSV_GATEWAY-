/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */
#include "wiced.h"
#include "wiced_rtos.h"
#include "wiced_audio.h"
#include "bluetooth_audio.h"

#ifdef USE_MEM_POOL
#include "mem_pool.h"
#endif

/*****************************************************************************
**
**  Name:           bt_audio_wiced_audio_player.c
**
**  Description:    BTA AVK interface to the Wiced audio subsystem
**
*****************************************************************************/

/******************************************************
 *                      Macros
 ******************************************************/
#define VOLUME_CONVERSION(step,level,min)                    ((double)step*(double)level + min)

/******************************************************
 *                    Constants
 ******************************************************/
#define BT_AUDIO_DEFAULT_PERIOD_SIZE        ( 1024 )
#define BT_AUDIO_BUFFER_SIZE                ( 4*BT_AUDIO_DEFAULT_PERIOD_SIZE )
#define NUM_USECONDS_IN_SECONDS                     (1000*1000)
#define NUM_MSECONDS_IN_SECONDS                     (1000)
#define WAIT_TIME_DELTA                             (20)
#define BT_AUDIO_PLAYER_QUEUE_MAX_SIZE  (28)
#define BT_AUDIO_PLAYER_QUEUE_THRESHOLD (10)


/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/
static wiced_bool_t read_bluetooth_audio_data( uint8_t *buffer, uint16_t* size );

/******************************************************
 *               Variables Definitions
 ******************************************************/
bt_audio_context_t player;
#ifdef USE_MEM_POOL
extern bt_buffer_pool_handle_t  mem_pool;
#endif

/******************************************************
 *               Function Definitions
 ******************************************************/
 wiced_bool_t is_bt_audio_player_initialized( void )
{
    return (wiced_bool_t)( player.state > BT_AUDIO_DEVICE_STATE_IDLE );
}

wiced_bool_t is_bt_audio_player_prepared( void )
{
    return (wiced_bool_t)( player.state == BT_AUDIO_DEVICE_STATE_CONFIGURED ||
                           player.state == BT_AUDIO_DEVICE_STATE_STOPPED );
}

wiced_result_t  bt_audio_init_player( void )
{
    wiced_result_t result = WICED_SUCCESS;

    if(player.state == BT_AUDIO_DEVICE_STATE_UNINITIALIZED)
    {
        result = wiced_rtos_init_queue(&player.queue, "PLAYER_QUEUE", sizeof(bt_audio_codec_data_t*), BT_AUDIO_PLAYER_QUEUE_MAX_SIZE);
        result = wiced_rtos_init_event_flags(&player.events);
        result = wiced_rtos_init_semaphore(&player.wait_for_cmd_completion);
        player.state = BT_AUDIO_DEVICE_STATE_IDLE;
    }
    WPRINT_APP_INFO ( ("bt_audio_init_player: result=%d\n", result) );
    return result;
}


wiced_result_t bt_audio_configure_player( bt_audio_config_t* p_audio_config )
{
    wiced_result_t    result;
    wiced_audio_config_t wiced_audio_conf = {0, };

    //WPRINT_APP_INFO(("bt_audio_configure_player: INIT\n"));

    if(p_audio_config == NULL)
        return WICED_BADARG;

    if(player.state != BT_AUDIO_DEVICE_STATE_IDLE)
        return WICED_ERROR;

    /* Initialize and configure audio framework for needed audio format */
    result = wiced_audio_init(PLATFORM_DEFAULT_AUDIO_OUTPUT, &player.bluetooth_audio_session_handle, BT_AUDIO_DEFAULT_PERIOD_SIZE);
    if( result != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ("bt_audio_configure_player:  Error Initializing Wiced Audio Framework[err: %d]\n",result) );
        return result;
    }

    result = wiced_audio_create_buffer(player.bluetooth_audio_session_handle, BT_AUDIO_BUFFER_SIZE, NULL, NULL );
    if( result != WICED_SUCCESS )
    {
        WPRINT_APP_INFO(("bt_audio_configure_player: Error Registering Buffer to Wiced Audio Framework [err: %d]\n",result));
        return result;
    }

   memcpy(&player.bluetooth_audio_config, p_audio_config, sizeof(bt_audio_config_t));
   WPRINT_APP_INFO(("bt_audio_configure_player: config sample_rate:%u channels:%d bps:%d\n", (unsigned int) player.bluetooth_audio_config.sample_rate,
                                            (int)player.bluetooth_audio_config.channels, (int)player.bluetooth_audio_config.bits_per_sample));

    wiced_audio_conf.sample_rate     = player.bluetooth_audio_config.sample_rate;
    wiced_audio_conf.channels        = player.bluetooth_audio_config.channels;
    wiced_audio_conf.bits_per_sample = player.bluetooth_audio_config.bits_per_sample;
    wiced_audio_conf.frame_size      = (wiced_audio_conf.channels * wiced_audio_conf.bits_per_sample) / 8;

    result = wiced_audio_configure( player.bluetooth_audio_session_handle, &wiced_audio_conf );
    if( result != WICED_SUCCESS )
    {
        WPRINT_APP_INFO(("bt_audio_configure_player: Error configuring Wiced Audio framework [err: %d]\n",result));
        return result;
    }
    player.state = BT_AUDIO_DEVICE_STATE_CONFIGURED;

    //Set the default volume
    result = bt_audio_update_player_volume(BT_AUDIO_DEFAULT_VOLUME);

    return result;
}


void bt_audio_write_to_player_buffer( bt_audio_codec_data_t* pcm )
{
    static uint8_t q_count = 0;
    wiced_result_t result;

    if( WICED_TRUE == is_bt_audio_player_initialized() )
    {
        result = wiced_rtos_push_to_queue( &player.queue, &pcm, WICED_NO_WAIT );
        if ( result != WICED_SUCCESS )
        {
            WPRINT_APP_INFO ( ("%s: push to queue failed, freeing buffer.\n", __func__) );
#ifdef USE_MEM_POOL
            bt_buffer_pool_free_buffer(pcm);
#else
            free(pcm);
#endif
            return;
        }
        if(player.state == BT_AUDIO_DEVICE_STATE_STARTED)
        {
            return;
        }
        else
        {
            q_count++;
            if( q_count >= BT_AUDIO_PLAYER_QUEUE_THRESHOLD  )
            {
                wiced_rtos_set_event_flags(&player.events, BT_AUDIO_EVENT_START_PLAYER);
                //WPRINT_APP_INFO ( ("%s: Sending Player Start Event, pcm_length = %d\n", __func__, (int)pcm->length) );
                q_count = 0;
            }
        }
    }
    else
    {
#ifdef USE_MEM_POOL
        bt_buffer_pool_free_buffer(pcm);
#else
        free(pcm);
#endif
    }
}


wiced_result_t bt_audio_stop_player( void )
{
    if(player.state <= BT_AUDIO_DEVICE_STATE_IDLE)
        return WICED_SUCCESS;

    //WPRINT_APP_INFO( ("bt_audio_stop_player\n") );

    //Send stop event to reset the player state to IDLE
    wiced_rtos_set_event_flags(&player.events, BT_AUDIO_EVENT_STOP_PLAYER );
    wiced_rtos_get_semaphore(&player.wait_for_cmd_completion, 34); //max time player can wait for buffer @8K

    return WICED_SUCCESS;
}

wiced_result_t bt_audio_update_player_volume( uint8_t level )
{
    wiced_result_t result = WICED_ERROR;
    double min_volume_db = 0.0, max_volume_db = 0.0, volume_db, step_db;

    if(level > BT_AUDIO_VOLUME_MAX)
        level = BT_AUDIO_DEFAULT_VOLUME;

    if ( is_bt_audio_player_initialized() != WICED_TRUE )
    {
        WPRINT_APP_INFO (("bt_audio_update_player_volume: Player not initialized\n"));
         return result;
    }

    result = wiced_audio_get_volume_range( player.bluetooth_audio_session_handle, &min_volume_db, &max_volume_db );
    if ( result != WICED_SUCCESS )
    {
        WPRINT_APP_INFO(("bt_audio_update_player_volume: wiced_audio_get_volume_range failed\n"));
        return result;
    }

    step_db = ( double )( max_volume_db - min_volume_db ) /( BT_AUDIO_VOLUME_MAX);
    volume_db = ( double ) VOLUME_CONVERSION( step_db, level, min_volume_db );

    return wiced_audio_set_volume( player.bluetooth_audio_session_handle, volume_db );
}


static wiced_result_t bt_audio_start( void )
{
    wiced_result_t    result = WICED_ERROR;

    if( is_bt_audio_player_prepared())
        result = wiced_audio_start(player.bluetooth_audio_session_handle);

    if(result == WICED_SUCCESS)
        player.state = BT_AUDIO_DEVICE_STATE_STARTED;

    WPRINT_APP_INFO(("bt_audio_start: Player start state [result: %d]\n",result));
    return result;
}

static void bt_audio_stop( void )
{
    uint8_t stop_audio=0;
    bt_audio_codec_data_t* pcm=NULL;

    if ( ( player.state > BT_AUDIO_DEVICE_STATE_IDLE) )
    {
        if ( player.state == BT_AUDIO_DEVICE_STATE_STARTED)
            stop_audio = 1;

        if(stop_audio)
        {
            wiced_audio_stop(player.bluetooth_audio_session_handle);
            if( WICED_SUCCESS != wiced_rtos_is_queue_empty(&player.queue) )
            {
                while( WICED_SUCCESS == wiced_rtos_pop_from_queue(&player.queue, &pcm, WICED_NO_WAIT) )
                {
                    if(pcm != NULL)
                    {
#ifdef USE_MEM_POOL
                        bt_buffer_pool_free_buffer(pcm);
#else
                        free(pcm);
#endif
                        pcm=NULL;
                    }
                }
            }
        }
        wiced_audio_deinit(player.bluetooth_audio_session_handle);
        player.state = BT_AUDIO_DEVICE_STATE_IDLE;
    }
    wiced_rtos_set_semaphore(&player.wait_for_cmd_completion);
}


wiced_result_t  bt_audio_deinit_player( void )
{
    wiced_result_t ret = WICED_SUCCESS;

    wiced_rtos_deinit_queue(&player.queue);
    wiced_rtos_deinit_event_flags(&player.events);
    wiced_rtos_deinit_semaphore(&player.wait_for_cmd_completion);

    /* reset player control block */
    memset(&player, 0, sizeof(bt_audio_context_t));
    player.state = BT_AUDIO_DEVICE_STATE_UNINITIALIZED;
    return ret;
}


static wiced_bool_t read_bluetooth_audio_data( uint8_t *buffer, uint16_t* size )
{
    static bt_audio_codec_data_t* pcm = NULL;
    uint32_t        current_buffer_pos = 0;
    //uint32_t flags_set;
    wiced_result_t result;
    uint32_t        wait_time_limit;
    wiced_bool_t    done = WICED_FALSE;
    uint32_t copy_len;

    /* If there is a chunk which was not written in the buffer on the previous call */
    /* write it know */
    if(pcm != NULL)
    {
        if( pcm->length != 0)
        {
            memcpy( buffer, (pcm->data+pcm->offset), pcm->length );
            current_buffer_pos = pcm->length;
        }
#ifdef USE_MEM_POOL
            bt_buffer_pool_free_buffer(pcm);
#else
            free(pcm);
#endif
        pcm = NULL;
    }
    else
    {
        current_buffer_pos = 0;
    }

    wait_time_limit = ((double)((*size)*NUM_MSECONDS_IN_SECONDS))/((double)(player.bluetooth_audio_config.sample_rate*4));

    /* Read continuously packets from the bluetooth audio queue */
    while(wait_time_limit > 0)
    {
        result = wiced_rtos_pop_from_queue(&player.queue, &pcm, 1);
        if(result != WICED_SUCCESS)
        {
            wait_time_limit--;
            continue;
        }

        /*Calculate how many bytes can be copied*/
        copy_len = ( ( current_buffer_pos + pcm->length ) <= *size )?pcm->length:( *size - current_buffer_pos );

        memcpy(&buffer[current_buffer_pos], pcm->data, copy_len);
        current_buffer_pos += copy_len;

        if(copy_len == pcm->length)
        {
#ifdef USE_MEM_POOL
            bt_buffer_pool_free_buffer(pcm);
#else
            free(pcm);
#endif
            pcm = NULL;
        }
        else //if(copy_len < pcm->length)
        {
            pcm->offset += copy_len;
            pcm->length -= copy_len;
        }

        if(current_buffer_pos == *size)
        {
            done = WICED_TRUE;
            break;
        }
    }

        if( done != WICED_TRUE )
        {
            /* If the time to fill a buffer with at least one audio period is expired
             * Fill left part of the buffer with silence
             */
            memset(&buffer[current_buffer_pos], 0x00, *size - current_buffer_pos);
            done = WICED_TRUE;
        }

    return done;
}


void bt_audio_player_task(uint32_t args)
{
    uint8_t*          ptr;
    uint16_t          n;
    uint16_t          available_bytes;
    wiced_result_t    result;
    uint32_t   flags_set;

    for ( ;;)
    {
        /* Wait for an audio ready event( all bluetooth audio buffers must be filled before starting playback ) */
        result = wiced_rtos_wait_for_event_flags(&player.events, BT_AUDIO_EVENT_ALL, &flags_set, WICED_TRUE, WAIT_FOR_ANY_EVENT, WICED_WAIT_FOREVER);
        if( result != WICED_SUCCESS )
            continue;

        WPRINT_APP_INFO(("[BT-PLAYER] Start bluetooth playback \n"));
        /* Audio thread is started, now it will read contents of queued bluetooth audio buffers and give it for further processing
         *  to the audio framework */
        while( !(flags_set & BT_AUDIO_EVENT_STOP_PLAYER))
        {
            /* wait till at least one audio period is available for writing */
            if( player.state != BT_AUDIO_DEVICE_STATE_STARTED)
            {
                result = wiced_audio_wait_buffer(player.bluetooth_audio_session_handle, BT_AUDIO_BUFFER_SIZE, 0 );
                wiced_assert( "Cant get a period from the buffer.", result == WICED_SUCCESS );
                available_bytes = BT_AUDIO_BUFFER_SIZE;
            }
            else
            {
                /* Wait till at least one period is avaialable for writing */
                uint32_t wait_time =        ((NUM_USECONDS_IN_SECONDS/player.bluetooth_audio_config.sample_rate) * BT_AUDIO_DEFAULT_PERIOD_SIZE)/NUM_MSECONDS_IN_SECONDS;
                result = wiced_audio_wait_buffer(player.bluetooth_audio_session_handle, BT_AUDIO_DEFAULT_PERIOD_SIZE, wait_time + 100);
                if( result != WICED_SUCCESS )
                {
                    /* Must do a recovery there */
                    wiced_audio_stop(player.bluetooth_audio_session_handle);
                    WPRINT_APP_INFO(("Recover after wait"));
                    player.state = BT_AUDIO_DEVICE_STATE_STOPPED;
                    continue;
                }
                available_bytes = BT_AUDIO_DEFAULT_PERIOD_SIZE;
            }

            while (available_bytes > 0)
            {
                n = available_bytes;
                result = wiced_audio_get_buffer( player.bluetooth_audio_session_handle, &ptr, &n );
                wiced_assert("Cant get an audio buffer", result == WICED_SUCCESS);
                if( result == WICED_ERROR )
                {
                    /* Underrun might have occured, recover by audio restart */
                    wiced_audio_stop(player.bluetooth_audio_session_handle);
                    WPRINT_APP_INFO(("Recover after get"));
                    player.state = BT_AUDIO_DEVICE_STATE_STOPPED;
                    break;
                }
                else if( result != WICED_SUCCESS )
                {
                    WPRINT_APP_INFO(("bt_audio_wiced_audio_player_task: No buffer avail"));
                    break;
                }
                if( n <= 0 )
                {
                    /* must do a recovery there */
                    wiced_audio_stop(player.bluetooth_audio_session_handle);
                    WPRINT_APP_INFO(("Overrun occured"));
                    player.state = BT_AUDIO_DEVICE_STATE_STOPPED;
                    break;
                }

                if(WICED_FALSE == read_bluetooth_audio_data(ptr, &n ))
                    break;

                wiced_assert("Should never return 0 bytes", ( n != 0 ) );
                result = wiced_audio_release_buffer( player.bluetooth_audio_session_handle, n );
                if( result == WICED_ERROR )
                {
                    wiced_audio_stop(player.bluetooth_audio_session_handle);
                    WPRINT_APP_INFO(("Recover after buffer release"));
                    player.state = BT_AUDIO_DEVICE_STATE_STOPPED;
                    break;
                }

                wiced_assert("Cant release an audio slot", result == WICED_SUCCESS);
                available_bytes -= n;
                if (player.state != BT_AUDIO_DEVICE_STATE_STARTED && available_bytes == 0)
                {
                    bt_audio_start();
                }
            } /* while (available_bytes > 0) */
            result = wiced_rtos_wait_for_event_flags(&player.events, BT_AUDIO_EVENT_ALL, &flags_set, WICED_TRUE, WAIT_FOR_ANY_EVENT, WICED_NO_WAIT);
        }
        bt_audio_stop();
        WPRINT_APP_INFO(("[bt_audio_wiced_audio_player_task] out of playback loop\n"));
    } /*end for (;;)*/
}

