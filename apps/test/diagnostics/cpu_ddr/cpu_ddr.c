/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

#include "wiced.h"

#include <stdio.h>

#include "platform_cache.h"

#include "cpu_ddr.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define RUN_TEST(name) \
    if (name##_test()) \
    { \
        test_counters.success++; \
        test_counters.name##_success++; \
    } \
    else \
    { \
        test_counters.failure++; \
        test_counters.name##_failure++; \
    }


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
    uint32_t success;
    uint32_t failure;
    uint32_t ddr_success;
    uint32_t ddr_failure;
    uint32_t cpu_perf_success;
    uint32_t cpu_perf_failure;
} test_counters_t;

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

volatile test_counters_t test_counters;

/******************************************************
 *               Function Definitions
 ******************************************************/

void cpu_ddr( void )
{
    unsigned i = 0;

    LOG_PRINTF(("Test is about to start.\n"));

    WICED_DISABLE_INTERRUPTS();

    PLATFORM_PMU->pmuwatchdog = 0x0; /* disable watchdog */

    srand(26);

    while (1)
    {
        LOG_PRINTF(("Iteration #%u is about to start\n", i));

        RUN_TEST(cpu_perf);

        if ( PLATFORM_FEATURE_ENAB(DDR) )
        {
            RUN_TEST(ddr);
        }

        platform_dcache_clean_range((void*)&test_counters, sizeof(test_counters));

        LOG_PRINTF(("Iteration #%u completed\n", i));

        ++i;
    }

    WICED_ENABLE_INTERRUPTS();
}
