/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//#define TEST_STRING_SHORT "G'day from WICED!"
#define TEST_STRING_SHORT "hello"

/* must have an even number of characters */
#define TEST_STRING_LONG  "G'day from WICED! G'day from WICED! G'day from WICED! G'day from WICED! G'day from WICED! "

void uart_dut();

#ifdef __cplusplus
} /*extern "C" */
#endif
