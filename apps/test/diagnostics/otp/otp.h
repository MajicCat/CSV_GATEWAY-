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

/******************************************************
 *                     Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define OTP_COMMANDS \
    { (char*) "otp_read_bit", otp_read_bit, 1, NULL, NULL, (char*) "<bit offset>", (char*) "Read OTP bit at specified bit offset"},

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
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

void otp_read_bit(unsigned int offset);

void otp_read_word(unsigned int wn );

void otp_dump(int arg );

void test_otp();

#ifdef __cplusplus
} /*extern "C" */
#endif
