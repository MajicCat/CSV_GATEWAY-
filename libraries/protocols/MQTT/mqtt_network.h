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
 * Network APIs.
 *
 * Internal, not to be used directly by applications.
 */
#pragma once

#include "wiced.h"
#include "mqtt_api.h"

#ifdef __cplusplus
extern "C" {
#endif

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

/******************************************************
 *                    Structures
 ******************************************************/

typedef struct
{
    wiced_tcp_socket_t              socket;
    wiced_queue_t                   queue;
    wiced_thread_t                  net_thread;
    wiced_ip_address_t              server_ip_address;
    uint16_t                        portnumber;
    void*                           p_user;
    wiced_tls_context_t             tls_context;
    wiced_tls_identity_t            tls_identity;
}mqtt_socket_t;

typedef struct wiced_mqtt_buffer_s
{
    wiced_packet_t*                 packet;
    uint8_t*                        data;
}wiced_mqtt_buffer_t;

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

/*
 * Network functions.
 *
 * Internal functions not to be used by user applications.
 */
wiced_result_t mqtt_network_init            ( const wiced_ip_address_t *server_ip_address, uint16_t portnumber, wiced_interface_t interface, void *p_user, mqtt_socket_t *socket, const wiced_mqtt_security_t *security );
wiced_result_t mqtt_network_deinit          ( mqtt_socket_t *socket );
wiced_result_t mqtt_network_connect         ( mqtt_socket_t *socket );
wiced_result_t mqtt_network_disconnect      ( mqtt_socket_t *socket );

wiced_result_t mqtt_network_create_buffer   ( wiced_mqtt_buffer_t *buffer, uint16_t size, mqtt_socket_t *socket );
wiced_result_t mqtt_network_send_buffer     ( const wiced_mqtt_buffer_t *buffer, mqtt_socket_t *socket );
wiced_result_t mqtt_network_delete_buffer   ( wiced_mqtt_buffer_t *buffer );

#ifdef __cplusplus
} /* extern "C" */
#endif
