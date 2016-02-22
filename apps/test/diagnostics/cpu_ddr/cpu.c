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
 * CPU test Application
 *
 * This program runs the cpu test.
 *
 * Application Instructions
 *   Connect a PC terminal to the serial port of the WICED Eval board,
 *   then build and download the application as described in the WICED
 *   Quick Start Guide
 *
 *   After download, the app shuffles the test orders,
 *   runs the tests on different frequencies, and analyzes test results
 *
 */

#include "wiced.h"

#include "platform_appscr4.h"

#include "typedefs.h"
#include "sbchipc.h"

#include "cpu_ddr.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define CPU_PERF_MS_TO_MEASURE 1000
#define CPU_PERF_ITER_INSTR    1000000
#define CPU_PERF_ITER_READ     100000
#define CPU_PERF_ERROR_THRESH  0.05

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

typedef struct cpu_single_test
{
    platform_cpu_clock_frequency_t       cpu_freq;
    char*                                descr;
    float                                weight;
    float                                result;
} cpu_single_test_t;

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

static cpu_single_test_t tests[] =
{
    {
        .cpu_freq           = PLATFORM_CPU_CLOCK_FREQUENCY_24_MHZ,
        .descr              = "24MHZ/24MHZ",
        .weight             = 1.000,
    },
    {
        .cpu_freq           = PLATFORM_CPU_CLOCK_FREQUENCY_48_MHZ,
        .descr              = "48MHZ/48MHZ",
        .weight             = 0.500,
    },
    {
        .cpu_freq           = PLATFORM_CPU_CLOCK_FREQUENCY_60_MHZ,
        .descr              = "60MHZ/60MHZ",
        .weight             = 0.400,
    },
    {
        .cpu_freq           = PLATFORM_CPU_CLOCK_FREQUENCY_80_MHZ,
        .descr              = "80MHZ/80MHZ",
        .weight             = 0.300,
    },
    {
        .cpu_freq           = PLATFORM_CPU_CLOCK_FREQUENCY_120_MHZ,
        .descr              = "120MHZ/120MHZ",
        .weight             = 0.200,
    },
    {
        .cpu_freq           = PLATFORM_CPU_CLOCK_FREQUENCY_160_MHZ,
        .descr              = "160MHZ/160MHZ",
        .weight             = 0.150,
    },
    {
        .cpu_freq           = PLATFORM_CPU_CLOCK_FREQUENCY_320_MHZ,
        .descr              = "320MHZ/160MHZ",
        .weight             = 0.090,
    },
};

/******************************************************
 *               Function Definitions
 ******************************************************/

static uint64_t
xorshift64star( void )
{
    static uint64_t x = 10062015;

    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;

    return x * 2685821657736338717ULL;
}

static void
cpu_shuffle_tests( void )
{
    const unsigned size = ARRAYSIZE( tests );
    unsigned i;

    for ( i = 0; i < size; ++i )
    {
        unsigned index1 = xorshift64star() % size;
        unsigned index2 = xorshift64star() % size;
        cpu_single_test_t tmp = tests[ index1 ];
        tests[ index1 ] = tests[ index2 ];
        tests[ index2 ] = tmp;
    }
}

static void
cpu_set_frequency( platform_cpu_clock_frequency_t cpu_freq )
{
    platform_tick_stop();

    platform_cpu_clock_init( cpu_freq );

    platform_tick_start();
}

static uint32_t
cpu_ilp_stamp( void )
{
    uint32_t stamp = PLATFORM_PMU->pmutimer;
    if ( stamp != PLATFORM_PMU->pmutimer )
    {
        stamp = PLATFORM_PMU->pmutimer;
    }
    return stamp;
}

static void
cpu_instr_loop ( uint32_t num )
{
    __asm__ __volatile__
    (
        "   MOV  r0, %0\n\t"
        "1: SUBS r0, r0, 1\n\t"
        "   BNE  1b" : : "r"(num) : "r0"
    );
}

