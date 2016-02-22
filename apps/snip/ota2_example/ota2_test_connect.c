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
 *
 * OTA Image test Application - connect & download NOW
 *
 * Over
 * The
 * Air
 *
 */

#include "ctype.h"
#include "wiced.h"
#include "wiced_tcpip.h"
#include "platform.h"
#include "platform_audio.h"
#include "command_console.h"
#include "console_wl.h"
#include "resources.h"
#include "internal/wwd_sdpcm.h"
#include "wiced_dct_common.h"

#include "ota2_test.h"
#include "ota2_test_config.h"

#ifdef WWD_TEST_NVRAM_OVERRIDE
#include "internal/bus_protocols/wwd_bus_protocol_interface.h"
#endif

#include "wiced_ota2_image.h"
#include "wiced_ota2_service.h"


/******************************************************
 *                      Macros
 ******************************************************/
/******************************************************
 *                    Constants
 ******************************************************/
#define OTA2_UPDATE_FILE_NAME          "brcmtest/OTA2_image_file.ota_image"

#define OTA2_APP_URI_MAX               1024
#define OTA2_HTTP_QUERY_SIZE           1024
#define OTA2_HTTP_FILENAME_SIZE        1024

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
#define WICED_OTA2_BUFFER_NODE_COUNT         (256)

#ifdef PLATFORM_L1_CACHE_BYTES
#define NUM_BUFFERS_POOL_SIZE(x)       ((WICED_LINK_MTU_ALIGNED + sizeof(wiced_packet_t) + 1) * (x))
#define APP_RX_BUFFER_POOL_SIZE        NUM_BUFFERS_POOL_SIZE(WICED_OTA2_BUFFER_NODE_COUNT)
#endif

#ifdef PLATFORM_L1_CACHE_BYTES
uint8_t                          ota2_rx_packets[APP_RX_BUFFER_POOL_SIZE + PLATFORM_L1_CACHE_BYTES]        __attribute__ ((section (".external_ram")));
#else
uint8_t                          ota2_rx_packets[WICED_NETWORK_MTU_SIZE * WICED_OTA2_BUFFER_NODE_COUNT]     __attribute__ ((section (".external_ram")));
#endif

/* template for HTTP GET */
static char ota2_get_request_template[] =
{
    "GET %s/%s HTTP/1.1\r\n"
    "Host: %s \r\n"
    "\r\n"
};

static char g_http_query[OTA2_HTTP_QUERY_SIZE];

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

/****************************************************************
 *  Console command Function Declarations
 ****************************************************************/

/****************************************************************
 *  HTTP URI connect / disconnect Function Declarations
 ****************************************************************/
wiced_result_t uri_split(const char* uri, char* host_buff, uint16_t host_buff_len, char* path_buff, uint16_t path_buff_len, uint16_t* port)
{
   const char *uri_start, *host_start, *host_end;
   const char *path_start;
   uint16_t host_len, path_len;

  if ((uri == NULL) || (host_buff == NULL) || (path_buff == NULL) || (port == NULL))
  {
      return WICED_ERROR;
  }

  *port = 0;

  /* drop http:// or htts://"  */
  uri_start = strstr(uri, "http");
  if (uri_start == NULL)
  {
      uri_start = uri;
  }
  if (strncasecmp(uri_start, "http://", 7) == 0)
  {
      uri_start += 7;
  }
  else if (strncasecmp(uri_start, "https://", 8) == 0)
  {
      uri_start += 8;
  }

  memset(host_buff, 0, host_buff_len);

  host_start = uri_start;
  host_len = strlen(host_start);
  host_end = strchr(host_start, ':');
  if (host_end != NULL)
  {
      *port = atoi(host_end + 1);
  }
  else
  {
      host_end = strchr(host_start, '/');
  }

  if (host_end != NULL)
  {
      host_len = host_end - host_start;
  }
  if( host_len > (host_buff_len - 1))
  {
      host_len = host_buff_len - 1;
  }
  memcpy(host_buff, host_start, host_len);

  memset(path_buff, 0, path_buff_len);
  path_start = strchr(host_start, '/');
  if( path_start != NULL)
  {
      path_len = strlen(path_start);
      if( path_len > (path_buff_len - 1))
      {
          path_len = path_buff_len - 1;
      }
      memcpy(path_buff, path_start, path_len);
  }


  return WICED_SUCCESS;
}

