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
 *  OTA2 platform-specific defines
 *
 *
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************
 *                     Macros
 ******************************************************/

/* To use a different CRC, define these values HERE in "Wiced-SDK/platforms/<platform>/platform_ota2_image.h"
 *  NOTE: changing the CRC routine requires re-building
 *        the OTA2 Image Creation Utility Program mk_ota2_imagexx.exe !!!
 */

//#define OTA2_CRC_INIT_VALUE                              CRC32_INIT_VALUE
//#define OTA2_CRC_FUNCTION(address, size, previous_value) (uint32_t)crc32(address, size, previous_value)
//typedef uint32_t    OTA2_CRC_VAR;
//#define OTA2_CRC_HTON(value)                             htonl(value)
//#define OTA2_CRC_NTOH(value)                             ntohl(value)

#ifdef __cplusplus
} /*extern "C" */
#endif

