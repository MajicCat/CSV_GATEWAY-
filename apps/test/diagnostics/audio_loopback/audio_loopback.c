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
 * Audio test Application
 *
 * This program tests the board's audio functionality.
 *
 * Application Instructions
 *   Connect a PC terminal to the serial port of the WICED Eval board,
 *   then build and download the application as described in the WICED
 *   Plug in speaker or headphones into the DAC plug
 *   Quick Start Guide
 *
 *   After download, the app initializes audio on platform,
 *   validates if tx or rx, and performs loop iterations
 *
 */

#include "wiced.h"
#include "wiced_audio.h"
#include "platform_audio.h"
#include "audio_loopback.h"
#include "platform_config.h"

#ifndef PLATFORM_NO_I2S
#define PLATFORM_NO_I2S   0
#endif

#if !PLATFORM_NO_I2S

/******************************************************
 *                      Macros
 ******************************************************/

#define BYTES_TO_MILLISECONDS(number_of_bytes)     (((MICROSECONDS_PER_SECOND/config.sample_rate) * number_of_bytes)/MILLISECONDS_PER_SECOND)

/******************************************************
 *                    Constants
 ******************************************************/

/* Transmit a sine wave instead of data from the SDIN line. */
#define ENABLE_SINE_WAVE_OUTPUT   0

/* Enable data validation when I2S SDIN/SDOUT lines are shorted.
 * This is only useful when using ENABLE_SINE_WAVE_OUTPUT.
*/
#define NO_DATA_VALIDATION      1

#define TEST_TX_AUDIO_DEVICE         PLATFORM_DEFAULT_AUDIO_OUTPUT  /* To specify, change to a valid device for the platform
                                                                     *  See: platforms/<platform>/platform.h
                                                                     *       platforms/<platform>/platform_audio.c
                                                                     *       WICED/platform/include/platform_audio.h
                                                                     *  example: for BCM943909WCD1_3.B0, specify
                                                                     *      AUDIO_DEVICE_ID_AK4954_DAC_LINE
                                                                     *  or
                                                                     *      AUDIO_DEVICE_ID_WM8533_DAC_LINE
                                                                     */
#if (ENABLE_SINE_WAVE_OUTPUT == 0 || NO_DATA_VALIDATION == 0)
#define TEST_RX_AUDIO_DEVICE         PLATFORM_DEFAULT_AUDIO_INPUT   /* To specify, change to a valid device for the platform
                                                                     *  See: platforms/<platform>/platform.h
                                                                     *       platforms/<platform>/platform_audio.c
                                                                     *       WICED/platform/include/platform_audio.h
                                                                     *  example: for BCM943909WCD1_3.B0, specify
                                                                     *      AUDIO_DEVICE_ID_AK4954_ADC_LINE
                                                                     *  or
                                                                     *      AUDIO_DEVICE_ID_SPDIF
                                                                     */
#endif


#define PERIOD_SIZE                 (1*1024)
#define BUFFER_SIZE                 WICED_AUDIO_BUFFER_ARRAY_DIM_SIZEOF(6, PERIOD_SIZE)

#define EXTRA_MILLIS                (10)
#define TX_START_THRESHOLD          (3*PERIOD_SIZE)

#define MICROSECONDS_PER_SECOND     (1000*1000)
#define MILLISECONDS_PER_SECOND     (1000)
#define BITS_PER_BYTE               (8)

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

static wiced_result_t get_audio_data( uint8_t* buffer, uint16_t buffer_length );
static wiced_result_t initialize_audio_device( const platform_audio_device_id_t audio_device_id, wiced_audio_config_t* config, uint8_t* buffer, size_t buffer_length, wiced_audio_session_ref* session );
static wiced_result_t loop_iteration( void );

#if ENABLE_SINE_WAVE_OUTPUT
static wiced_result_t copy_data( uint8_t* buffer, uint16_t buffer_length );
static wiced_result_t validate_data( uint8_t* buffer, uint16_t buffer_length );
static wiced_result_t validate_rx_data( uint16_t available_bytes );
#endif

/******************************************************
 *               Variables Definitions
 ******************************************************/

/* FIXME The audio subsystem should tell us what buffer requirements or
 *       do this allocation for us!
 */
static uint8_t                  tx_buffer[BUFFER_SIZE];
#ifdef TEST_RX_AUDIO_DEVICE
static uint8_t                  rx_buffer[BUFFER_SIZE];
#endif
static wiced_audio_session_ref  tx_session;
static wiced_audio_session_ref  rx_session;
static int                      is_tx_started;

static wiced_audio_config_t config =
{
    .sample_rate        = 44100,
    .channels           = 2,
    .bits_per_sample    = 16,
    .frame_size         = 4,
};

