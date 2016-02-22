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
 * SDIO test Application
 *
 * This program test the SDIO functionality.
 *
 * Application Instructions
 *   Insert a SDIO card and connect a PC terminal to the serial port of
 *   the WICED Eval board, then build and download the application as described
 *   in the WICED Quick Start Guide
 *
 *   After download, the app initialises the SDIO interface and
 *   dump Card Common Control Registers(CCCR) of the SDIO card
 *
 */


#include <typedefs.h>
#include "wiced.h"
#include "wiced_platform.h"
#include "platform_config.h"
#include "platform_sdio.h"

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
/* field offset in structure */
#ifndef FIELD_OFFSET
#define FIELD_OFFSET(type, field)    ((long)(long *)&(((type *)0)->field))
#endif

/******************************************************
 *                    Structures
 ******************************************************/
typedef struct
{
    uint8_t     cccr_rev;
    uint8_t     sd_spec_rev;
    uint8_t     io_enable;
    uint8_t     io_ready;
    uint8_t     int_enable;
    uint8_t     int_pending;
    uint8_t     io_abort;
    uint8_t     bus_if_ctrl;
    uint8_t     capabilities;
    uint8_t     cis_ptr_0;
    uint8_t     cis_ptr_1;
    uint8_t     cis_ptr_2;
    uint8_t     bus_suspend;
    uint8_t     func_select;
    uint8_t     exec_flags;
    uint8_t     ready_flags;
    uint8_t     f0_blk_size_0;
    uint8_t     f0_blk_size_1;
    uint8_t     power_ctrl;
    uint8_t     bus_speed_select;
    uint8_t     uhsi_support;
    uint8_t     driver_strength;
    uint8_t     intr_extension;
} sdio_cccr_t;

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/
static wiced_result_t sdio_dump_device_cccr( void );
void sdio_client_check_isr();

int sdio_test( void )
{
    printf( "SDIO Test Application\n");

    if ( platform_sdio_host_init( sdio_client_check_isr ) != WICED_SUCCESS )
    {
        printf( "SDIO Host init FAIL\n" );
        return -1;
    }

    printf( "SDIO Host init SUCCESS\n" );

    platform_sdio_host_enable_high_speed();

    if ( sdio_dump_device_cccr() != WICED_SUCCESS )
    {
        printf( "Dump CCCR FAIL\n" );
        platform_sdio_host_deinit();
        return -2;
    }

    return 0;
}

