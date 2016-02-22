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

#include "ssdp_app_dct.h"
#include "http_stream.h"
#include "daemons/SSDP/wiced_ssdp.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define SSDP_DEFAULT_PORT           (6900)

#define APP_TAG_VALID                0x42EDBABE
#define APP_TAG_INVALID              0xDEADBEEF

#define MSEARCH_RESPONSES_MAX      (16)         /* maximum M-Search responses we will store */

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum {
    APP_EVENT_SHUTDOWN          = (1 << 0),

    APP_EVENT_SSDP_INIT         = (1 << 1),
    APP_EVENT_SSDP_DEINIT       = (1 << 2),

    APP_EVENT_SSDP_MSEARCH      = (1 << 3),
    APP_EVENT_SSDP_INFORMATION  = (1 << 4),

    APP_EVENT_SSDP_LOG_LEVEL_OFF   = (1 << 5),
    APP_EVENT_SSDP_LOG_LEVEL_LOW   = (1 << 6),
    APP_EVENT_SSDP_LOG_LEVEL_INFO  = (1 << 7),
    APP_EVENT_SSDP_LOG_LEVEL_DEBUG = (1 << 8),

} APP_EVENTS_T;

#define APP_ALL_EVENTS       (-1)

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct application_info_s {
    uint32_t                        tag;

    int                             app_done;

    wiced_event_flags_t             events;

    wiced_ip_setting_t              ip_settings;

    application_dct_t               *dct_app;
    platform_dct_network_config_t   *dct_network;
    platform_dct_wifi_config_t      *dct_wifi;

    wiced_thread_t                  app_thread;
    wiced_thread_t                  *app_thread_ptr;

    wiced_ssdp_params_t             ssdp_params;
    void                            *ssdp_info;

    wiced_ssdp_msearch_response_t   msearch_responses[MSEARCH_RESPONSES_MAX];
} application_info_t;

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
} /* extern "C" */
#endif
