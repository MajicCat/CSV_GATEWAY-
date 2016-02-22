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
 */

#include "wiced.h"
#include "uart_tester.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#ifndef TESTER_UART
#define TESTER_UART         ( WICED_UART_1 )
#endif

#ifdef RING_BUFFER_ENABLE
#define RX_BUFFER_SIZE      ( 64 )
#endif

#ifdef FLOW_CONTROL_ENABLE
#define TEST_STRING         ( TEST_STRING_LONG )
#else
#define TEST_STRING         ( TEST_STRING_SHORT )
#endif

#define RECEIVE_TIMEOUT_MS  ( 20000 )

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

wiced_uart_config_t uart_config =
{
    .baud_rate    = 115200,
    .data_width   = DATA_WIDTH_8BIT,
    .parity       = NO_PARITY,
    .stop_bits    = STOP_BITS_1,
#ifdef FLOW_CONTROL_ENABLE
    .flow_control = FLOW_CONTROL_CTS,
#else
    .flow_control = FLOW_CONTROL_DISABLED,
#endif
};

#ifdef RING_BUFFER_ENABLE
wiced_ring_buffer_t rx_buffer;
uint8_t             rx_data[RX_BUFFER_SIZE];
#endif

/******************************************************
 *               Function Definitions
 ******************************************************/

void application_start( )
{
    uint8_t        rx_message[300];
    wiced_result_t result;
    char           start_character;

    /* Initialise WICED core functions */
    wiced_core_init();

    /* Initialise ring buffer */
#ifdef RING_BUFFER_ENABLE
    ring_buffer_init( &rx_buffer, rx_data, RX_BUFFER_SIZE );
#endif

    /* Initialise UART. A ring buffer is used to hold received characters */
    WPRINT_APP_INFO( ("Initialise UART...\r\n") );

#ifdef RING_BUFFER_ENABLE
    wiced_uart_init( TESTER_UART, &uart_config, &rx_buffer );
#else
    wiced_uart_init( TESTER_UART, &uart_config, NULL );
#endif

    /* Waiting for a trigger to start test */
    WPRINT_APP_INFO( ("Waiting for user to start\r\n") );
    result = wiced_uart_receive_bytes( STDIO_UART, &start_character, 1, WICED_WAIT_FOREVER );

    /* Send a test string to the terminal */
    WPRINT_APP_INFO( ("Sending test string...\r\n") );
    wiced_uart_transmit_bytes( TESTER_UART, TEST_STRING, strlen( TEST_STRING ) );

    /* Wait for a response from the DUT, it should get the same string that is sent */
    WPRINT_APP_INFO( ("Receiving the response...\r\n") );
    if ( ( result = wiced_uart_receive_bytes( TESTER_UART, rx_message, strlen( TEST_STRING ), RECEIVE_TIMEOUT_MS ) ) != WICED_SUCCESS )
    {
        rx_message[ strlen( TEST_STRING ) ] = '\0';
        if ( result == WICED_TIMEOUT )
        {
            WPRINT_APP_INFO( ( "Error: UART receive timeout. Test failed\r\n" ) );
            WPRINT_APP_INFO( ( "Response received : %s\r\n", rx_message ) );
        }
        else
        {
            WPRINT_APP_INFO( ( "Error: Unknown\r\n" ) );
        }

    }
    else
    {
        rx_message[ strlen( TEST_STRING ) ] = '\0';
        WPRINT_APP_INFO( ("Response : %s\r\n", rx_message) );

        /* Print on the console UART, the result of the test */
        if ( strcmp( (const char*) rx_message, TEST_STRING ) == 0 )
        {
            WPRINT_APP_INFO( ( "OK: UART test succeeded\r\n" ) );
        }
        else
        {
            WPRINT_APP_INFO( ( "Error: UART test failed\r\n" ) );
        }
    }
}