void ota2_print_connect_status(ota2_data_t *player)
{
    OTA2_APP_PRINT(OTA2_LOG_INFO, ("Connect State:   %d\r\n", player->connect_state));
    OTA2_APP_PRINT(OTA2_LOG_INFO, ("Connect State:   %s\r\n", player->last_connected_host_name));
    OTA2_APP_PRINT(OTA2_LOG_INFO, ("Connect State:   %d\r\n", player->last_connected_port));
    if (player->connect_state == WICED_TRUE) {
        OTA2_APP_PRINT(OTA2_LOG_INFO, ("Connect State:   %d\r\n", player->tcp_socket.socket.nx_tcp_socket_connect_port));
    }

}

wiced_result_t  ota2_check_socket_created(ota2_data_t* player)
{
    wiced_result_t result;
    if (player->tcp_socket_created == WICED_TRUE)
    {
        return WICED_SUCCESS;
    }

    result = wiced_tcp_create_socket( &player->tcp_socket, WICED_STA_INTERFACE );
    if ( result != WICED_SUCCESS )
    {
        OTA2_APP_PRINT(OTA2_LOG_ERROR, ("wiced_tcp_create_socket() failed!\r\n"));
        return result;
    }

    player->tcp_socket_created = WICED_TRUE;
    return result;
}

wiced_result_t  ota2_socket_destroy(ota2_data_t* player)
{
    wiced_result_t result;
    if (player->tcp_socket_created == WICED_FALSE)
    {
        return WICED_SUCCESS;
    }

    result = wiced_tcp_delete_socket( &player->tcp_socket );
    if ( result != WICED_SUCCESS )
    {
        OTA2_APP_PRINT(OTA2_LOG_ERROR, ("wiced_tcp_delete_socket() failed!\r\n"));
        return result;
    }

    player->tcp_socket_created = WICED_FALSE;
    return result;
}

wiced_result_t ota2_setup_stream_buffers(ota2_data_t *player)
{
    wiced_result_t result = WICED_SUCCESS;

    /* Guard against multiple creations of the packet pool */
    if (player->tcp_packet_pool_created == WICED_FALSE)
    {
        /* Create extra rx packet pool on the first megabyte of the external memory, this packet pool will be used for an audio queue */
        result = wiced_network_create_packet_pool(ota2_rx_packets, (uint32_t)sizeof(ota2_rx_packets), WICED_NETWORK_PACKET_RX);
        if (result != WICED_SUCCESS)
        {
            return result;
        }
        player->tcp_packet_pool_created = WICED_TRUE;
    }

    return WICED_SUCCESS;
}

