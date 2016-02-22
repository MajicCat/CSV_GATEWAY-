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
#include "wiced_audio.h"
#include "platform_audio.h"

#include <math.h>

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
#define BUFFER_SIZE                 WICED_AUDIO_BUFFER_ARRAY_DIM_SIZEOF(10, PERIOD_SIZE)

#define EXTRA_MILLIS                (10)
#define TX_START_THRESHOLD          (3*PERIOD_SIZE)

#define SAMPLE_FREQUENCY_IN_HZ      (44100)

#define MICROSECONDS_PER_SECOND     (1000*1000)
#define MILLISECONDS_PER_SECOND     (1000)
#define BITS_PER_BYTE               (8)

#define SINE_WAVE_FREQUENCY_IN_HZ   (1000)
#define SINE_WAVE_VOLUME_PERCENTAGE (0.8F)

#define MAX_DATA_SIZE_IN_BYTES      (1024*2)

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
static wiced_result_t initialize_audio_device( const platform_audio_device_id_t device_id, wiced_audio_config_t* config, uint8_t* buffer, size_t buffer_length, wiced_audio_session_ref* session );
static wiced_result_t loop_iteration( void );

#if ENABLE_SINE_WAVE_OUTPUT
static uint16_t initialize_data( float volume, uint tone_frequency_in_hz, const wiced_audio_config_t* config, uint8_t* buffer, uint16_t buffer_length_in_bytes );
static wiced_result_t copy_data( uint8_t* buffer, uint16_t buffer_length );
static void validate_data( uint8_t* buffer, uint16_t buffer_length );
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
static int                      number_of_iterations;

static wiced_audio_config_t config =
{
    .sample_rate        = SAMPLE_FREQUENCY_IN_HZ,
    .channels           = 2,
    .bits_per_sample    = 16,
    .frame_size         = 4,
};

#if ENABLE_SINE_WAVE_OUTPUT
static int16_t  data[MAX_DATA_SIZE_IN_BYTES/2];
static int      number_of_data_samples = 45;
#endif

/******************************************************
 *               Function Definitions
 ******************************************************/

void application_start( void )
{
    wiced_result_t result;

#if ENABLE_SINE_WAVE_OUTPUT
    /* Initialize sample data. */
    number_of_data_samples = initialize_data( SINE_WAVE_VOLUME_PERCENTAGE, SINE_WAVE_FREQUENCY_IN_HZ, &config, (uint8_t *)data, sizeof data);
#endif

    /* Initialize platform audio. */
    result = platform_init_audio( );
    if ( result != WICED_SUCCESS )
    {
        wiced_assert("platform_init_audio", result == WICED_SUCCESS);
        return;
    }

    /* Initialize TX device. */
    result = initialize_audio_device( TEST_TX_AUDIO_DEVICE, &config, tx_buffer, sizeof(tx_buffer), &tx_session );
    if ( result != WICED_SUCCESS )
    {
        wiced_assert("initialize_audio_device TX", result == WICED_SUCCESS);
        goto exit_with_platform_init;
    }

#ifdef TEST_RX_AUDIO_DEVICE
    /* Initialize RX device. */
    result = initialize_audio_device(TEST_RX_AUDIO_DEVICE, &config, rx_buffer, sizeof(rx_buffer), &rx_session );
    if ( result != WICED_SUCCESS )
    {
        wiced_assert("initialize_audio_device RX", result == WICED_SUCCESS);
        goto exit_with_platform_init;
    }

    /* Start RX. */
    result = wiced_audio_start( rx_session );
    wiced_assert("wiced_audio_start RX", result == WICED_SUCCESS);
#endif

    /* Main loop. */
    while ( result == WICED_SUCCESS )
    {
        number_of_iterations++;
        result = loop_iteration( );
        wiced_assert("loop_iteration", result == WICED_SUCCESS);
    }

exit_with_platform_init:
    platform_deinit_audio();

    return;
}

static wiced_result_t initialize_audio_device( const platform_audio_device_id_t device_id, wiced_audio_config_t* config, uint8_t* buffer, size_t buffer_length, wiced_audio_session_ref* session )
{
    wiced_result_t result = WICED_SUCCESS;

    /* Initialize device. */
    result = wiced_audio_init( device_id, session, PERIOD_SIZE );
    if ( result != WICED_SUCCESS )
    {
        wiced_assert("wiced_audio_init", WICED_SUCCESS == result);
        return result;
    }

    /* Allocate audio buffer. */
    result = wiced_audio_create_buffer( *session, buffer_length, WICED_AUDIO_BUFFER_ARRAY_PTR( buffer ), NULL );
    if ( result != WICED_SUCCESS )
    {
        wiced_assert("wiced_audio_create_buffer", WICED_SUCCESS == result);
        goto exit_with_error;
    }

    /* Configure session. */
    result = wiced_audio_configure( *session, config );
    if ( result != WICED_SUCCESS )
    {
        wiced_assert("wiced_audio_configure", WICED_SUCCESS == result);
        goto exit_with_error;
    }

    return result;

exit_with_error:
    if ( wiced_audio_deinit( *session ) != WICED_SUCCESS )
    {
        wiced_assert("wiced_audio_deinit", 0);
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
        wiced_assert("copy_data", result == WICED_SUCCESS);
        return result;
    }

    /* Validate and eat RX data. */
    if ( rx_session != NULL )
    {
        result = validate_rx_data( buffer_length );
        wiced_assert("validate_rx_data", result == WICED_SUCCESS);
    }

    return result;
}

