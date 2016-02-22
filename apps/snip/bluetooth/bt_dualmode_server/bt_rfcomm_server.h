/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */
#include "bt_config_dct.h"

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
/** @file
 *
 * RFCOMM Server Sample Application
 *
 */
#define BT_RFCOMM_SERVER_APP_SCN        1   /* Service Class Number */
#define BT_RFCOMM_SERVER_APP_MTU        500

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Constants
 ****************************************************************************/

/*****************************************************************************
 * Globals
 *****************************************************************************/
extern wiced_bt_cfg_settings_t wiced_bt_cfg_settings;
extern const wiced_bt_cfg_buf_pool_t wiced_bt_cfg_buf_pools[];
extern const uint8_t wiced_bt_sdp_db[];
extern const uint16_t wiced_bt_sdp_db_size;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
