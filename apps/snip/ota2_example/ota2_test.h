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

#include "http_stream.h"
#include "dns.h"

#include "ota2_test_dct.h"

/******************************************************
 *                      Macros
 ******************************************************/
/* log levels */
typedef enum {
    OTA2_LOG_ALWAYS      = 0,
    OTA2_LOG_ERROR,
    OTA2_LOG_WARNING,
    OTA2_LOG_NOTIFY,
    OTA2_LOG_INFO,
    OTA2_LOG_DEBUG,
    OTA2_LOG_DEBUG1,
} wiced_ota_log_level_t;

#define OTA2_APP_PRINT(level, arg)   {if (g_player != NULL){if (g_player->log_level >= level) WPRINT_MACRO(arg);}else{WPRINT_MACRO(arg);} }

/******************************************************
 *                    Constants
 ******************************************************/

#define OTA2_THREAD_STACK_SIZE  8000

#define PLAYER_TAG_VALID                    0x62EDBA26
#define PLAYER_TAG_INVALID                  0xDEADBEEF

#define OTA2_APP_URI_MAX               1024
#define OTA2_HTTP_QUERY_SIZE           1024
#define OTA2_HTTP_FILENAME_SIZE        1024

#define HTTP_PORT                           80
#define OTA2_HTTP_THREAD_PRIORITY      (WICED_DEFAULT_WORKER_PRIORITY)
#define OTA2_HTTP_STACK_SIZE           (6*1024)

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum {
    PLAYER_EVENT_SHUTDOWN           = (1 <<  0),

    PLAYER_EVENT_CONNECT            = (1 <<  1),
    PLAYER_EVENT_DISCONNECT         = (1 <<  2),
    PLAYER_EVENT_STATUS             = (1 <<  3),

    PLAYER_EVENT_GET_UPDATE         = (1 <<  5),
    PLAYER_EVENT_GET_UPDATE_NOW     = (1 <<  6),

    PLAYER_EVENT_FACTORY_RESET_STATUS = (1 <<  8),
    PLAYER_EVENT_FACTORY_RESET_NOW    = (1 <<  9),

    PLAYER_EVENT_UPDATE_NOW         = (1 << 10),
    PLAYER_EVENT_UPDATE_REBOOT      = (1 << 11),
    PLAYER_EVENT_UPDATE_STATUS      = (1 << 12),

    PLAYER_EVENT_LOG_LEVEL          = (1 << 20),

    PLAYER_EVENT_HTTP_THREAD_DONE   = (1 << 30),

} PLAYER_EVENTS_T;

#define PLAYER_ALL_EVENTS       (-1)

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/
#define MAX_HTTP_OBJECT_PATH        128

typedef struct ota2_data_s {
    uint32_t tag;

    wiced_ota_log_level_t           log_level;

    wiced_event_flags_t             events;

    platform_dct_network_config_t*  dct_network;
    platform_dct_wifi_config_t*     dct_wifi;
    ota2_dct_t*                     dct_app;

    wiced_thread_t                  http_thread;
    volatile wiced_thread_t*        http_thread_ptr;

    wiced_ip_address_t              ip_address;

    /* source info for http streaming */
    char*                           uri_desc;
    char                            uri_to_stream[OTA2_APP_URI_MAX];

    char                            http_query[OTA2_HTTP_QUERY_SIZE];       /* for building the http query */
    char                            server_uri[OTA2_APP_URI_MAX];
    char                            filename[OTA2_HTTP_FILENAME_SIZE];
    wiced_bool_t                    connect_state;      /* WICED_TRUE when connected to a server */
    wiced_tcp_socket_t              tcp_socket;
    wiced_bool_t                    tcp_socket_created;
    wiced_bool_t                    tcp_packet_pool_created;

    char                            last_connected_host_name[MAX_HTTP_HOST_NAME_SIZE + 1];
    uint16_t                        last_connected_port;

    /* debugging */
    wiced_time_t                    start_time;         /* when the app started */

    /* internal */
    void*                           internal;

} ota2_data_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

extern ota2_data_t *g_player;

/******************************************************
 *               Function Declarations
 ******************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif
