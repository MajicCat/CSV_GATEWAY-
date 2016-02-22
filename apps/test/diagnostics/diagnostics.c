/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

#include <stdlib.h>
#include <string.h>

#include "wiced.h"

#include "otp/otp.h"
#include "audio_loopback/audio_loopback.h"
#include "uart/dut/uart_dut.h"
#include "i2c/i2c.h"
#include "hibernation/hibernation.h"
#include "hwcrypto_test/hwcrypto_test.h"
#include "cpu_ddr/cpu_ddr.h"
#include "gpio/gpio.h"
#include "spi/spi.h"
#include "gci/gci.h"
#include "usb/usb.h"
#include "pwm/pwm.h"
#include "sdio/sdio.h"
#include "gmac_loopback/gmac_loopback.h"

#include "command_console.h"
#include "wiced_management.h"
#include "command_console_thread.h"
#include "command_console_platform.h"

#include "wiced_framework.h"

#define MAX_LINE_LENGTH  (128)
#define MAX_HISTORY_LENGTH (20)
#define MAX_NUM_COMMAND_TABLE  (8)
#define CONSOLE_THREAD_STACK_SIZE    (6*1024)

static char line_buffer[MAX_LINE_LENGTH];
static char history_buffer_storage[MAX_LINE_LENGTH * MAX_HISTORY_LENGTH];

/* definition for test_uart */
#ifndef DUT_UART
#define DUT_UART        ( WICED_UART_2 )
#endif

#ifdef FLOW_CONTROL_ENABLE
#define TEST_STRING     ( TEST_STRING_LONG )
#else
#define TEST_STRING     ( TEST_STRING_SHORT )
#endif

#ifdef RING_BUFFER_ENABLE
#define RX_BUFFER_SIZE  ( 64 )
#endif

#ifdef RING_BUFFER_ENABLE
wiced_ring_buffer_t rx_buffer;
uint8_t             rx_data[RX_BUFFER_SIZE];
#endif /* RING_BUFFER_ENABLE */
/* definition for test_uart */

#define DIAGNOSTICS_COMMANDS \
{ "test",  diagnostics_test,  1, NULL, NULL, "<peripheral>", "tests specified peripheral (test help for peripheral list)" }, \
{ "reset", diagnostics_reset, 0, NULL, NULL, "",             "reset board" },


int diagnostics_test( int argc, char* argv[])
{
    char* peripheral = argv[1];

    if ( strcmp( peripheral, "audio_loopback" ) == 0)
    {
        audio_loopback();
    }
    else if ( strcmp( peripheral, "cpu" ) == 0 )
    {
        cpu_perf_test();
    }
    else if ( strcmp( peripheral, "ddr" ) == 0 )
    {
        ddr_test();
    }
    else if ( strcmp(peripheral, "uart" ) == 0 )
    {
        uart_dut();
    }
    else if ( strcmp(peripheral, "i2c" ) == 0 )
    {
        i2c();
    }
    else if ( strcmp(peripheral, "hibernation") == 0 )
    {
        hibernation();
    }
    else if ( strcmp(peripheral, "hwcrypto_test") == 0 )
    {
        hwcrypto_test();
    }
    else if ( strcmp(peripheral, "gpio" ) == 0 )
    {
        gpio();
    }
    else if ( strcmp( peripheral, "otp_read_bit" ) == 0 )
    {
        otp_read_bit(0);
    }
    else if ( strcmp( peripheral, "otp_read_word" ) == 0 )
    {
        otp_read_word(0);
    }
    else if ( strcmp( peripheral, "otp_dump" ) == 0 )
    {
        otp_dump(0);
    }
    else if ( strcmp( peripheral, "spi") == 0 )
    {
        spi();
    }
    else if (strcmp( peripheral, "gci") == 0)
    {
        gci_test();
    }
#if (WICED_USB_SUPPORT)
    else if (strcmp( peripheral, "usb_host") == 0)
    {
        usb_host_test();
    }
#endif
    else if (strcmp( peripheral, "pwm") == 0)
    {
        pwm_test();
    }
#if (WICED_SDIO_SUPPORT)
    else if (strcmp( peripheral, "sdio") == 0)
    {
        sdio_test();
    }
#endif
    else if (strcmp( peripheral, "gmac_loopback") == 0)
    {
        gmac_loopback_test();
    }
    else if ( strcmp( peripheral, "help" ) == 0 )
    {
        printf( "Available commands:\n" );
        printf( "   audio_loopback\n" );
        printf( "       - Tests the audio; place speakers into DAC OUT audio jack\n" );
        printf( "   cpu\n" );
        printf( "       - Runs through shuffled cpu tests and analyzes error margin\n" );
        printf( "   ddr\n" );
        printf( "       -  Writes and reads from ddr with cache, without cache, with quick boot, with alt boot, and with random data\n" );
        printf( "   i2c\n" );
        printf( "       - Inits and tests i2c with standard, low, and high speeds and reading/writing 04H and 32H sequences\n" );
        printf( "   gpio\n" );
        printf( "       - Inputs into input and reads from test pin with rising edge, falling edge, and both edges to check if pin input/output is same\n" );
        printf( "   hibernation\n" );
        printf( "       - Sends platform into hibernation for specified freq and ticks; upon wake checks if returned from hibernation condition set or not, will return to diagnostic app\n" );
        printf( "   hwcrypto_test\n" );
        printf( "       - Test cyrpto apis, with 128 byte aes, 256 byte sha hash\n" );
        printf( "   otp_read_bit\n" );
        printf( "       - Reads bit 0 from otp\n" );
        printf( "   otp_read_word\n" );
        printf( "       - Reads byte 0 from otp\n" );
        printf( "   otp_dump\n" );
        printf( "       - Dumps otp contents for users\n" );
        printf( "   spi\n" );
        printf( "       - Reads the flash id\n" );
        printf( "   uart\n" );
        printf( "       - Tests the gpio; enter inputs when prompted\n" );
        printf( "   gci\n" );
        printf( "       - Dump GCI core indirect status registers\n" );
#if (WICED_USB_SUPPORT)
        printf( "   usb_host\n" );
        printf( "       - Tests USB Host mass storage; plug one USB flash disk device into board\n" );
#endif
        printf( "   pwm\n" );
        printf( "       - Tests PWM at various frequencies and duty cycles\n" );
#if (WICED_SDIO_SUPPORT)
        printf( "   sdio\n" );
        printf( "       - Inits SDIO and dump Card Common Control Registers(CCCR)\n" );
#endif
        printf( "   gmac_loopback\n" );
        printf( "       - Run Ethernet (GMAC) loopback test\n" );
    }
    else
    {
        printf( "invalid input (enter test help for all peripherals)\n" );
    }

    return 0;
}

int diagnostics_reset( int argc, char* argv[])
{
    platform_mcu_reset( );
    return 0;
}

static const command_t init_commands[] =
{
    DIAGNOSTICS_COMMANDS
    CMD_TABLE_END
};

/**
 *  @param thread_input : Unused parameter - required to match thread prototype
 */
void application_start( void )
{
    /* Initialise core services .*/
    wiced_core_init( );

    printf( "Diagnostics app\n" );

    hibernation_boot( );

    command_console_init( STDIO_UART, MAX_LINE_LENGTH, line_buffer, MAX_HISTORY_LENGTH, history_buffer_storage, " " );
    console_add_cmd_table( init_commands );
}
