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
 * LPC43xx UART implementation
 */
#include <string.h>
#include "platform_peripheral.h"
#include "RTOS/wwd_rtos_interface.h"
#include "wwd_assert.h"
#include "chip.h"

/******************************************************
 *                      Macros
 ******************************************************/
/* USART0,2,3 */
#define NUMBER_OF_UART_PORTS     (4)

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

typedef struct
{
    uint8_t databits;
    uint8_t parity;
    uint8_t stopbits;
}config_uart_data_t;

/******************************************************
 *               Function Declarations
 ******************************************************/
static uint32_t platform_uart_get_config_data ( const platform_uart_config_t* config );
static uint8_t  platform_uart_get_port_number ( const LPC_USART_T* uart_base );

/******************************************************
 *                    Variables
 ******************************************************/
/* Uart runtime objects. */
static const IRQn_Type uart_irq_vectors[NUMBER_OF_UART_PORTS] =
{
    [0] = USART0_IRQn,
    [1] = UART1_IRQn,
    [2] = USART2_IRQn,
    [3] = USART3_IRQn,
};

/******************************************************
 *               Function Definitions
 ******************************************************/

platform_result_t platform_uart_init( platform_uart_driver_t* driver, const platform_uart_t* interface, const platform_uart_config_t* config, wiced_ring_buffer_t* optional_ring_buffer )
{
    LPC_USART_T * uart_base   = (LPC_USART_T*) interface->uart_base;
    uint32_t      uart_number = platform_uart_get_port_number( uart_base );

    driver->rx_size              = 0;
    driver->tx_size              = 0;
    driver->last_transmit_result = PLATFORM_SUCCESS;
    driver->last_receive_result  = PLATFORM_SUCCESS;
    driver->interface            = (platform_uart_t*) interface;
    host_rtos_init_semaphore( &driver->tx_complete );
    host_rtos_init_semaphore( &driver->rx_complete );
    platform_pin_set_alternate_function( &interface->tx_pin );
    platform_pin_set_alternate_function( &interface->rx_pin );

    /* Initialize USART peripheral */
    Chip_UART_Init( uart_base );
    Chip_UART_SetBaud( uart_base, config->baud_rate );
    Chip_UART_ConfigData( uart_base, platform_uart_get_config_data( config ) );
    Chip_UART_TXEnable( uart_base );

    /* Enable receive data and line status interrupt */
    Chip_UART_SetupFIFOS(uart_base, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV0 | UART_FCR_TX_RS | UART_FCR_RX_RS));
    Chip_UART_IntEnable(uart_base, (UART_IER_RBRINT | UART_IER_RLSINT));

    /* Disabling IRQ for now. */

    NVIC_SetPriority(uart_irq_vectors[uart_number], 0x8);
    NVIC_EnableIRQ(uart_irq_vectors[uart_number]);

    if ( optional_ring_buffer != NULL )
    {
        /* Note that the ring_buffer should've been initialized first */
        driver->rx_buffer = optional_ring_buffer;
        driver->rx_size   = 0;
    }

    return PLATFORM_SUCCESS;
}

platform_result_t platform_uart_transmit_bytes( platform_uart_driver_t* driver, const uint8_t* data_out, uint32_t size )
{
    LPC_USART_T * uart_base     = (LPC_USART_T*) driver->interface->uart_base;
    Chip_UART_SendBlocking( uart_base, data_out, (int) size );
    return WICED_SUCCESS;
}