wiced_result_t ota2_connect(ota2_data_t* player)
{
    wiced_result_t      result;
    wiced_ip_address_t  ip_address;
    uint16_t            port = 0;
    uint16_t            connect_tries;

    char                host_name[MAX_HTTP_HOST_NAME_SIZE];
    char                object_path[MAX_HTTP_OBJECT_PATH];

    if (player->connect_state == WICED_TRUE)
    {
        OTA2_APP_PRINT(OTA2_LOG_WARNING, ("ota2_connect() already connected!\r\n"));
        return WICED_SUCCESS;
    }

    if ( ota2_check_socket_created(player) != WICED_SUCCESS)
    {
        return WICED_ERROR;
    }

    result = uri_split(player->server_uri, host_name, sizeof(host_name), object_path, sizeof(object_path), &port);
    if (result != WICED_SUCCESS)
    {
        OTA2_APP_PRINT(OTA2_LOG_ERROR, ("ota2_connect() uri_split() failed %d!\r\n", result));
        return result;
    }

    /* check that we have a port # */
    if (port == 0)
    {
        port = HTTP_PORT;
    }


    OTA2_APP_PRINT(OTA2_LOG_ERROR, ("Connect to: host:%s path:%s port:%d\r\n", host_name, object_path, port) );


    if (isdigit((unsigned char)host_name[0]) && isdigit((unsigned char)host_name[1]) && isdigit((unsigned char)host_name[2])
            && host_name[3] == '.')
    {
        int         ip[4];
        char*       numeral;

        ip_address.version = WICED_IPV4;
        numeral = host_name;
        ip[0] = atoi(numeral);
        numeral = strchr(numeral, '.');
        if( numeral == NULL )
        {
            OTA2_APP_PRINT(OTA2_LOG_ERROR, ("ota2_connect() parsing URL numerically failed 1!\r\n"));
            return result;

        }
        numeral++;
        ip[1] = atoi(numeral);
        numeral = strchr(numeral, '.');
        if( numeral == NULL )
        {
            OTA2_APP_PRINT(OTA2_LOG_ERROR, ("ota2_connect() parsing URL numerically failed 2!\r\n"));
            return result;

        }
        numeral++;
        ip[2] = atoi(numeral);
        numeral = strchr(numeral, '.');
        if( numeral == NULL )
        {
            OTA2_APP_PRINT(OTA2_LOG_ERROR, ("ota2_connect() parsing URL numerically failed 3!\r\n"));
            return result;

        }
        numeral++;
        ip[3] = atoi(numeral);
        numeral = strchr(numeral, '.');

        SET_IPV4_ADDRESS( ip_address, MAKE_IPV4_ADDRESS(ip[0], ip[1], ip[2], ip[3]));

        OTA2_APP_PRINT(OTA2_LOG_INFO, ("Using (%ld.%ld.%ld.%ld)\r\n",
                ((ip_address.ip.v4 >> 24) & 0xff), ((ip_address.ip.v4 >> 16) & 0x0ff),
                ((ip_address.ip.v4 >>  8) & 0xff), ((ip_address.ip.v4 >>  0) & 0x0ff)) );

    }
    else
    {
        OTA2_APP_PRINT(OTA2_LOG_INFO, ("ota2_connect() dns_client_hostname_lookup(%s)!\r\n", host_name));
        result =  dns_client_hostname_lookup( host_name, &ip_address, 10000 );
        if (result!= WICED_SUCCESS)
        {
            OTA2_APP_PRINT(OTA2_LOG_ERROR, ("ota2_connect() dns_client_hostname_lookup(%s) failed!\r\n", host_name));
            return result;
        }
    }

    OTA2_APP_PRINT(OTA2_LOG_ERROR, ("player->tcp_socket.socket.nx_tcp_socket_ip_ptr == NULL!\r\n"));
    OTA2_APP_PRINT(OTA2_LOG_ERROR, ("player->tcp_socket.socket.nx_tcp_socket_ip_ptr->nx_ip_driver_link_up %d\r\n",
            ((player->tcp_socket.socket.nx_tcp_socket_ip_ptr == NULL) ? 0xFFFF :
    player->tcp_socket.socket.nx_tcp_socket_ip_ptr->nx_ip_driver_link_up) ));

    connect_tries = 0;
    result = WICED_ERROR;
    while ((connect_tries < 3) && (result == WICED_ERROR) )
    {
        OTA2_APP_PRINT(OTA2_LOG_ERROR, ("Try %d Connecting to %s:%d  (%ld.%ld.%ld.%ld) !\r\n",
                 connect_tries, host_name, port,
                ((ip_address.ip.v4 >> 24) & 0xff), ((ip_address.ip.v4 >> 16) & 0x0ff),
                ((ip_address.ip.v4 >>  8) & 0xff), ((ip_address.ip.v4 >>  0) & 0x0ff)) );
        result = wiced_tcp_connect( &player->tcp_socket, &ip_address, port, 2000 );
        connect_tries++;;
    }
    if ( result != WICED_SUCCESS )
    {
        OTA2_APP_PRINT(OTA2_LOG_ERROR, ("ota2_connect() wiced_tcp_connect() failed! %d\r\n", result));
        return result;
    }
    OTA2_APP_PRINT(OTA2_LOG_ERROR, ("Connected to %ld.%ld.%ld.%ld : %d !\r\n",
            ((ip_address.ip.v4 >> 24) & 0xff), ((ip_address.ip.v4 >> 16) & 0x0ff),
            ((ip_address.ip.v4 >>  8) & 0xff), ((ip_address.ip.v4 >>  0) & 0x0ff), port) );


    strcpy(player->last_connected_host_name, host_name);
    player->last_connected_port = port;
    player->connect_state = WICED_TRUE;

    ota2_print_connect_status(player);

    return result;
}

wiced_result_t ota2_disconnect(ota2_data_t* player)
{
    wiced_result_t result = WICED_SUCCESS;

    player->connect_state = WICED_FALSE;

    if (player->tcp_socket_created == WICED_TRUE)
    {
        result = ota2_socket_destroy(player);
        OTA2_APP_PRINT(OTA2_LOG_INFO, ("ota2_disconnect() ota2_socket_destroy() - %d\r\n", result));
        player->last_connected_port = 0;
    }

    ota2_print_connect_status(player);
    return result;
}

