/*
 * $Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.$
 */

/** @file
 *
 * SPI test Application
 *
 * This application snippet demonstrates how to use the
 * SPI API API to read the ID of a serial flash chip.
 *
 * Features demonstrated
 *  - SPI API
 *
 * Application Instructions
 *   Connect a PC terminal to the serial port of the WICED Eval board,
 *   then build and download the application as described in the WICED
 *   Quick Start Guide
 *
 *   After download, the app initialises the SPI interface and
 *   reads the ID of the SPI flash chip on the WICED eval board
 *
 */

#include "wiced.h"
#include "wiced_platform.h"
#include "spi.h"
#include "platform_config.h"

#if defined(WICED_PLATFORM_INCLUDES_SPI_FLASH)

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
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

int spi( void )
{
    int ret_code = 0;
    wiced_spi_message_segment_t segments[2];
    uint8_t jedec_id_command = 0x9F;
    uint8_t jedec_id_value[3] = { 0, 0, 0 };

    printf("\n*** SPI TEST\n");

    wiced_spi_init( &wiced_spi_flash );

    /* Prepare a message to read spi flash JEDEC ID */
    /* First segment is a write segment */
    segments[0].tx_buffer = &jedec_id_command;
    segments[0].length = 1;
    segments[0].rx_buffer = NULL;

    /* Second segment is a read segment */
    segments[1].tx_buffer = NULL;
    segments[1].length = 3;
    segments[1].rx_buffer = jedec_id_value;

    /* Transfer two segments */
    wiced_spi_transfer( &wiced_spi_flash, segments, 2 );

    /* Print the SPI flash ID to the UART */
    WPRINT_APP_INFO( ( "\r\nFlash ID = %02X %02X %02X\r\n", jedec_id_value[0], jedec_id_value[1], jedec_id_value[2] ) );

    if ( ( jedec_id_value[0] == 0 ) && ( jedec_id_value[1] == 0 ) && ( jedec_id_value[2] == 0 ) )
    {
        ret_code = -1;
    }
    else if ( ( jedec_id_value[0] == 0xFF ) && ( jedec_id_value[1] == 0xFF ) && ( jedec_id_value[2] == 0xFF ) )
    {
        ret_code = -2;
    }

    WPRINT_APP_INFO( ( "\n*** SPI TEST: %s\n", (ret_code == 0) ? "SUCCEED" : "FAILED" ) );

    return ret_code;
}

#else

int spi( void )
{
    WPRINT_APP_INFO( ( "\nSPI test NOT supported\n" ) );
    return 0;
}

#endif
