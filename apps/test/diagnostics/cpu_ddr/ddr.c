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
 * DDR test Application
 *
 * This program runs the ddr tests.
 *
 * Application Instructions
 *   Connect a PC terminal to the serial port of the WICED Eval board,
 *   then build and download the application as described in the WICED
 *   Quick Start Guide
 *
 *   After download, the app runs through ddr with cache, without cache,
 *   with quick boot, with alt boot, and with random data
 *   Writes to ddr, reads back, and checks if same.
 *
 */

#include <stdio.h>

#include "platform_m2m.h"
#include "platform_map.h"
#include "platform_cache.h"

#include "wiced.h"

#include "cpu_ddr.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define DDR_BASE                 PLATFORM_DDR_BASE(0x0)
#define DDR_SIZE                 platform_ddr_get_size()

#define RANDOM_CHUNK_MAX_SIZE    (128 * 1024)
#define RANDOM_CHUNK_MIN_SIZE    (2 * 1024)

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef wiced_bool_t(*ddr_test_func_t)(void);

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

static wiced_bool_t ddr_test_third_party(void);

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

static wiced_bool_t
ddr_test_seq(wiced_bool_t cache_clean)
{
    wiced_bool_t res = WICED_TRUE;
    uint32_t p;

    for (p = DDR_BASE; p != DDR_BASE + DDR_SIZE; p += sizeof(uint32_t))
    {
        *((uint32_t*)p) = p;
    }

    if (cache_clean)
    {
        platform_dcache_clean_and_inv_range((void*)DDR_BASE, DDR_SIZE);
    }

    for (p = DDR_BASE; p != DDR_BASE + DDR_SIZE; p += sizeof(uint32_t))
    {
        uint32_t val = *(uint32_t*)p;

        if (val != p)
        {
            DBG_PRINTF(("addr 0x%x expected 0x%x got 0x%x\n", (unsigned)p, (unsigned)p, (unsigned)val));
            res = WICED_FALSE;
            break;
        }
    }

    return res;
}

static wiced_bool_t
ddr_test_seq_cache_clean(void)
{
    return ddr_test_seq(WICED_TRUE);
}

static wiced_bool_t
ddr_test_seq_cache_keep(void)
{
    return ddr_test_seq(WICED_FALSE);
}

static wiced_bool_t
ddr_test_random(void)
{
    static uint32_t first[RANDOM_CHUNK_MAX_SIZE];
    static uint32_t second[RANDOM_CHUNK_MAX_SIZE];

    const uint32_t start = DDR_BASE;
    const uint32_t end = DDR_BASE + DDR_SIZE - 4;

    uint32_t size = DDR_SIZE;
    uint32_t addr = DDR_BASE;
    uint32_t i = 0;

    if (RANDOM_CHUNK_MAX_SIZE < DDR_SIZE)
    {
        do
        {
            size = (rand() % (RANDOM_CHUNK_MAX_SIZE - RANDOM_CHUNK_MIN_SIZE)) + RANDOM_CHUNK_MIN_SIZE;
            addr = (rand() % (DDR_SIZE - size)) + DDR_BASE;
        } while ( ((addr + size) > end) || (addr < start) );
    }

    for (i = 0; i < (size / sizeof(uint32_t)); i++)
    {
        first[i] = rand();
    }

    /* Fill the memory with random data */
    m2m_unprotected_dma_memcpy((void *)addr, first, size, WICED_TRUE);

    /* Read back for comparison */
    m2m_unprotected_dma_memcpy(second, (void *)addr, size, WICED_TRUE);
    for (i = 0; i < (size / sizeof(int32_t)); i++)
    {
        if (first[i] != second[i])
        {
            DBG_PRINTF(("\nMem error @ 0x%x: found 0x%x, expected 0x%x\n", (unsigned)addr, (unsigned)second[i], (unsigned)first[i]));
            return WICED_FALSE;
        }
    }

    return WICED_TRUE;
}

/*!
 ******************************************************************************
 * Runs through ddr with cache, without cache, with quick boot, with alt boot, and with random data
 * Writes to ddr, reads back, and checks if same.
 *
 * @return
 */
