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
 * OTP test Application
 *
 * This program calls the otp read bit, read word, and dump functions with their platform equivalents.
 *
 * Application Instructions
 *   Connect a PC terminal to the serial port of the WICED Eval board,
 *   then build and download the application as described in the WICED
 *   Quick Start Guide
 *
 *   After download, the app runs through the different otp functions
 *
 */

#include "wwd_debug.h"
#include "RTOS/wwd_rtos_interface.h"
#include "wiced_platform.h"
#include "platform_mcu_peripheral.h"
//#include "command_console.h"
#include "wiced_framework.h"
#include "otp.h"

/******************************************************
 *                      Macros
 ******************************************************/

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
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

static wiced_bool_t otp_initialized = WICED_FALSE;

/******************************************************
 *               Function Definitions
 ******************************************************/

/*!
 ******************************************************************************
 * Calls platform_otp_read_bit()
 *
 * @return  0 for success, otherwise error
 */
void otp_read_bit(unsigned int offset )
{
    uint16_t read_bit = 0;
    //int offset = atoi( argv[ 1 ] );

    if ( offset < 0 )
    {
        WPRINT_APP_INFO(("OTP Bit Offset Invalid\r\n"));
    }

    if (otp_initialized == WICED_FALSE)
    {
        if ( (wiced_result_t)platform_otp_init() != WICED_SUCCESS )
        {
            WPRINT_APP_INFO(("OTP Initialization Error\r\n"));
        }

        otp_initialized = WICED_TRUE;
    }

    if ( (wiced_result_t) platform_otp_read_bit( (unsigned int)offset, &read_bit ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO(("OTP Read Bit Error\r\n"));
    }

    WPRINT_APP_INFO(("\nOTP Bit [%u] = %u\r\n", (unsigned int)offset, (unsigned int)read_bit));
}

/*!
 ******************************************************************************
 * Calls platform_otp_read_word()
 *
 * @return  0 for success, otherwise error
 */
void otp_read_word(unsigned int wn )
{
    uint16_t data = 0;
    //int offset = atoi( argv[ 1 ] );

    if ( wn < 0 )
    {
        WPRINT_APP_INFO(("OTP Word Wn Invalid\r\n"));
    }

    if (otp_initialized == WICED_FALSE)
    {
        if ( (wiced_result_t)platform_otp_init() != WICED_SUCCESS )
        {
            WPRINT_APP_INFO(("OTP Initialization Error\r\n"));
        }

        otp_initialized = WICED_TRUE;
    }

    if ( (wiced_result_t) platform_otp_read_word( (unsigned int)wn, &data ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO(("OTP Read Word Error\r\n"));
    }

    WPRINT_APP_INFO(("\nOTP Word [%u] = %u\r\n", (unsigned int)wn, (unsigned int)data));
}

/*!
 ******************************************************************************
 * Calls platform_otp_dump()
 *
 * @return  0 for success, otherwise error
 */
void otp_dump(int arg )
{

    if (otp_initialized == WICED_FALSE)
    {
        if ( (wiced_result_t)platform_otp_init() != WICED_SUCCESS )
        {
            WPRINT_APP_INFO(("OTP Initialization Error\r\n"));
        }

        otp_initialized = WICED_TRUE;
    }

    if ( (wiced_result_t) platform_otp_dump() != WICED_SUCCESS )
    {
        WPRINT_APP_INFO(("OTP Dump error\r\n"));
    }

    //WPRINT_APP_INFO(("\nOTP Dump [%d] = %d\r\n", (unsigned int)arg, (unsigned int)arg));
}



void test_otp( void )
{
    for(int i = 0; i < 15; i++)
        otp_read_bit(i);
    for(int i = 0; i < 15; i++)
        otp_read_word(i);
    for(int i = 0; i < 15; i++)
        otp_dump(i);
}