static uint16_t initialize_data( float volume, uint tone_frequency_in_hz, const wiced_audio_config_t* config, uint8_t* buffer, uint16_t buffer_length_in_bytes )
{
    int         sample_index             = 0;
    const uint  sample_frequency_in_hz  = config->sample_rate;
    const uint  bytes_per_sample        = config->bits_per_sample / 8;
    int16_t     *buf16                  = (int16_t *) buffer;
    uint16_t    total_samples_written   = 0;

    WPRINT_APP_INFO(("%d Hz sine wave, %d sample frequency at %f volume \r\n", tone_frequency_in_hz, sample_frequency_in_hz, volume));
    WPRINT_APP_INFO(("buffer total %d bytes \r\n", buffer_length_in_bytes));
    while (1)
    {
        float rad       = (2.0 * M_PI) * sample_index / (((float)sample_frequency_in_hz / tone_frequency_in_hz));
        float v         = sinf(rad) * volume;
        int16_t data   = (int16_t)(v < 0 ? -INT16_MIN * v : INT16_MAX * v) & 0xFFFF;

        buf16[ sample_index ] = data;

        if ( sample_index > 0 && ((tone_frequency_in_hz * (sample_index+1) % sample_frequency_in_hz) == 0) )
        {
            total_samples_written = sample_index + 1;
            break;
        }

        sample_index++;
        wiced_assert( "frame buffer too small", buffer_length_in_bytes >= (sample_index * bytes_per_sample) );
    }

    WPRINT_APP_INFO(("buffer initialized with %d samples\r\n", total_samples_written));
    return total_samples_written;
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

        if ( ++last_pos >= number_of_data_samples )
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
            wiced_assert("wiced_audio_get_buffer", result == WICED_SUCCESS);
            wiced_assert("bad size", avail <= remaining);
            break;
        }

        if ( do_data_validation != 0 )
        {
            validate_data( buffer, avail );
        }

        result = wiced_audio_release_buffer( rx_session, avail );
        if ( result != WICED_SUCCESS )
        {
            wiced_assert("wiced_audio_release_buffer", result == WICED_SUCCESS);
            break;
        }

        remaining -= avail;
    }

    return result;
}

static void validate_data( uint8_t* buffer, uint16_t buffer_length )
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
            for ( pos = 0; pos < number_of_data_samples; pos++ )
            {
                if ( buf16[ i ] == data[ pos ] )
                {
                    break;
                }
            }
            if ( number_of_data_samples == pos )
            {
                continue;
            }
            if ( pos != number_of_data_samples && buf16[ i + 1 ] == data[ pos ] )
            {
                last_pos = pos;
                wiced_assert("last_pos", allowed == 0);
                break;
            }
        }
    }
    if ( last_pos < 0 )
    {
        --allowed;
        wiced_assert("data pattern not found", allowed != 0 || last_pos >= 0);
        return;
    }

    while ( i < buffer_length / 2 )
    {
        int16_t *buf16 = (int16_t *) buffer;

        if ( buf16[ i++ ] != data[ last_pos ] )
        {
            wiced_assert("invalid data L", 0);
        }
        if ( buf16[ i++ ] != data[ last_pos ] )
        {
            wiced_assert("invalid data R", 0);
        }

        if ( ++last_pos >= number_of_data_samples )
        {
            last_pos = 0;
        }
    }
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
            wiced_assert("wiced_audio_get_buffer", result == WICED_SUCCESS);
            wiced_assert("bad size", avail <= remaining);
            break;
        }

        memcpy( buffer, buf, avail );
        result = wiced_audio_release_buffer( sh, avail );
        if ( result != WICED_SUCCESS )
        {
            wiced_assert("wiced_audio_release_buffer", result == WICED_SUCCESS);
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
        wiced_assert("wiced_audio_get_current_buffer_weight", result == WICED_SUCCESS);
        if ( result == WICED_SUCCESS )
        {
            if ( weight >= TX_START_THRESHOLD )
            {
                if ( result == WICED_SUCCESS )
                {
                    result = wiced_audio_start( tx_session );
                    wiced_assert("wiced_audio_start TX", result == WICED_SUCCESS);
                }
                if ( result == WICED_SUCCESS )
                {
                    is_tx_started = 1;
                }
            }
        }
    }

    /* Wait for data that can be transmitted. */
    /* In the case of canned data, this defines the transmit cadence. */
    if (result == WICED_SUCCESS)
    {
        if (rx_session != NULL)
        {
            result = wiced_audio_wait_buffer(rx_session, PERIOD_SIZE, timeout);
            wiced_assert("wiced_audio_wait_buffer RX", result == WICED_SUCCESS);
        }
    }

    /* Wait for slot in transmit buffer. */
    if (result == WICED_SUCCESS)
    {
        result = wiced_audio_wait_buffer(tx_session, PERIOD_SIZE, timeout);
        wiced_assert("wiced_audio_wait_buffer TX", result == WICED_SUCCESS);
    }

    /* Copy available data to transmit buffer. */
    if (result == WICED_SUCCESS)
    {
        remaining = PERIOD_SIZE;
        while (0 != remaining && result == WICED_SUCCESS)
        {
            uint8_t *buf;
            uint16_t avail = remaining;

            if (result == WICED_SUCCESS)
            {
                result = wiced_audio_get_buffer(tx_session, &buf, &avail);
                wiced_assert("wiced_audio_get_buffer", result == WICED_SUCCESS);
                wiced_assert("bad size", avail <= remaining);
            }

            if (result == WICED_SUCCESS)
            {
                result = get_audio_data(buf, avail);
                wiced_assert("get_data", result == WICED_SUCCESS);
            }

            if (result == WICED_SUCCESS)
            {
                result = wiced_audio_release_buffer(tx_session, avail);
                wiced_assert("wiced_audio_release_buffer", result == WICED_SUCCESS);
                remaining -= avail;
            }
        }
    }

    return result;
}

