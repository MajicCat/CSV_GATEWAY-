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
 * UART test Application
 *
 * This program test the UART functionality.
 *
 * Application Instructions
 *   Connect a PC terminal to the serial port of the WICED Eval board,
 *   then build and download the application as described in the WICED
 *   Quick Start Guide
 *
 *   After download, the app prompts you to enter in hello within a certain time limit.
 *   This test the uart user interface.
 *
 */

#include "wiced.h"
#include "uart_dut.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#ifndef DUT_UART
#define DUT_UART        ( WICED_UART_1 ) //originally 2
#endif

#ifdef FLOW_CONTROL_ENABLE
#define TEST_STRING     ( TEST_STRING_LONG )
#else
#define TEST_STRING     ( TEST_STRING_SHORT )
#endif

#ifdef RING_BUFFER_ENABLE
#define RX_BUFFER_SIZE  ( 64 )
#endif

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

#ifdef RING_BUFFER_ENABLE
wiced_ring_buffer_t rx_buffer;
uint8_t             rx_data[RX_BUFFER_SIZE];
#endif /* RING_BUFFER_ENABLE */

/******************************************************
 *               Function Definitions
 ******************************************************/

void uart_dut()
{
    uint8_t             rx_msg[300];
    wiced_uart_config_t uart_config;

    wiced_core_init();

    uart_config.data_width   = DATA_WIDTH_8BIT;
    uart_config.parity       = NO_PARITY;
    uart_config.stop_bits    = STOP_BITS_1;
#ifdef FLOW_CONTROL_ENABLE
    uart_config.flow_control = FLOW_CONTROL_RTS;
#else /* TEST_RTS_FLOW_CONTROL */
    uart_config.flow_control = FLOW_CONTROL_DISABLED;
#endif
    //uart_config.baud_rate =      115200;
    //WPRINT_APP_INFO( ("Initialise UART...") );

    uint32_t len;
    int i;
    int baud_rates[7];
    baud_rates[0] = 115200;
    baud_rates[1] = 1000000;
    baud_rates[2] = 1500000;
    baud_rates[3] = 2000000;
    baud_rates[4] = 2500000;
    baud_rates[5] = 3000000;
    baud_rates[6] = 4000000;

    for (i = 0; i < 7; i++) {
        uart_config.baud_rate = baud_rates[i];
        WPRINT_APP_INFO( ("Initialise UART with baud rate: %d\n", baud_rates[i]) );
#ifdef RING_BUFFER_ENABLE
        ring_buffer_init(&rx_buffer, rx_data, RX_BUFFER_SIZE );
        wiced_uart_init( DUT_UART, &uart_config, &rx_buffer );
#else /* RING_BUFFER_ENABLE */
        /* Test for UART_4 which is a WLAN core uart */
        wiced_uart_init( DUT_UART, &uart_config, NULL );
#endif /* RING_BUFFER_ENABLE */

        WPRINT_APP_INFO( ("Please enter '%s'\n", TEST_STRING) );

#ifdef FLOW_CONTROL_ENABLE
        /* Receive first part of the test phrase */
        WPRINT_APP_INFO(("Receiving the first part of the test string\r\n"));
        wiced_uart_receive_bytes( DUT_UART, rx_msg, strlen(TEST_STRING) / 2, WICED_WAIT_FOREVER);
        WPRINT_APP_INFO(("Check state of the RTS line. It must have a high logic level\r\n"));
        WPRINT_APP_INFO(("Introducing delay\r\n"));
        wiced_rtos_delay_milliseconds(5000);

        /* Receive the second part of the test phrase after we introduced a very big delay */
        /* during the waiting time the rts line should stay in high logic level */
        WPRINT_APP_INFO(("Receiving the second part of the test phrase\r\n"));
        len = strlen(TEST_STRING) / 2;
        wiced_uart_receive_bytes( DUT_UART, &rx_msg[strlen(TEST_STRING) / 2], &len, WICED_WAIT_FOREVER);
#else /* TEST_RTS_FLOW_CONTROL */
        /* Wait in receive until the same number of characters as the length of the test phrase is received */
        WPRINT_APP_INFO( ("Receiving test phrase...\r\n") );
        len = strlen(TEST_STRING);
        wiced_uart_receive_bytes( DUT_UART, rx_msg, &len, 5000 );
#endif /* TEST_RTS_FLOW_CONTROL */
        rx_msg[strlen(TEST_STRING)] = '\0';
        WPRINT_APP_INFO(("Received! - %s\r\n", rx_msg));

        /* Compare with the test-phrase that test expects */
        if ( strcmp( (const char*) rx_msg, TEST_STRING ) )
        {
            WPRINT_APP_INFO( ("Error: String is incorrect, %s\r\n", rx_msg) );
            WPRINT_APP_INFO( ("UART test failed!\r\n") );
        }
        else
        {
            WPRINT_APP_INFO( ("Posting response...\r\n") );
            wiced_uart_transmit_bytes( DUT_UART, rx_msg, strlen((const char*)(rx_msg)) );
            WPRINT_APP_INFO( ("UART test succeeded!\r\n") );
        }
    };
}
