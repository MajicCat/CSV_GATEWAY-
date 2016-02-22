/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

#include "command_console_commands.h"

#include "wiced_management.h"
#include "wiced_deep_sleep.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define POWERSAVE_DEEP_SLEEP_TIME_MS                              "120000"
#define POWERSAVE_DEEP_SLEEP_SHORT_TIME_MS                        "5000"
#define POWERSAVE_ITERATION_DELAY_MS                              "3000"

#define POWERSAVE_ITERATION_SLEEP_COMMAND                         "sleep "POWERSAVE_ITERATION_DELAY_MS
#define POWERSAVE_JOIN_COMMAND                                    "join test123 open PASSWORD 192.168.1.100 192.168.1.255 192.168.1.1"
#define POWERSAVE_WLAN_POWERSAVE_ENABLE_MODE_CONFIG_COMMAND       "wifi_powersave 1"
#define POWERSAVE_WLAN_POWERSAVE_DISABLE_MODE_CONFIG_COMMAND      "wifi_powersave 0"
#define POWERSAVE_APPS_FREQ_60MHZ_COMMAND                         "mcu_powersave_freq 2"
#define POWERSAVE_APPS_ALP_AVAILABLE_CLOCK_REQUEST_COMMAND        "mcu_powersave_clock 1 0"
#define POWERSAVE_APPS_HT_AVAILABLE_CLOCK_REQUEST_COMMAND         "mcu_powersave_clock 1 1"
#define POWERSAVE_APPS_BACKPLANE_ON_ILP_CLOCK_REQUEST_COMMAND     "mcu_powersave_clock 1 2"
#define POWERSAVE_APPS_BACKPLANE_ON_ALP_CLOCK_REQUEST_COMMAND     "mcu_powersave_clock 1 3"
#define POWERSAVE_APPS_TICKLESS_MODE_CONFIG_COMMAND               "mcu_powersave_tick 0"
#define POWERSAVE_APPS_DEEPSLEEP_MODE_CONFIG_COMMAND              "mcu_powersave_mode 0"
#define POWERSAVE_APPS_SLEEP_MODE_CONFIG_COMMAND                  "mcu_powersave_mode 1"
#define POWERSAVE_APPS_POWERSAVE_ENABLE_COMMAND                   "mcu_powersave 1"
#define POWERSAVE_APPS_SLEEP_WITH_POWERSAVE_ENABLED_COMMAND       "mcu_powersave_sleep 1 "POWERSAVE_DEEP_SLEEP_TIME_MS
#define POWERSAVE_APPS_SHORT_SLEEP_WITH_POWERSAVE_ENABLED_COMMAND "mcu_powersave_sleep 1 "POWERSAVE_DEEP_SLEEP_SHORT_TIME_MS
#define POWERSAVE_IPERF_UDP_SERVER                                "iperf -s -u -i 1"
#define POWERSAVE_START_SEQUENCE_AGAIN_COMMAND                    NULL

#define POWERSAVE_STANDALONE_TEST_DEEPSLEEP_NO_ASSOC              0 /* change to 1 to enable */
#define POWERSAVE_STANDALONE_TEST_DEEPSLEEP_ASSOC                 0 /* change to 1 to enable */
#define POWERSAVE_STANDALONE_TEST_WAIT_FOR_WLAN                   0 /* change to 1 to enable */
#define POWERSAVE_STANDALONE_TEST_ACTIVE_WAIT_FOR_WLAN            0 /* change to 1 to enable */
#define POWERSAVE_STANDALONE_TEST_LOW_POWER_NETWORKING            0 /* change to 1 to enable */
#define POWERSAVE_STANDALONE_TEST_WAKEUP_FROM_DEEP_SLEEP_PROFILE  0 /* change to 1 to enable, for more precise profiling may worth to go to UART driver and make TX function (e.g. uart_slow_transmit_bytes()) do nothing */

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

/******************************************************
 *               Function Definitions
 ******************************************************/

static char line_buffer[MAX_LINE_LENGTH];
static char history_buffer_storage[MAX_LINE_LENGTH * MAX_HISTORY_LENGTH];