wiced_bool_t
ddr_test( void )
{
    if (!PLATFORM_FEATURE_ENAB(DDR))
    {
        LOG_PRINTF(("DDR test NOT supported\n"));
        return WICED_TRUE;
    }

    static ddr_test_func_t ddr_test_funcs[] =
    {
        ddr_test_seq_cache_clean,
        ddr_test_seq_cache_keep,
        ddr_test_random,
        ddr_test_third_party
    };

    wiced_bool_t res = WICED_TRUE;
    unsigned i;

    for (i = 0; (i < ARRAYSIZE(ddr_test_funcs)) && res; ++i)
    {
        wiced_time_t rtos_time1, rtos_time2;

        LOG_PRINTF(("DDR test #%u is about to start (base 0x%x size %uMB)...\n", i, (unsigned)DDR_BASE, (unsigned)(DDR_SIZE / 1024 / 1024)));

        wiced_time_get_time(&rtos_time1);

        res = ddr_test_funcs[i]();

        wiced_time_get_time(&rtos_time2);

        LOG_PRINTF(("DDR test #%u %s (run for %u RTOS ticks)\n", i, res ? "SUCCEEDED" : "FAILED", (unsigned)(rtos_time2 - rtos_time1)));
    }

    platform_dcache_clean_all();

    return res;
}

/**********************************************************************
 *
 * Description: General-purpose memory testing functions.
 *
 * Copyright (c) 2000 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 *
 **********************************************************************/

/*
 * Set the data bus width.
 */
typedef uint32_t datum;

/*
 * Function prototypes.
 */
datum   memTestDataBus(volatile datum * address);
datum * memTestAddressBus(volatile datum * baseAddress, unsigned long nBytes);
datum * memTestDevice(volatile datum * baseAddress, unsigned long nBytes);

/**********************************************************************
 *
 * Function:    memTestDataBus()
 *
 * Description: Test the data bus wiring in a memory region by
 *              performing a walking 1's test at a fixed address
 *              within that region.  The address (and hence the
 *              memory region) is selected by the caller.
 *
 * Notes:
 *
 * Returns:     0 if the test succeeds.
 *              A non-zero result is the first pattern that failed.
 *
 **********************************************************************/
datum
memTestDataBus(volatile datum * address)
{
    datum pattern;


    /*
     * Perform a walking 1's test at the given address.
     */
    for (pattern = 1; pattern != 0; pattern <<= 1)
    {
        /*
         * Write the test pattern.
         */
        *address = pattern;


        /*
         * Read it back (immediately is okay for this test).
         */
        if (*address != pattern)
        {
            DBG_PRINTF(("addr 0x%x expected 0x%x got 0x%x\n", (unsigned)address, (unsigned)pattern, (unsigned)*address));
            return (pattern);
        }
    }

    return (0);

}   /* memTestDataBus() */


/**********************************************************************
 *
 * Function:    memTestAddressBus()
 *
 * Description: Test the address bus wiring in a memory region by
 *              performing a walking 1's test on the relevant bits
 *              of the address and checking for aliasing. This test
 *              will find single-bit address failures such as stuck
 *              -high, stuck-low, and shorted pins.  The base address
 *              and size of the region are selected by the caller.
 *
 * Notes:       For best results, the selected base address should
 *              have enough LSB 0's to guarantee single address bit
 *              changes.  For example, to test a 64-Kbyte region,
 *              select a base address on a 64-Kbyte boundary.  Also,
 *              select the region size as a power-of-two--if at all
 *              possible.
 *
 * Returns:     NULL if the test succeeds.
 *              A non-zero result is the first address at which an
 *              aliasing problem was uncovered.  By examining the
 *              contents of memory, it may be possible to gather
 *              additional information about the problem.
 *
 **********************************************************************/