static void
cpu_read_loop ( uint32_t num )
{
    const uint32_t addr = (uint32_t)&PLATFORM_APPSCR4->cycle_cnt;

    __asm__ __volatile__
    (
        "   MOV  r0, %0\n\t"
        "1: LDR  r1, [%1]\n\t"
        "   SUBS r0, r0, 1\n\t"
        "   BNE  1b" : : "r"(num), "r"(addr) : "r0", "r1"
    );
}

static float
cpu_ilp_ticks_per_loop ( void )
{
    uint32_t num_loops      = 0;
    uint32_t stamp_begin    = cpu_ilp_stamp();
    uint32_t stamp_deadline = stamp_begin + ILP_CLOCK / 1000 * CPU_PERF_MS_TO_MEASURE;

    while ( WICED_TRUE )
    {
        uint32_t stamp_end;

        cpu_instr_loop( CPU_PERF_ITER_INSTR );
        cpu_read_loop( CPU_PERF_ITER_READ );

        ++num_loops;
        stamp_end = cpu_ilp_stamp();

        if ( stamp_end >= stamp_deadline )
        {
            return ( (float)( stamp_end - stamp_begin ) ) / num_loops;
        }
    }
}

static void
cpu_run_tests( void )
{
    unsigned i;

    for( i = 0; i < ARRAYSIZE( tests ); ++i )
    {
        cpu_set_frequency( tests[i].cpu_freq );

        tests[i].result = cpu_ilp_ticks_per_loop();

        LOG_PRINTF(( "2.CPU performance test for %s : %f ticks\n", tests[i].descr, tests[i].result ));
    }

    cpu_set_frequency( PLATFORM_CPU_CLOCK_FREQUENCY );
}

static void
cpu_normalize_test_results( void )
{
    float    max_weight = 0.0;
    float    max_ticks  = 0.0;
    unsigned i;

    for( i = 0; i < ARRAYSIZE( tests ); ++i )
    {
        if ( tests[i].weight > max_weight )
        {
            max_weight = tests[i].weight;
        }
        if ( tests[i].result > max_ticks )
        {
            max_ticks = tests[i].result;
        }
    }

    for( i = 0; i < ARRAYSIZE( tests ); ++i )
    {
        tests[i].result = tests[i].result * max_weight / max_ticks;
    }
}

static wiced_bool_t
cpu_analize_test_results( void )
{
    wiced_bool_t result = WICED_TRUE;
    unsigned     i;

    for( i = 0; i < ARRAYSIZE( tests ); ++i )
    {
        wiced_bool_t pass = WICED_TRUE;
        float diff = 1.0 - tests[i].weight / tests[i].result;

        if ( ( diff > CPU_PERF_ERROR_THRESH ) || ( diff < -CPU_PERF_ERROR_THRESH ) )
        {
            result = WICED_FALSE;
            pass   = WICED_FALSE;
        }

        LOG_PRINTF(( "3.CPU performance test for %s is %s : should be %f while %f got\n",
            tests[i].descr, pass ? "SUCCEED" : "FAILED", tests[i].weight, tests[i].result ));
    }

    return result;
}
/*!
 ******************************************************************************
 * Shuffles the test orders, runs the tests on different frequencies, and analyzes test results
 *
 * @return
 */
wiced_bool_t
cpu_perf_test( void )
{
    wiced_bool_t res;

    LOG_PRINTF(( "1.CPU performance test is about to start: pll1=0x%lx pll2=0x%lx\n", platform_pmu_pllcontrol( 1, 0, 0), platform_pmu_pllcontrol( 2, 0, 0)));

    cpu_shuffle_tests();

    cpu_run_tests();

    cpu_normalize_test_results();

    res = cpu_analize_test_results();

    LOG_PRINTF(( "4.CPU performance test : %s\n\n", res ? "SUCCESS" : "FAILURE" ));

    return res;
}