platform_result_t platform_uart_receive_bytes( platform_uart_driver_t* driver, uint8_t* data_in, uint32_t* expected_data_size, uint32_t timeout_ms )
{

    /* This is a very dodgy implementation for reading from UART, just to get console APP up
     * Should definitly be replaced later.
     */
    if ( driver->rx_buffer != NULL )
    {
        while ( *expected_data_size != 0 )
        {
            uint32_t transfer_size = MIN(driver->rx_buffer->size / 2, *expected_data_size);

            /* Check if ring buffer already contains the required amount of data. */
            if ( transfer_size > ring_buffer_used_space( driver->rx_buffer ) )
            {
                /* Set rx_size and wait in rx_complete semaphore until data reaches rx_size or timeout occurs */
                driver->rx_size = transfer_size;

                if ( host_rtos_get_semaphore( &driver->rx_complete, timeout_ms, WICED_FALSE ) != WWD_SUCCESS )
                {
                    driver->rx_size = 0;
                    return PLATFORM_TIMEOUT;
                }

                /* Reset rx_size to prevent semaphore being set while nothing waits for the data */
                driver->rx_size = 0;
            }

            *expected_data_size -= transfer_size;

            // Grab data from the buffer
            do
            {
                uint8_t* available_data;
                uint32_t bytes_available;

                ring_buffer_get_data( driver->rx_buffer, &available_data, &bytes_available );
                bytes_available = MIN( bytes_available, transfer_size );
                memcpy( data_in, available_data, bytes_available );
                transfer_size -= bytes_available;
                data_in = ( (uint8_t*)data_in + bytes_available );
                ring_buffer_consume( driver->rx_buffer, bytes_available );
            }
            while ( transfer_size != 0 );
        }

        if ( *expected_data_size != 0 )
        {
            return PLATFORM_ERROR;
        }
        else
        {
            return PLATFORM_SUCCESS;
        }
    }
    else
    {
        /* TODO: need to implement this */
        return PLATFORM_UNSUPPORTED;
    }
}

static uint32_t platform_uart_get_config_data ( const platform_uart_config_t* config )
{
    uint32_t    config_data = 0x00000000;
    switch (config->data_width)
    {
        case DATA_WIDTH_5BIT:
            config_data |= UART_LCR_WLEN5;
            break;
        case DATA_WIDTH_6BIT:
            config_data |= UART_LCR_WLEN6;
            break;
        case DATA_WIDTH_7BIT:
            config_data |= UART_LCR_WLEN7;
            break;
        default:
            config_data |= UART_LCR_WLEN8;
            wiced_assert( "data width not supported", config->data_width == DATA_WIDTH_8BIT );
    }
    config_data |= config->stop_bits == STOP_BITS_1 ? UART_LCR_SBS_1BIT : UART_LCR_SBS_2BIT;
    config_data |= config->parity == NO_PARITY ? UART_LCR_PARITY_DIS : UART_LCR_PARITY_EN;
    //config_data |= config->parity == ODD_PARITY? UART_LCR_PARITY_ODD : UART_LCR_PARITY_EVEN;

    return config_data;
}

static uint8_t  platform_uart_get_port_number ( const LPC_USART_T* uart_base )
{
    if ( uart_base == LPC_USART0 )
    {
        return 0;
    }
    else if ( uart_base == LPC_UART1 )
    {
        return 1;
    }
    else if ( uart_base == LPC_USART2 )
    {
        return 2;
    }
    else if ( uart_base == LPC_USART3 )
    {
        return 3;
    }
    else
    {
        return 0xff;
    }
}

/******************************************************
 *            IRQ Handlers Definition
 ******************************************************/

void platform_uart_irq( platform_uart_driver_t* driver )
{
    LPC_USART_T* uart = (LPC_USART_T*) driver->interface->uart_base;

    /* New data will be ignored if data not popped in time */
    while ( Chip_UART_ReadLineStatus(uart) & ( UART_LSR_RDR ) ) {
        uint8_t ch = Chip_UART_ReadByte(uart);
        ring_buffer_write( driver->rx_buffer,&ch, 1 );
    }

    // Notify thread if sufficient data are available
    if ( ( driver->rx_size > 0 ) && ( ring_buffer_used_space( driver->rx_buffer ) >= driver->rx_size ) )
    {
        host_rtos_set_semaphore( &driver->rx_complete, WICED_TRUE );
        driver->rx_size = 0;
    }
}