datum *
memTestAddressBus(volatile datum * baseAddress, unsigned long nBytes)
{
    unsigned long addressMask = (nBytes/sizeof(datum) - 1);
    unsigned long offset;
    unsigned long testOffset;

    datum pattern     = (datum) 0xAAAAAAAA;
    datum antipattern = (datum) 0x55555555;


    /*
     * Write the default pattern at each of the power-of-two offsets.
     */
    for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
    {
        baseAddress[offset] = pattern;
    }

    /*
     * Check for address bits stuck high.
     */
    testOffset = 0;
    baseAddress[testOffset] = antipattern;

    for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
    {
        if (baseAddress[offset] != pattern)
        {
            DBG_PRINTF(("addr 0x%x expected 0x%x got 0x%x\n", (unsigned)(baseAddress + offset), (unsigned)pattern, (unsigned)baseAddress[offset]));
            return ((datum *) &baseAddress[offset]);
        }
    }

    baseAddress[testOffset] = pattern;

    /*
     * Check for address bits stuck low or shorted.
     */
    for (testOffset = 1; (testOffset & addressMask) != 0; testOffset <<= 1)
    {
        baseAddress[testOffset] = antipattern;

        if (baseAddress[0] != pattern)
        {
            DBG_PRINTF(("addr 0x%x expected 0x%x got 0x%x\n", (unsigned)baseAddress, (unsigned)pattern, (unsigned)baseAddress[0]));
            return ((datum *) &baseAddress[testOffset]);
        }

        for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
        {
            if ((baseAddress[offset] != pattern) && (offset != testOffset))
            {
                DBG_PRINTF(("addr 0x%x expected 0x%x got 0x%x\n", (unsigned)(baseAddress + offset), (unsigned)pattern, (unsigned)baseAddress[offset]));
                return ((datum *) &baseAddress[testOffset]);
            }
        }

        baseAddress[testOffset] = pattern;
    }

    return (NULL);

}   /* memTestAddressBus() */


/**********************************************************************
 *
 * Function:    memTestDevice()
 *
 * Description: Test the integrity of a physical memory device by
 *              performing an increment/decrement test over the
 *              entire region.  In the process every storage bit
 *              in the device is tested as a zero and a one.  The
 *              base address and the size of the region are
 *              selected by the caller.
 *
 * Notes:
 *
 * Returns:     NULL if the test succeeds.
 *
 *              A non-zero result is the first address at which an
 *              incorrect value was read back.  By examining the
 *              contents of memory, it may be possible to gather
 *              additional information about the problem.
 *
 **********************************************************************/
datum *
memTestDevice(volatile datum * baseAddress, unsigned long nBytes)
{
    unsigned long offset;
    unsigned long nWords = nBytes / sizeof(datum);

    datum pattern;
    datum antipattern;


    /*
     * Fill memory with a known pattern.
     */
    for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
        baseAddress[offset] = pattern;
    }

    /*
     * Check each location and invert it for the second pass.
     */
    for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
        if (baseAddress[offset] != pattern)
        {
            DBG_PRINTF(("addr 0x%x expected 0x%x got 0x%x\n", (unsigned)(baseAddress + offset), (unsigned)pattern, (unsigned)baseAddress[offset]));
            return ((datum *) &baseAddress[offset]);
        }

        antipattern = ~pattern;
        baseAddress[offset] = antipattern;
    }

    /*
     * Check each location for the inverted pattern and zero it.
     */
    for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
        antipattern = ~pattern;
        if (baseAddress[offset] != antipattern)
        {
            DBG_PRINTF(("addr 0x%x expected 0x%x got 0x%x\n", (unsigned)(baseAddress + offset), (unsigned)antipattern, (unsigned)baseAddress[offset]));
            return ((datum *) &baseAddress[offset]);
        }
    }

    return (NULL);

}   /* memTestDevice() */

static wiced_bool_t
ddr_test_third_party(void)
{
    wiced_result_t result = WICED_TRUE;

    platform_dcache_disable();

    if (memTestDataBus((volatile datum*)DDR_BASE) != 0)
    {
        result = WICED_FALSE;
    }

    if (memTestAddressBus((volatile datum*)DDR_BASE, DDR_SIZE) != NULL)
    {
        result = WICED_FALSE;
    }

    platform_dcache_enable();

    if (memTestDevice((volatile datum*)DDR_BASE, DDR_SIZE) != NULL)
    {
        result = WICED_FALSE;
    }

    return result;
}