static wiced_result_t sdio_dump_device_cccr( void )
{
    uint32_t    address, sdio_response;
    sdio_cccr_t cccr;
    wiced_result_t result;
    sdio_cmd_argument_t arg;

    for ( address = 0; address < sizeof(cccr); address++ )
    {
        uint8_t *value;
        value = ((uint8_t*)&cccr)+address;

        arg.value = 0;
        arg.cmd52.function_number  = 0;
        arg.cmd52.register_address = (unsigned int) ( address & 0x00001ffff );
        arg.cmd52.rw_flag = (unsigned int) ( BUS_READ );
        arg.cmd52.write_data = 0;

        result = platform_sdio_host_transfer( BUS_READ, SDIO_CMD_52, SDIO_BYTE_MODE, SDIO_1B_BLOCK, arg.value, 0, 0, RESPONSE_NEEDED, &sdio_response );
        if ( result != WICED_SUCCESS )
        {
            printf( "platform_sdio_host_transfer FAIL\n" );
            return WICED_ERROR;
        }
        *value = sdio_response;
    }

    /* Dump CCCR information */
    printf( "Card Common Control Registers:\n" );
    printf( "cccr_rev         (0x%02lX) = 0x%02X\n", FIELD_OFFSET(sdio_cccr_t, cccr_rev), cccr.cccr_rev );
    printf( "sd_spec_rev      (0x%02lX) = 0x%02X\n", FIELD_OFFSET(sdio_cccr_t, sd_spec_rev), cccr.sd_spec_rev );
    printf( "io_enable        (0x%02lX) = 0x%02X\n", FIELD_OFFSET(sdio_cccr_t, io_enable), cccr.io_enable );
    printf( "io_ready         (0x%02lX) = 0x%02X\n", FIELD_OFFSET(sdio_cccr_t, io_ready), cccr.io_ready );
    printf( "int_enable       (0x%02lX) = 0x%02X\n", FIELD_OFFSET(sdio_cccr_t, int_enable), cccr.int_enable );
    printf( "int_pending      (0x%02lX) = 0x%02X\n", FIELD_OFFSET(sdio_cccr_t, int_pending), cccr.int_pending );
    printf( "io_abort         (0x%02lX) = 0x%02X\n", FIELD_OFFSET(sdio_cccr_t, io_abort), cccr.io_abort );
    printf( "bus_if_ctrl      (0x%02lX) = 0x%02X\n", FIELD_OFFSET(sdio_cccr_t, bus_if_ctrl), cccr.bus_if_ctrl );
    printf( "capabilities     (0x%02lX) = 0x%02X\n", FIELD_OFFSET(sdio_cccr_t, capabilities), cccr.capabilities );
    printf( "cis_ptr_0        (0x%02lX) = 0x%02X\n", FIELD_OFFSET(sdio_cccr_t, cis_ptr_0), cccr.cis_ptr_0 );
    printf( "cis_ptr_1        (0x%02lX) = 0x%02X\n", FIELD_OFFSET(sdio_cccr_t, cis_ptr_1), cccr.cis_ptr_1 );
    printf( "cis_ptr_2        (0x%02lX) = 0x%02X\n", FIELD_OFFSET(sdio_cccr_t, cis_ptr_2), cccr.cis_ptr_2 );
    printf( "bus_suspend      (0x%02lX) = 0x%02X\n", FIELD_OFFSET(sdio_cccr_t, bus_suspend), cccr.bus_suspend );
    printf( "func_select      (0x%02lX) = 0x%02X\n", FIELD_OFFSET(sdio_cccr_t, func_select), cccr.func_select );
    printf( "exec_flags       (0x%02lX) = 0x%02X\n", FIELD_OFFSET(sdio_cccr_t, exec_flags), cccr.exec_flags );
    printf( "ready_flags      (0x%02lX) = 0x%02X\n", FIELD_OFFSET(sdio_cccr_t, ready_flags), cccr.ready_flags );
    printf( "f0_blk_size_0    (0x%02lX) = 0x%02X\n", FIELD_OFFSET(sdio_cccr_t, f0_blk_size_0), cccr.f0_blk_size_0 );
    printf( "f0_blk_size_1    (0x%02lX) = 0x%02X\n", FIELD_OFFSET(sdio_cccr_t, f0_blk_size_1), cccr.f0_blk_size_1 );
    printf( "power_ctrl       (0x%02lX) = 0x%02X\n", FIELD_OFFSET(sdio_cccr_t, power_ctrl), cccr.power_ctrl );
    printf( "bus_speed_select (0x%02lX) = 0x%02X\n", FIELD_OFFSET(sdio_cccr_t, bus_speed_select), cccr.bus_speed_select );
    printf( "uhsi_support     (0x%02lX) = 0x%02X\n", FIELD_OFFSET(sdio_cccr_t, uhsi_support), cccr.uhsi_support );
    printf( "driver_strength  (0x%02lX) = 0x%02X\n", FIELD_OFFSET(sdio_cccr_t, driver_strength), cccr.driver_strength );
    printf( "intr_extension   (0x%02lX) = 0x%02X\n", FIELD_OFFSET(sdio_cccr_t, intr_extension), cccr.intr_extension );

    return WICED_SUCCESS;
}


/* Client Interrupt handler */
void sdio_client_check_isr()
{
    /*  Handle client interrupt here. The sequence should be:
     *  1. disable client interrupt
     *  2. signal client thread by mutex or semaphore to indicate interrupt arrives
     *  3. process interrupt in client thread
     *  4. enable client interrupt
     */
}