/* Sine wave at 980Hz */
static const int16_t data[45] =
{
    0xffff,0x0e42,0x1c38,0x29a8,0x3643,0x41d1,0x4c1b,0x54e3,
    0x5c0b,0x6162,0x64d9,0x6655,0x65d9,0x635a,0x5ef2,0x58af,
    0x50af,0x4725,0x3c2e,0x3015,0x2304,0x154c,0x0723,0xf8dd,
    0xeab5,0xdcf9,0xcfef,0xc3cf,0xb8dc,0xaf52,0xa74e,0xa112,
    0x9ca2,0x9a2b,0x99a7,0x9b2b,0x9e99,0xa3fa,0xab19,0xb3e9,
    0xbe2b,0xc9c0,0xd656,0xe3c9,0xf1be
};

/******************************************************
 *               Function Definitions
 ******************************************************/

/*!
 ******************************************************************************
 * Initializes audio on platform, validates if tx or rx, and performs loop iterations
 *
 * @return
 */
int audio_loopback( void )
{
    static wiced_bool_t audio_inited = WICED_FALSE;
    int ret_code = 0;
    wiced_result_t result;
    int counter;

    WPRINT_APP_INFO(("\n*** AUDIO LOOPBACK TEST\n"));

    /* Initialize platform audio. */
    if ( !audio_inited )
    {
        result = platform_init_audio( );
        if ( result != WICED_SUCCESS )
        {
            WPRINT_APP_INFO(("Init device failed\n"));
            ret_code = -1;
            goto exit_platform_init_failed;
        }
        else
        {
            WPRINT_APP_INFO(("Init device success\n"));
            audio_inited = WICED_TRUE;
        }
    }

    /* Initialize TX device. */
    result = initialize_audio_device( TEST_TX_AUDIO_DEVICE, &config, tx_buffer, sizeof(tx_buffer), &tx_session );
    if ( result != WICED_SUCCESS )
    {
        ret_code = -2;
        WPRINT_APP_INFO(("initialize_audio_device TX failed\n"));
        goto exit_tx_init_failed;
    }
    else
    {
        WPRINT_APP_INFO(("Init TX success\n"));
    }

#ifdef TEST_RX_AUDIO_DEVICE
    /* Initialize RX device. */
    result = initialize_audio_device( TEST_RX_AUDIO_DEVICE, &config, rx_buffer, sizeof(rx_buffer), &rx_session );
    if ( result != WICED_SUCCESS )
    {
        ret_code = -3;
        WPRINT_APP_INFO(("initialize_audio_device RX failed\n"));
        goto exit_rx_init_failed;
    }
    else
    {
        WPRINT_APP_INFO(("Init RX success\n"));
    }

    /* Start RX. */
    WPRINT_APP_INFO(("Starting audio\n"));
    result = wiced_audio_start( rx_session );
    if ( result != WICED_SUCCESS )
    {
        ret_code = -4;
        WPRINT_APP_INFO(("wiced_audio_start RX failed\n"));
        goto exit_rx_start_failed;
    }
    else
    {
        WPRINT_APP_INFO(("Audio started\n"));
    }
#endif

    /* Main loop. */
    WPRINT_APP_INFO(("Starting main loop\n"));
    for ( counter = 0; counter < 10; counter++ )
    {
        result = loop_iteration( );
        if (result != WICED_SUCCESS)
        {
            ret_code = -5;
            break;
        }
    }

    if ( is_tx_started )
    {
        wiced_audio_stop( tx_session);
        is_tx_started = 0;
    }

#ifdef TEST_RX_AUDIO_DEVICE
    wiced_audio_stop( rx_session);

exit_rx_start_failed:
    wiced_audio_deinit( rx_session);

exit_rx_init_failed:
#endif
    wiced_audio_deinit( tx_session);

exit_tx_init_failed:
    /*
     * May not support deiniting (43909 does not). In this case keep platform audio initialized.
     * But this is not good as audio including codec is not re-initialized, and other tests (particular I2C test)
     * is talking to codec and may break current test when current test run next time without power-cycling.
     */
    if ( platform_deinit_audio() == WICED_SUCCESS )
    {
        /* Deiniting supported. */
        audio_inited = WICED_FALSE;
    }

exit_platform_init_failed:
    WPRINT_APP_INFO(("\n*** AUDIO LOOPBACK TEST: %s\n", (ret_code == 0) ? "SUCCEED" : "FAILED"));

    return ret_code;
}

