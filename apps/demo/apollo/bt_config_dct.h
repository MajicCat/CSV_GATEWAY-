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
extern "C"
{
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/* These are the default Bluetooth address, name and class-of-device */
#define WICED_BLUETOOTH_DEVICE_NAME    "Apollo_BT_Source"
#define WICED_BLUETOOTH_DEVICE_ADDRESS "\x11\x22\x33\xAA\xBB\xCC"

/* Service class: Audio(speakers, microphones,...), Major device class: Audio/Video, Minor device class: Portable-Audio
 * See here for more details- https://www.bluetooth.org/en-us/specification/assigned-numbers/baseband */

#define WICED_BLUETOOTH_DEVICE_CLASS   "\x20\x04\x1C"

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

#ifdef __cplusplus
} /*extern "C" */
#endif