/* get the OTA Image file */
wiced_result_t ota2_get_OTA_file(const char* hostname, const char* filename, uint16_t port, wiced_tcp_socket_t* tcp_socket)
{
    wiced_result_t      result;
    char                port_name[16] = "\0";
    wiced_packet_t*     reply_packet;
    uint32_t            offset;
    wiced_bool_t        done;
    uint32_t            content_length;
    uint32_t            wait_loop_count;
    http_header_t       length_header, range_header;
    uint32_t            range_start, range_end;

    length_header.name = "Content-Length";
    length_header.value = NULL;

    range_header.name = "bytes";
    range_header.value = NULL;
    range_start = 0;
    range_end = 0;

    sprintf(port_name, ":%d", port);

    sprintf(g_http_query, ota2_get_request_template, hostname, filename, port_name);

    OTA2_APP_PRINT(OTA2_LOG_INFO, ("calling to Send query for OTA file: [%s]\r\n", g_http_query));

    result = wiced_tcp_send_buffer( tcp_socket, g_http_query, (uint16_t)strlen(g_http_query) );
    OTA2_APP_PRINT(OTA2_LOG_INFO, ("ota2_get_OTA_file() wiced_tcp_send_buffer() result %d [%s]!\r\n", result, g_http_query));
    if ( result != WICED_SUCCESS )
    {
        if (result == WICED_TCPIP_SOCKET_CLOSED)
        {
            OTA2_APP_PRINT(OTA2_LOG_DEBUG, ("wiced_tcp_send_buffer() wiced_tcp_receive() %d socket_closed!\r\n", result));
        }
        OTA2_APP_PRINT(OTA2_LOG_ERROR, ("ota2_get_OTA_file() wiced_tcp_send_buffer() failed %d [%s]!\r\n", result, g_http_query));
        return WICED_ERROR;
    }

    reply_packet = NULL;
    content_length = 0;
    result = WICED_SUCCESS;
    done = WICED_FALSE;
    offset = 0;
    wait_loop_count = 0;
    while ((result == WICED_SUCCESS) && (done == WICED_FALSE))
    {
        if (reply_packet != NULL)
        {
            /* free the packet */
            wiced_packet_delete( reply_packet );
        }
        reply_packet = NULL;

        result = wiced_tcp_receive( tcp_socket, &reply_packet, 1000 ); /* short timeout */
        if (result == WICED_TCPIP_TIMEOUT)
        {
            OTA2_APP_PRINT(OTA2_LOG_DEBUG, ("ota2_get_OTA_file() wiced_tcp_receive() %d timeout!\r\n", result));
            result = WICED_SUCCESS; /* so we stay in our loop */

            if ( wait_loop_count++ > 100)
            {
                OTA2_APP_PRINT(OTA2_LOG_ERROR, ("ota2_get_OTA() wiced_ota2_write_data() Timed out received:%ld of %ld!\r\n",
                                                          offset, content_length));
                goto _closed_socket;
            }
        }
        else if (result == WICED_TCPIP_SOCKET_CLOSED)
        {
            OTA2_APP_PRINT(OTA2_LOG_DEBUG, ("ota2_get_OTA_file() wiced_tcp_receive() %d socket_closed!\r\n", result));
            goto _closed_socket;
        }
        else if (reply_packet != NULL)
        {
            OTA2_APP_PRINT(OTA2_LOG_INFO, ("ota2_get_OTA_file() wiced_tcp_receive() result:%d reply_packet:%p\r\n", result, reply_packet));

            /* for this packet */
            uint8_t*            body;
            uint32_t            body_length;
            http_status_code_t  response_code;

            if (result != WICED_SUCCESS)
            {
                OTA2_APP_PRINT(OTA2_LOG_INFO, ("-------------------- wiced_tcp_receive() result:%d reply_packet:%p\r\n", result, reply_packet));
                result = WICED_SUCCESS; /* stay in the loop and try again */
                continue;
            }
            body = NULL;
            body_length = 0;

            response_code = 0;
            result = http_process_response( reply_packet, &response_code );
            OTA2_APP_PRINT(OTA2_LOG_INFO, ("http_process_response() result:%d response:%d -- continue\r\n ", result, response_code));
            if (result != WICED_SUCCESS )
            {
                OTA2_APP_PRINT(OTA2_LOG_INFO, ("HTTP response result:%d code: %d, continue to process (could be no header)\r\n ", result, response_code));
                result = WICED_SUCCESS; /* so we try again */
            }
            else
            {

                if (response_code < 100)
                {
                    /* do nothing here */
                }
                else if (response_code < 200 )
                {
                    /* 1xx (Informational): The request was received, continuing process */
                    continue;
                }
                else if (response_code < 300 )
                {
                    /* 2xx (Successful): The request was successfully received, understood, and accepted */
                }
                else if (response_code < 400 )
                {
                    /* 3xx (Redirection): Further action needs to be taken in order to complete the request */

                    OTA2_APP_PRINT(OTA2_LOG_INFO, ("HTTP response code: %d, redirection - code needed to handle this!\r\n ", response_code));
                    return WICED_ERROR;
                }
                else
                {
                    /* 4xx (Client Error): The request contains bad syntax or cannot be fulfilled */
                    OTA2_APP_PRINT(OTA2_LOG_INFO, ("HTTP response code: %d, ERROR!\r\n ", response_code));
                    return WICED_ERROR;
                }
            }

            if (content_length == 0)
            {
                http_extract_headers( reply_packet, &length_header, sizeof(http_header_t) );
                if (length_header.value != NULL)
                {
                    content_length = atol(length_header.value);
                }
            }
            if (range_start == 0)
            {
                range_end = 0;
                http_extract_headers( reply_packet, &range_header, sizeof(http_header_t) );
                if (range_header.value != NULL)
                {
                    char *minus;
                    range_start = atol(range_header.value);
                    minus = strchr(range_header.value, '-');
                    if (minus != NULL)
                    {
                        /* skip the minus */
                        minus++;
                        range_end = atol(minus);
                    }
                    OTA2_APP_PRINT(OTA2_LOG_ERROR, ("RANGE: %ld - %ld \r\n ", range_start, range_end));
                }
            }

            result = http_get_body( reply_packet, &body, &body_length );
            if ((result != WICED_SUCCESS) || (body == NULL))
            {
                /* get_body can fail if there is no header, try just getting the packet data */
                uint8_t* packet_data;
                uint16_t packet_data_length;
                uint16_t available_data_length;

                wiced_packet_get_data( reply_packet, 0, &packet_data, &packet_data_length, &available_data_length );

                OTA2_APP_PRINT(OTA2_LOG_INFO, ("ota2_get_OTA() http_get_body() failed, just use the data: packet_data:%p packet_data_length:%d available_data_length:%d\r\n", packet_data, packet_data_length, available_data_length));

                if ((packet_data != NULL) && (available_data_length != 0) && (available_data_length <= packet_data_length))
                {
                    body = packet_data;
                    body_length = available_data_length;
                    result = WICED_SUCCESS;
                }
            }

            /* if we got data, save it */
            if ((body != NULL) && (body_length > 0))
            {
#define BAR_GRAPH_LENGTH 48
                char            bar_graph[BAR_GRAPH_LENGTH] = {0};
                uint64_t        vert_bar;

                vert_bar = 0;
                if (content_length != 0)
                {
                    vert_bar = (offset * (BAR_GRAPH_LENGTH -2)) / content_length;
                }

                bar_graph[0] = '|';
                bar_graph[BAR_GRAPH_LENGTH - 2] = '|';
                memset( &bar_graph[1], '-', (BAR_GRAPH_LENGTH - 3));
                bar_graph[vert_bar] = '|';
                bar_graph[BAR_GRAPH_LENGTH - 1] = '\0';

                OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("%s\r", bar_graph));
                OTA2_APP_PRINT(OTA2_LOG_INFO, ("writing! offset:%ld body:%p length:%ld\r\n", offset, body, body_length));

                result = wiced_ota2_image_write_data(body, offset, body_length);
                if (result != WICED_SUCCESS)
                {
                    OTA2_APP_PRINT(OTA2_LOG_ERROR, ("---------------------- ota2_get_OTA() wiced_ota2_write_data() FAILED!\r\n"));
                }
                else
                {
                    offset += body_length;
                }
            }
        } /* reply packet != NULL */

        if ((content_length != 0) && (offset >= content_length))
        {
            wiced_ota2_image_update_staged_status(WICED_OTA2_IMAGE_EXTRACT_ON_NEXT_BOOT);
            OTA2_APP_PRINT(OTA2_LOG_ERROR, ("ota2_get_OTA() Finished %ld >= %ld !\r\n", offset, content_length));
            done = WICED_TRUE;
        }

    } /* while result == success && done == WICED_FALSE */

    if ((content_length != 0) && (offset < content_length))
    {
        wiced_ota2_image_update_staged_status(WICED_OTA2_IMAGE_INVALID);
        OTA2_APP_PRINT(OTA2_LOG_ERROR, ("ota2_get_OTA() FAILED %ld < %ld!\r\n", offset, content_length ));
        return WICED_ERROR;
    }

_closed_socket:
    if (reply_packet != NULL)
    {
        /* free the packet */
        wiced_packet_delete( reply_packet );
    }
    reply_packet = NULL;

    OTA2_APP_PRINT(OTA2_LOG_INFO, ("\r\n ota2_get_OTA_file() Exiting %d\r\n", result));

    return result;
}