static wiced_result_t initialize_audio_device( const platform_audio_device_id_t audio_device_id, wiced_audio_config_t* config, uint8_t* buffer, size_t buffer_length, wiced_audio_session_ref* session )
{
    wiced_result_t result = WICED_SUCCESS;

    /* Initialize device. */
    result = wiced_audio_init( audio_device_id, session, PERIOD_SIZE );
    if ( result != WICED_SUCCESS )
    {
        WPRINT_APP_INFO(("wiced_audio_init failed\n"));
        return result;
    }

    /* Allocate audio buffer. */
    result = wiced_audio_create_buffer( *session, buffer_length, WICED_AUDIO_BUFFER_ARRAY_PTR( buffer ), NULL );
    if ( result != WICED_SUCCESS )
    {
        WPRINT_APP_INFO(("wiced_audio_create_buffer failed\n"));
        goto exit_with_error;
    }

    /* Configure session. */
    result = wiced_audio_configure( *session, config );
    if ( result != WICED_SUCCESS )
    {
        WPRINT_APP_INFO(("wiced_audio_configure failed\n"));
        goto exit_with_error;
    }

    return result;

exit_with_error:
    if ( wiced_audio_deinit( *session ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO(("wiced_audio_deinit failed\n"));
    }

    return result;
}

#if ENABLE_SINE_WAVE_OUTPUT
/* Copy audio buffer from memory to buffer of given size. */
static wiced_result_t get_audio_data( uint8_t* buffer, uint16_t buffer_length )
{
    wiced_result_t result;

    /* Copy predefined data. */
    result = copy_data( buffer, buffer_length );
    if ( result != WICED_SUCCESS )
    {
        WPRINT_APP_INFO(("copy_data failed\n"));
        return result;
    }

    /* Validate and eat RX data. */
    if ( rx_session != NULL )
    {
        result = validate_rx_data( buffer_length );
        if ( result != WICED_SUCCESS )
        {
            WPRINT_APP_INFO(("validate_rx_data failed"));
            return result;
        }
    }

    return result;
}

static wiced_result_t copy_data( uint8_t* buffer, uint16_t buffer_length )
{
    int i;
    static int last_pos;

    for ( i = 0; i < buffer_length / 2; )
    {
        int16_t *buf16 = (int16_t *) buffer;
        buf16[ i++ ] = data[ last_pos ];
        buf16[ i++ ] = data[ last_pos ];

        if ( ++last_pos >= sizeof( data ) / sizeof( data[ 0 ] ) )
        {
            last_pos = 0;
        }
    }

    return WICED_SUCCESS;
}

static wiced_result_t validate_rx_data( uint16_t available_bytes )
{
    wiced_result_t  result = WICED_SUCCESS;
    uint16_t        remaining = available_bytes;
    int             do_data_validation;

    do_data_validation = (is_tx_started != 0 && NO_DATA_VALIDATION == 0) ? 1 : 0;

    while ( remaining != 0 && result == WICED_SUCCESS )
    {
        uint8_t* buffer;
        uint16_t avail = remaining;

        result = wiced_audio_get_buffer( rx_session, &buffer, &avail );
        if ( result != WICED_SUCCESS )
        {
            WPRINT_APP_INFO(("wiced_audio_get_buffer failed\n"));
            break;
        }
        if ( avail > remaining )
        {
            WPRINT_APP_INFO(("bad buffer size\n"));
            result = WICED_ERROR;
            break;
        }

        if ( do_data_validation != 0 )
        {
            result = validate_data( buffer, avail );
            if ( result != WICED_SUCCESS )
            {
                WPRINT_APP_INFO(("validate_data failed\n"));
                break;
            }
        }

        result = wiced_audio_release_buffer( rx_session, avail );
        if ( result != WICED_SUCCESS )
        {
            WPRINT_APP_INFO(("wiced_audio_release_buffer failed\n"));
            break;
        }

        remaining -= avail;
    }

    return result;
}

static wiced_result_t validate_data( uint8_t* buffer, uint16_t buffer_length )
{
    int i = 0;
    static int last_pos = -1;
    static int allowed = 10;

    if ( last_pos < 0 )
    {
        int pos = 0;
        for ( i = 0; i < buffer_length / 2; i += 2 )
        {
            int16_t *buf16 = (int16_t *) buffer;
            for ( pos = 0; pos < sizeof( data ) / sizeof( data[ 0 ] ); pos++ )
            {
                if ( buf16[ i ] == data[ pos ] )
                {
                    break;
                }
            }
            if ( sizeof( data ) / sizeof( data[ 0 ] ) == pos )
            {
                continue;
            }
            if ( pos != sizeof( data ) / sizeof( data[ 0 ] ) && buf16[ i + 1 ] == data[ pos ] )
            {
                last_pos = pos;
                if ( allowed != 0 )
                {
                    WPRINT_APP_INFO(("last_pos is bad\n"));
                    return WICED_ERROR;
                }
                break;
            }
        }
    }
    if ( last_pos < 0 )
    {
        --allowed;
        if ( allowed == 0 )
        {
            WPRINT_APP_INFO(("data pattern not found\n"));
            return WICED_ERROR;
        }

        return WICED_SUCCESS;
    }

    while ( i < buffer_length / 2 )
    {
        int16_t *buf16 = (int16_t *) buffer;

        if ( buf16[ i++ ] != data[ last_pos ] )
        {
            WPRINT_APP_INFO(("invalid data L\n"));
            return WICED_ERROR;
        }
        if ( buf16[ i++ ] != data[ last_pos ] )
        {
            WPRINT_APP_INFO(("invalid data R\n"));
            return WICED_ERROR;
        }

        if ( ++last_pos >= sizeof( data ) / sizeof( data[ 0 ] ) )
        {
            last_pos = 0;
        }
    }

    return WICED_SUCCESS;
}

#else
/* Copy audio buffer from session to buffer of given size. */
static wiced_result_t get_audio_data( uint8_t* buffer, uint16_t buffer_length )
{
    wiced_result_t          result;
    uint16_t                remaining = buffer_length;
    wiced_audio_session_ref sh        = rx_session;

    result = WICED_SUCCESS;

    while ( 0 != remaining && result == WICED_SUCCESS )
    {
        uint8_t *buf;
        uint16_t avail = remaining;

        result = wiced_audio_get_buffer( sh, &buf, &avail );
        if ( result != WICED_SUCCESS )
        {
            WPRINT_APP_INFO(("wiced_audio_get_buffer failed\n"));
            break;
        }
        if ( avail > remaining )
        {
            WPRINT_APP_INFO(("bad size\n"));
            result = WICED_ERROR;
            break;
        }

        memcpy( buffer, buf, avail );
        result = wiced_audio_release_buffer( sh, avail );
        if ( result != WICED_SUCCESS )
        {
            WPRINT_APP_INFO(("wiced_audio_release_buffer failed\n"));
            break;
        }
        buffer    += avail;
        remaining -= avail;
    }

    return result;
}
#endif

static wiced_result_t loop_iteration( void )
{
    wiced_result_t result;
    uint16_t       remaining;
    const uint32_t timeout = BYTES_TO_MILLISECONDS(PERIOD_SIZE) + EXTRA_MILLIS;

    result = WICED_SUCCESS;

    /* Start data transmission. */
    if ( !is_tx_started )
    {
        uint32_t weight;

        /* Determine if we should start TX. */
        result = wiced_audio_get_current_buffer_weight( tx_session, &weight );
        if ( result != WICED_SUCCESS )
        {
            WPRINT_APP_INFO(("wiced_audio_get_current_buffer_weight failed\n"));
            return result;
        }
        if ( weight >= TX_START_THRESHOLD )
        {
            result = wiced_audio_start( tx_session );
            if ( result != WICED_SUCCESS )
            {
                WPRINT_APP_INFO(("wiced_audio_start TX failed\n"));
                return result;
            }

            WPRINT_APP_INFO(("TX started\n"));

            is_tx_started = 1;
        }
    }

    /* Wait for data that can be transmitted. */
    /* In the case of canned data, this defines the transmit cadence. */
    if (rx_session != NULL)
    {
        result = wiced_audio_wait_buffer(rx_session, PERIOD_SIZE, timeout);
        if ( result != WICED_SUCCESS )
        {
            WPRINT_APP_INFO(("wiced_audio_wait_buffer RX failed\n"));
            return result;
        }
    }
    else
    {
        WPRINT_APP_INFO(("rx session is NULL\n"));
    }

    /* Wait for slot in transmit buffer. */
    result = wiced_audio_wait_buffer(tx_session, PERIOD_SIZE, timeout);
    if ( result != WICED_SUCCESS )
    {
        WPRINT_APP_INFO(("wiced_audio_wait_buffer TX failed\n"));
        return result;
    }

    /* Copy available data to transmit buffer. */
    remaining = PERIOD_SIZE;
    while (0 != remaining && result == WICED_SUCCESS)
    {
        uint8_t *buf;
        uint16_t avail = remaining;

        result = wiced_audio_get_buffer(tx_session, &buf, &avail);
        if ( result != WICED_SUCCESS )
        {
            WPRINT_APP_INFO(("wiced_audio_get_buffer failed\n"));
            return result;
        }
        if ( avail > remaining )
        {
            WPRINT_APP_INFO(("bad size\n"));
            return WICED_ERROR;
        }

        result = get_audio_data(buf, avail);
        if ( result != WICED_SUCCESS )
        {
            WPRINT_APP_INFO(("get_data failed\n"));
            return result;
        }

        result = wiced_audio_release_buffer(tx_session, avail);
        if ( result != WICED_SUCCESS )
        {
            WPRINT_APP_INFO(("wiced_audio_release_buffer failed\n"));
            return result;
        }

        remaining -= avail;
    }

    return WICED_SUCCESS;
}

#else

int audio_loopback( void )
{
    WPRINT_APP_INFO( ( "\nAudio loopback test NOT supported\n" ) );
    return 0;
}

#endif
