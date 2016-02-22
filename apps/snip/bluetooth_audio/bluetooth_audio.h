/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

/** @file
 *
 * Bluetooth A2DP Application Programming Interface
 *
 */
#pragma once

#include "wiced_bt_a2dp_sink.h"
#include "wiced_audio.h"
#include "wiced_codec_if.h"

/*****************************************************************************
**  constants
*****************************************************************************/

#define BT_AUDIO_DEFAULT_VOLUME  63 /* mid of 0-127 */
#define BT_AUDIO_VOLUME_STEP        8
#define BT_AUDIO_VOLUME_MAX         127
#define BT_AUDIO_VOLUME_MIN          0

/*****************************************************************************
 ** enumerations
 *****************************************************************************/

typedef enum
{
    BT_AUDIO_EVENT_START_PLAYER  = 0x00000001,
    BT_AUDIO_EVENT_STOP_PLAYER   = 0x00000002,
    BT_AUDIO_EVENT_ALL           = 0xFFFFFFFF
}player_event_flags_t;


typedef enum
{
    BT_AUDIO_DEVICE_STATE_UNINITIALIZED,
    BT_AUDIO_DEVICE_STATE_IDLE,
    BT_AUDIO_DEVICE_STATE_CONFIGURED,
    BT_AUDIO_DEVICE_STATE_STARTED,
    BT_AUDIO_DEVICE_STATE_STOPPED
}bt_audio_device_state_t;

typedef enum
{
    BT_AUDIO_CODEC_EVENT_DECODE        = 0x00000001,
    BT_AUDIO_CODEC_EVENT_CONFIGURE   = 0x00000002,
    BT_AUDIO_CODEC_EVENT_ALL               = 0xFFFFFFFF
}codec_event_flags_t;

/*****************************************************************************
**  type definitions
*****************************************************************************/
typedef struct
{
    uint16_t  length;
    uint16_t  offset;
    uint8_t   data[]; //data of length size.
}bt_audio_codec_data_t;

typedef struct
{
    uint32_t sample_rate;
    uint8_t  bits_per_sample;
    uint8_t  channels;
    uint8_t  volume;
    uint32_t bit_rate;
} bt_audio_config_t;

typedef struct
{
    wiced_codec_interface_t*    wiced_sbc_if;
    wiced_queue_t         queue;
    wiced_event_flags_t  events;
} bt_audio_decoder_context_t;

typedef struct
{
    wiced_event_flags_t  events;
    wiced_queue_t        queue;

    bt_audio_device_state_t state;
    //uint8_t                   now_playing;
    wiced_audio_session_ref   bluetooth_audio_session_handle;
    bt_audio_config_t         bluetooth_audio_config;
    uint32_t                  audio_buffer_weight;
    uint32_t                  audio_buffer_weight_high_water_mark;
    uint32_t                  overrun_count;      /* Number of times player did not have data to play */
    uint32_t                  cback_count;
    wiced_semaphore_t         wait_for_cmd_completion;
} bt_audio_context_t;

/*****************************************************************************
**  external function declarations
*****************************************************************************/
wiced_result_t  bt_audio_init_player( void );
wiced_result_t bt_audio_configure_player( bt_audio_config_t* p_audio_config );
wiced_result_t bt_audio_decoder_context_init( void );
wiced_result_t bt_audio_reset_decoder_config( void );

void bt_audio_decoder_task( uint32_t arg );
wiced_result_t bt_audio_configure_decoder(wiced_bt_a2dp_codec_info_t* codec_config);
wiced_result_t bt_audio_write_to_decoder_queue(bt_audio_codec_data_t* audio);
void bt_audio_write_to_player_buffer( bt_audio_codec_data_t* pcm );
wiced_result_t bt_audio_stop_player( void );
wiced_result_t bt_audio_update_player_volume( uint8_t level );
void bt_audio_player_task(uint32_t args);


#ifdef __cplusplus
extern "C"
{
#endif


#ifdef __cplusplus
}
#endif
