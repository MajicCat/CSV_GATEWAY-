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

#include "wiced.h"
#include "apollo_dct.h"

/******************************************************
 *                      Macros
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

typedef struct
{
    platform_dct_network_config_t*  dct_network;
    platform_dct_wifi_config_t*     dct_wifi;
#ifdef WICED_DCT_INCLUDE_BT_CONFIG
    platform_dct_bt_config_t*       dct_bt;
#endif
    apollo_dct_t*                   dct_app;
} apollo_dct_collection_t;

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

wiced_result_t apollo_config_init(apollo_dct_collection_t *dct_tables);
wiced_result_t apollo_config_deinit(apollo_dct_collection_t *dct_tables);
void apollo_set_config(apollo_dct_collection_t* dct_tables, int argc, char *argv[]);
void apollo_config_print_info(apollo_dct_collection_t* dct_tables);
wiced_result_t apollo_config_save(apollo_dct_collection_t* dct_tables);

#ifdef __cplusplus
} /* extern "C" */
#endif
