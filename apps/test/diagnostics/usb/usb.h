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
int usb_host_test(void);

int usb_host_test_storage_rw(int argc, char* argv[]);
int usb_host_test_storage_stress(int argc, char* argv[]);
int usb_host_test_storage_rw_throughput(int argc, char* argv[]);
int usb_host_test_hid_keyboard(int argc, char* argv[]);
int usb_host_test_hid_mouse(int argc, char* argv[]);


#ifdef __cplusplus
} /*extern "C" */
#endif
