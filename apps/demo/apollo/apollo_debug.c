/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

/** @file Apollo debug support routines
 *
 */

#include "apollo_debug.h"
#include "apollo_log.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define TCP_PACKET_MAX_DATA_LENGTH        1450
#define TCP_DEFAULT_SERVER_IP_ADDRESS     MAKE_IPV4_ADDRESS(192,168,0,200)
#define TCP_DEFAULT_SERVER_PORT           19702
#define TCP_CLIENT_CONNECT_TIMEOUT        500
#define TCP_CONNECTION_NUMBER_OF_RETRIES  3

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
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/

static wiced_tcp_socket_t tcp_client_socket;
static wiced_bool_t tcp_socket_initialized;

/******************************************************
 *               Function Definitions
 ******************************************************/

wiced_result_t apollo_debug_create_tcp_data_socket(wiced_ip_address_t* ip_addr, int port)
{
    wiced_ip_address_t server_ip_address;
    wiced_result_t     result;
    int                connection_retries;

    if (tcp_socket_initialized == WICED_TRUE)
    {
        return WICED_ERROR;
    }

    if (ip_addr == NULL)
    {
        SET_IPV4_ADDRESS(server_ip_address, TCP_DEFAULT_SERVER_IP_ADDRESS);
    }
    else
    {
        memcpy(&server_ip_address, ip_addr, sizeof(wiced_ip_address_t));
    }

    if (port <= 0)
    {
        port = TCP_DEFAULT_SERVER_PORT;
    }

    /* Create a TCP socket */
    if (wiced_tcp_create_socket(&tcp_client_socket, WICED_STA_INTERFACE) != WICED_SUCCESS)
    {
        apollo_log_msg(APOLLO_LOG_ERR, "TCP socket creation failed\n");
        return WICED_ERROR;
    }

    /* Bind to the socket */
    wiced_tcp_bind(&tcp_client_socket, port);

    /* Connect to the remote TCP server, try several times */
    connection_retries = 0;
    do
    {
        result = wiced_tcp_connect(&tcp_client_socket, &server_ip_address, port, TCP_CLIENT_CONNECT_TIMEOUT);
        connection_retries++;
    } while (result != WICED_SUCCESS && connection_retries < TCP_CONNECTION_NUMBER_OF_RETRIES);

    if (result != WICED_SUCCESS)
    {
        apollo_log_msg(APOLLO_LOG_ERR, "Unable to connect to the debug server!\n");
        return result;
    }

    tcp_socket_initialized = WICED_TRUE;

    return result;
}


wiced_result_t apollo_debug_close_tcp_data_socket(void)
{
    if (!tcp_socket_initialized)
    {
        return WICED_ERROR;
    }

    tcp_socket_initialized = WICED_FALSE;

    wiced_tcp_disconnect(&tcp_client_socket);
    wiced_tcp_delete_socket(&tcp_client_socket);

    return WICED_SUCCESS;
}


wiced_result_t apollo_debug_send_tcp_data(uint8_t* data, int datalen)
{
    wiced_packet_t* packet;
    char*           tx_data;
    uint16_t        available_data_length;
    wiced_result_t  result;

    if (tcp_socket_initialized != WICED_TRUE)
    {
        return WICED_ERROR;
    }

    if (datalen > TCP_PACKET_MAX_DATA_LENGTH)
    {
        apollo_log_msg(APOLLO_LOG_ERR, "Too much data %d (max %d)\n", datalen, TCP_PACKET_MAX_DATA_LENGTH);
        return WICED_ERROR;
    }

    /* Create the TCP packet. Memory for the tx_data is automatically allocated */
    if (wiced_packet_create_tcp(&tcp_client_socket, TCP_PACKET_MAX_DATA_LENGTH, &packet, (uint8_t**)&tx_data, &available_data_length) != WICED_SUCCESS)
    {
        apollo_log_msg(APOLLO_LOG_ERR, "TCP packet creation failed\n");
        return WICED_ERROR;
    }

    /* Write the message into tx_data"  */
    memcpy(tx_data, data, datalen);

    /* Set the end of the data portion */
    wiced_packet_set_data_end(packet, (uint8_t*)tx_data + datalen);

    /* Send the TCP packet */
    result = wiced_tcp_send_packet(&tcp_client_socket, packet);
    if (result != WICED_SUCCESS)
    {
        apollo_log_msg(APOLLO_LOG_ERR, "TCP packet send failed (%d)\n", result);

        /* Delete packet, since the send failed */
        wiced_packet_delete(packet);

        return WICED_ERROR;
    }

    return WICED_SUCCESS;
}


int apollo_debug_tcp_log_output_handler(APOLLO_LOG_LEVEL_T level, char *logmsg)
{
    apollo_debug_send_tcp_data((uint8_t*)logmsg, strlen(logmsg) + 1);

    return 0;
}