static wiced_bool_t wprint_permission = WICED_FALSE;

static const command_t commands[] =
{
    ALL_COMMANDS
    CMD_TABLE_END
};

static void startup_commands( void )
{
    static char* cold_boot_commands[] =
    {
#if POWERSAVE_STANDALONE_TEST_WAKEUP_FROM_DEEP_SLEEP_PROFILE
        POWERSAVE_APPS_TICKLESS_MODE_CONFIG_COMMAND,
        POWERSAVE_APPS_DEEPSLEEP_MODE_CONFIG_COMMAND,
        POWERSAVE_APPS_SHORT_SLEEP_WITH_POWERSAVE_ENABLED_COMMAND,
        POWERSAVE_START_SEQUENCE_AGAIN_COMMAND
#elif POWERSAVE_STANDALONE_TEST_DEEPSLEEP_NO_ASSOC
        POWERSAVE_ITERATION_SLEEP_COMMAND,
        POWERSAVE_APPS_TICKLESS_MODE_CONFIG_COMMAND,
        POWERSAVE_APPS_DEEPSLEEP_MODE_CONFIG_COMMAND,
        POWERSAVE_APPS_SLEEP_WITH_POWERSAVE_ENABLED_COMMAND,
        POWERSAVE_ITERATION_SLEEP_COMMAND,
        POWERSAVE_START_SEQUENCE_AGAIN_COMMAND
#elif POWERSAVE_STANDALONE_TEST_DEEPSLEEP_ASSOC
        POWERSAVE_JOIN_COMMAND,
        POWERSAVE_ITERATION_SLEEP_COMMAND,
        POWERSAVE_WLAN_POWERSAVE_ENABLE_MODE_CONFIG_COMMAND,
        POWERSAVE_ITERATION_SLEEP_COMMAND,
        POWERSAVE_APPS_TICKLESS_MODE_CONFIG_COMMAND,
        POWERSAVE_APPS_DEEPSLEEP_MODE_CONFIG_COMMAND,
        POWERSAVE_APPS_SLEEP_WITH_POWERSAVE_ENABLED_COMMAND,
        POWERSAVE_ITERATION_SLEEP_COMMAND,
        POWERSAVE_START_SEQUENCE_AGAIN_COMMAND
#elif POWERSAVE_STANDALONE_TEST_WAIT_FOR_WLAN
        POWERSAVE_JOIN_COMMAND,
        POWERSAVE_ITERATION_SLEEP_COMMAND,
        POWERSAVE_WLAN_POWERSAVE_ENABLE_MODE_CONFIG_COMMAND,
        POWERSAVE_APPS_TICKLESS_MODE_CONFIG_COMMAND,
        POWERSAVE_APPS_SLEEP_MODE_CONFIG_COMMAND,
        POWERSAVE_APPS_SLEEP_WITH_POWERSAVE_ENABLED_COMMAND,
        POWERSAVE_ITERATION_SLEEP_COMMAND,
        POWERSAVE_START_SEQUENCE_AGAIN_COMMAND
#elif POWERSAVE_STANDALONE_TEST_ACTIVE_WAIT_FOR_WLAN
        POWERSAVE_JOIN_COMMAND,
        POWERSAVE_ITERATION_SLEEP_COMMAND,
        POWERSAVE_WLAN_POWERSAVE_ENABLE_MODE_CONFIG_COMMAND,
        POWERSAVE_APPS_ALP_AVAILABLE_CLOCK_REQUEST_COMMAND,
        POWERSAVE_APPS_BACKPLANE_ON_ILP_CLOCK_REQUEST_COMMAND,
        POWERSAVE_APPS_TICKLESS_MODE_CONFIG_COMMAND,
        POWERSAVE_APPS_SLEEP_MODE_CONFIG_COMMAND,
        POWERSAVE_APPS_SLEEP_WITH_POWERSAVE_ENABLED_COMMAND,
        POWERSAVE_ITERATION_SLEEP_COMMAND,
        POWERSAVE_START_SEQUENCE_AGAIN_COMMAND
#elif POWERSAVE_STANDALONE_TEST_LOW_POWER_NETWORKING
        POWERSAVE_JOIN_COMMAND,
        POWERSAVE_WLAN_POWERSAVE_DISABLE_MODE_CONFIG_COMMAND,
        POWERSAVE_ITERATION_SLEEP_COMMAND,
        POWERSAVE_APPS_FREQ_60MHZ_COMMAND,
        POWERSAVE_ITERATION_SLEEP_COMMAND,
        POWERSAVE_APPS_TICKLESS_MODE_CONFIG_COMMAND,
        POWERSAVE_APPS_HT_AVAILABLE_CLOCK_REQUEST_COMMAND,
        POWERSAVE_APPS_BACKPLANE_ON_ALP_CLOCK_REQUEST_COMMAND,
        POWERSAVE_APPS_POWERSAVE_ENABLE_COMMAND,
        POWERSAVE_IPERF_UDP_SERVER
#endif
    };
    static char* warm_boot_commands[] =
    {
#if POWERSAVE_STANDALONE_TEST_DEEPSLEEP_ASSOC || POWERSAVE_STANDALONE_TEST_DEEPSLEEP_NO_ASSOC
        POWERSAVE_ITERATION_SLEEP_COMMAND,
        POWERSAVE_APPS_TICKLESS_MODE_CONFIG_COMMAND,
        POWERSAVE_APPS_DEEPSLEEP_MODE_CONFIG_COMMAND,
        POWERSAVE_APPS_SLEEP_WITH_POWERSAVE_ENABLED_COMMAND,
        POWERSAVE_ITERATION_SLEEP_COMMAND,
        POWERSAVE_START_SEQUENCE_AGAIN_COMMAND
#elif POWERSAVE_STANDALONE_TEST_WAKEUP_FROM_DEEP_SLEEP_PROFILE
        POWERSAVE_APPS_TICKLESS_MODE_CONFIG_COMMAND,
        POWERSAVE_APPS_DEEPSLEEP_MODE_CONFIG_COMMAND,
        POWERSAVE_APPS_SHORT_SLEEP_WITH_POWERSAVE_ENABLED_COMMAND,
        POWERSAVE_START_SEQUENCE_AGAIN_COMMAND
#endif
    };

    char** commands;
    int commands_num;
    int i;

    if ( WICED_DEEP_SLEEP_IS_WARMBOOT_HANDLE() )
    {
        commands = warm_boot_commands;
        commands_num = ARRAYSIZE( warm_boot_commands );
    }
    else
    {
        commands = cold_boot_commands;
        commands_num = ARRAYSIZE( cold_boot_commands );
    }

    i = 0;
    while ( i < commands_num )
    {
        if ( commands[ i ] == POWERSAVE_START_SEQUENCE_AGAIN_COMMAND )
        {
            i = 0;
        }
        else
        {
            console_parse_cmd( commands[ i ] );
            i++;
        }
    }
}

void application_start( void )
{
    /* Initialise the device */
    if ( WICED_DEEP_SLEEP_IS_WARMBOOT_HANDLE( ) )
    {
        wiced_resume_after_deep_sleep( );
    }
    else
    {
        wiced_init( );
    }
    wprint_permission = WICED_TRUE;

    /* Run console */
    command_console_init( STDIO_UART, MAX_LINE_LENGTH, line_buffer, MAX_HISTORY_LENGTH, history_buffer_storage, " " );
    console_add_cmd_table( commands );

    /* Console is up and running in dedicated thread. Let's run now startup commands. */
    startup_commands();
}

int platform_wprint_permission( void )
{
#if POWERSAVE_STANDALONE_TEST_WAKEUP_FROM_DEEP_SLEEP_PROFILE
    UNUSED_VARIABLE( wprint_permission );
    return 0;
#else
    return ( !WICED_DEEP_SLEEP_IS_WARMBOOT_HANDLE( ) || wprint_permission ) ? 1 : 0;
#endif
}

#if POWERSAVE_STANDALONE_TEST_WAKEUP_FROM_DEEP_SLEEP_PROFILE
void mcu_powersave_deep_sleep_event_handler( wiced_bool_t before )
{
}
#endif
