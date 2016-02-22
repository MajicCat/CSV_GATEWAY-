/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

#include "platform_peripheral.h"
#include "platform_appscr4.h"
#include "platform_assert.h"
#include "platform_peripheral.h"
#include "platform_config.h"

#include "cr4.h"

#include "typedefs.h"
#include "sbchipc.h"

#include "wiced_defaults.h"
#include "wiced_osl.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/


#define PLATFORM_WATCHDOG_TIMEOUT_MULTIPLIER    platform_reference_clock_get_freq( PLATFORM_REFERENCE_CLOCK_ILP )

#define PLATFORM_WATCHDOG_TIMEOUT(sec)          ((sec) * PLATFORM_WATCHDOG_TIMEOUT_MULTIPLIER)

#ifndef APPLICATION_WATCHDOG_TIMEOUT_SECONDS
#define APPLICATION_WATCHDOG_TIMEOUT_SECONDS    MAX_WATCHDOG_TIMEOUT_SECONDS
#endif

#ifndef PLATFORM_MCU_RESET_MAX_REBOOT_SECONDS
#define PLATFORM_MCU_RESET_MAX_REBOOT_SECONDS   60
#endif

#ifndef PLATFORM_MCU_RESET_MAX_WAIT_SECONDS
#define PLATFORM_MCU_RESET_MAX_WAIT_SECONDS     3
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
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

static void
platform_watchdog_set( uint32_t timeout )
{
    while ( PLATFORM_PMU->pmustatus & PST_SLOW_WR_PENDING );
    PLATFORM_PMU->pmuwatchdog = timeout;
}

platform_result_t platform_watchdog_init( void )
{
#ifndef WICED_DISABLE_WATCHDOG
    return platform_watchdog_kick();
#else
    platform_watchdog_deinit();
    return PLATFORM_FEATURE_DISABLED;
#endif
}

platform_result_t platform_watchdog_deinit( void )
{
    platform_watchdog_set( 0 );
    return PLATFORM_SUCCESS;
}

platform_result_t platform_watchdog_kick_seconds( uint32_t seconds )
{
#ifndef WICED_DISABLE_WATCHDOG
    platform_watchdog_set( PLATFORM_WATCHDOG_TIMEOUT( seconds ) );
    return PLATFORM_SUCCESS;
#else
    return PLATFORM_FEATURE_DISABLED;
#endif
}

platform_result_t platform_watchdog_kick( void )
{
    return platform_watchdog_kick_seconds( APPLICATION_WATCHDOG_TIMEOUT_SECONDS );
}

wiced_bool_t platform_watchdog_check_last_reset( void )
{
#ifndef WICED_DISABLE_WATCHDOG
    if ( PLATFORM_PMU->pmustatus & PST_WDRESET )
    {
        /* Clear the flag and return */
        PLATFORM_PMU->pmustatus = PST_WDRESET;
        return WICED_TRUE;
    }
#endif

    return WICED_FALSE;
}

void platform_mcu_specific_reset( platform_reset_type_t type )
{
    WICED_DISABLE_INTERRUPTS();

    /*
     * This reset type does not break JTAG.
     * So set it up here, just in case it is earlier changed.
     */
    platform_common_chipcontrol( &PLATFORM_PMU->pmucontrol, PMU_CONTROL_RESETCONTROL_MASK, PMU_CONTROL_RESETCONTROL_RESTORE_RES_MASKS );

#ifndef WICED_DISABLE_WATCHDOG
    /*
     * Fire PMU watchdog far in a future, use hard-coded ILP clock to reduce application sizes which do not use watchdog.
     * If board would hang during rebooting, PMU watchdog will try to reset board.
     * After application loaded it will kick or deinit watchdog.
     */
    platform_watchdog_set( PLATFORM_MCU_RESET_MAX_REBOOT_SECONDS * ILP_CLOCK );
#endif

    /*
     * Make sure board not enter endless reset cycle.
     * During wait loop we should have JTAG operational.
     * Don't run loop in tiny-bootloader as after resetting boot goes slow cold path, and normal bootloader will run this loop.
     */
#if !defined(TINY_BOOTLOADER) && PLATFORM_MCU_RESET_MAX_WAIT_SECONDS
    if ( !platform_is_init_completed() )
    {
        cr4_init_cycle_counter();
    }
    while( ( cr4_get_cycle_counter() < PLATFORM_MCU_RESET_MAX_WAIT_SECONDS * CPU_CLOCK_HZ ) && !cr4_is_cycle_counter_overflowed() );
#endif

    /* Choose reset type */
    if ( type == PLATFORM_RESET_TYPE_POWERCYCLE )
    {
        platform_common_chipcontrol( &PLATFORM_PMU->pmucontrol, PMU_CONTROL_RESETCONTROL_MASK, PMU_CONTROL_RESETCONTROL_RESET );
    }

    /* Set watchdog to reset system on next tick */
    PLATFORM_CHIPCOMMON->clock_control.watchdog_counter = 1;

    /* Loop forever */
    while (1);
}

void platform_mcu_reset( void )
{
    platform_mcu_specific_reset( PLATFORM_RESET_TYPE_POWERCYCLE );
}
