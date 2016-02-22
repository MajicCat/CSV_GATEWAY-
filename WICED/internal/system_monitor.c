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
 */
#include "wiced_platform.h"
#include "wiced_rtos.h"
#include "wwd_assert.h"
#include "RTOS/wwd_rtos_interface.h"
#include "wiced_framework.h"
#include "wiced_internal_api.h"
#include <string.h>

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
#ifndef MAX_WAKE_FROM_SLEEP_DELAY_TICKS
#define MAX_WAKE_FROM_SLEEP_DELAY_TICKS (100)
#endif

#ifdef APPLICATION_WATCHDOG_TIMEOUT_SECONDS
#define APPLICATION_WATCHDOG_TIMEOUT_MILLISECONDS  (APPLICATION_WATCHDOG_TIMEOUT_SECONDS * 1000)
/* Set the system monitor thread delay time to 20% less than the watchdog timer to fit both actual and debug/shadow watchdog timeout */
#define DEFAULT_SYSTEM_MONITOR_PERIOD ( APPLICATION_WATCHDOG_TIMEOUT_MILLISECONDS * 8/10 - MAX_WAKE_FROM_SLEEP_DELAY_TICKS)
#else
#ifndef DEFAULT_SYSTEM_MONITOR_PERIOD
#define DEFAULT_SYSTEM_MONITOR_PERIOD   (5000)
#endif
#endif

#ifndef MAXIMUM_NUMBER_OF_SYSTEM_MONITORS
#define MAXIMUM_NUMBER_OF_SYSTEM_MONITORS    (5)
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

static wiced_system_monitor_t* system_monitors[MAXIMUM_NUMBER_OF_SYSTEM_MONITORS];
static wiced_semaphore_t       system_monitor_thread_semaphore;

/******************************************************
 *               Function Definitions
 ******************************************************/

void system_monitor_thread_main( wiced_thread_arg_t arg )
{
    UNUSED_PARAMETER(arg);

    memset(system_monitors, 0, sizeof(system_monitors));

    if (wiced_rtos_init_semaphore(&system_monitor_thread_semaphore) != WICED_SUCCESS)
    {
        wiced_assert("semaphore init failed", 0);
    }

    /* - Can watch threads
     * Each thread can set a counter that is decremented every event.
     * Once any timer == 0, the watchdog is no longer kicked
     *
     * - Can watch packet buffer status
     * If no RX packets are available, take timestamp (T). If (current time - T) > X seconds, stop kicking watchdog.
     * X can be 10 second default. Time will be set to zero once RX buffer is freed
     *
     * - Can watch bus data credits
     * If no credits are available, take timestamp (B). If (current time - B) > X seconds, stop kicking watchdog.
     * This will be dependent on if WLAN is up. Timer will be set to 0 if credits become available.
     */
    while (1)
    {
        int a;
        uint32_t current_time = host_rtos_get_time();

        for (a = 0; a < MAXIMUM_NUMBER_OF_SYSTEM_MONITORS; ++a)
        {
            if (system_monitors[a] != NULL)
            {
                if ((current_time - system_monitors[a]->last_update) > system_monitors[a]->longest_permitted_delay)
                {
                    /* A system monitor update period has been missed and hence explicitly resetting the MCU rather than waiting for the watchdog to bite */
                    wiced_framework_reboot();
                }
            }
        }
        wiced_watchdog_kick();
        wiced_rtos_get_semaphore(&system_monitor_thread_semaphore, DEFAULT_SYSTEM_MONITOR_PERIOD);
    }

    wiced_rtos_deinit_semaphore(&system_monitor_thread_semaphore);

    WICED_END_OF_CURRENT_THREAD( );
}

wiced_result_t wiced_register_system_monitor(wiced_system_monitor_t* system_monitor, uint32_t initial_permitted_delay)
{
    int a;

    /* Find spare entry and add the new system monitor */
    for ( a = 0; a < MAXIMUM_NUMBER_OF_SYSTEM_MONITORS; ++a )
    {
        if (system_monitors[a] == NULL)
        {
            system_monitor->last_update = host_rtos_get_time();
            system_monitor->longest_permitted_delay = initial_permitted_delay;
            system_monitors[a] = system_monitor;
            return WICED_SUCCESS;
        }
    }

    return WICED_ERROR;
}

wiced_result_t wiced_update_system_monitor(wiced_system_monitor_t* system_monitor, uint32_t permitted_delay)
{
    uint32_t current_time = host_rtos_get_time();
    /* Update the system monitor if it hasn't already passed it's permitted delay */
    if ((current_time - system_monitor->last_update) <= system_monitor->longest_permitted_delay)
    {
        system_monitor->last_update             = current_time;
        system_monitor->longest_permitted_delay = permitted_delay;
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_wakeup_system_monitor_thread(void)
{
    return wiced_rtos_set_semaphore(&system_monitor_thread_semaphore);
}
