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
 * Hibernation test Application
 *
 * This program sends the board to hibernation for a specified amount of ticks*freq, then
 * wakes up.
 *
 * Application Instructions
 *   Connect a PC terminal to the serial port of the WICED Eval board,
 *   then build and download the application as described in the WICED
 *   Quick Start Guide
 *
 *   After download, the app hibernates.  The board resets to the app that was flashed, which
 *   will confirm if you've returned from hibernation or not
 *
 */

#include "wiced.h"
#include "hibernation.h"
#include "platform_cache.h"
#include "platform_config.h"

#include "i2c/i2c.h"
#include "spi/spi.h"
#include "audio_loopback/audio_loopback.h"

#ifndef PLATFORM_HIB_NOT_AVAILABLE
#define PLATFORM_HIB_NOT_AVAILABLE    0
#endif

#if !PLATFORM_HIB_NOT_AVAILABLE

/******************************************************
 *                      Macros
 ******************************************************/

#define HIBERNATION_PERIOD_MS          3000

#define HIBERNATION_BOOT_DDR_TEST_SIZE ( 1 * 1024 * 1024 )

/******************************************************
 *                    Constants
 ******************************************************/

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

static int hibernation_boot_test_ddr( void );

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/
/*!
 ******************************************************************************
 * Sends platform into hibernation for number of milliseconds.
 * Returning from hibernation is via booting from serial flash, so application must be flashed.
 *
 * IMPORTANT NOTE: Do not try to call this function straight after the boot without delay.
 *                 In hibernation mode JTAG is not available, and if spend in hibernation
 *                 majority of time then it is hard to debug board.
 *                 If this happened please try to use and keep HIB_WAKE pin.
 *
 * @return
 */
void hibernation( void )
{
    uint32_t          freq = platform_hibernation_get_clock_freq( );
    uint32_t          max_ticks = platform_hibernation_get_max_ticks ( );
    uint32_t          ticks_to_wakeup;
    platform_result_t result = PLATFORM_BADARG;

    printf( "Hibernation test application.\n" );
    printf( "\n\n*** To make it work please make sure that application is flashed if not done so! ***\n\n" );
    printf( "Frequency %u maximum ticks 0x%x\n", (unsigned)freq, (unsigned)max_ticks );
    printf( "After short sleep will hibernate for %u ms\n", (unsigned)HIBERNATION_PERIOD_MS );

    host_rtos_delay_milliseconds( 1000 );

    ticks_to_wakeup = freq * HIBERNATION_PERIOD_MS / 1000;
    if ( ticks_to_wakeup > max_ticks )
    {
        printf( "Scheduled ticks number %u is too big\n", (unsigned)ticks_to_wakeup );
    }
    else
    {
        result = platform_hibernation_start( ticks_to_wakeup );
    }

    printf( "Hibernation failure: %d\n", (int)result );
}

/*!
 ******************************************************************************
 * To be called during returning from hibernation
 *
 * @return
 */
void hibernation_boot( void )
{
    if ( platform_hibernation_is_returned_from( ) )
    {
        uint32_t     freq        = platform_hibernation_get_clock_freq( );
        uint32_t     spent_ticks = platform_hibernation_get_ticks_spent( );
        uint32_t     spent_ms    = 1000 * spent_ticks / freq;
        wiced_bool_t success     = WICED_TRUE;

        printf( "Returned from hibernation: spent there %u ticks, %u ms\n", (unsigned)spent_ticks, (unsigned)spent_ms );

        if ( PLATFORM_FEATURE_ENAB(DDR) && ( hibernation_boot_test_ddr() != 0 ) )
        {
            success = WICED_FALSE;
        }

        if ( spi() != 0 )
        {
            success = WICED_FALSE;
        }

        if ( audio_loopback() != 0 )
        {
            success = WICED_FALSE;
        }

        if ( i2c() != 0 )
        {
            success = WICED_FALSE;
        }

        printf( "\n*** HIBERNATION: peripherals tested: %s\n", success ? "SUCCEED" : "FAILED" );
    }
    else
    {
        printf( "Normal boot\n" );
    }
}

static int hibernation_boot_test_ddr( void )
{
    int      ret_code   = 0;
    uint32_t base       = PLATFORM_DDR_BASE(0x0);
    uint32_t size       = platform_ddr_get_size();
    uint8_t  patterns[] = {0xAA, 0x00, 0xFF, 0x55, 0xF0, 0x0F, 0xCC, 0x33};
    uint32_t byte_num;
    int      test_num;

    printf("\n*** DDR TEST\n");

    for ( test_num = 0; test_num < ARRAYSIZE(patterns); test_num++ )
    {
        uint8_t* test_base = (uint8_t*)( ( rand() % size ) + base );
        uint32_t test_size = rand() % MIN( HIBERNATION_BOOT_DDR_TEST_SIZE, ( size - (test_base - (uint8_t*)base ) ) );

        printf( "DDR test %d from %d: pattern 0x%02x addr %p size %u\n", test_num + 1, ARRAYSIZE(patterns), (unsigned)patterns[test_num], test_base, (unsigned)test_size );

        if ( test_size != 0 )
        {
            memset( test_base, patterns[test_num], test_size );

            platform_dcache_clean_and_inv_range( test_base, test_size );

            for  (byte_num = 0; byte_num != test_size; byte_num++ )
            {
                uint8_t val = test_base[byte_num];
                if ( val != patterns[test_num] )
                {
                    printf( "\tTest failed due %u byte is 0x%x while it must be 0x%x\n", (unsigned)byte_num, (unsigned)val, (unsigned)patterns[test_num] );
                    ret_code = -1;
                    break;
                }
            }
        }

        if ( !ret_code )
        {
            printf( "\tTest succeeded\n" );
        }
    }

    printf("\n*** DDR TEST: %s\n", (ret_code == 0) ? "SUCCEED" : "FAILED");

    return ret_code;
}

#else

void hibernation( void )
{
    printf( "\nHibernation test NOT supported\n" );
}

void hibernation_boot( void )
{
}

#endif
