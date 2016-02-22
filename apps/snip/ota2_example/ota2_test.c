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
 * OTA Image test Application
 *
 * Over
 * The
 * Air
 *
 * Update Test
 *
 * This application snippet demonstrates how to use the WICED
 * interface for performing Over The Air Updates to your device
 *
 * Application Instructions
 * 1. Modify the CLIENT_AP_SSID/CLIENT_AP_PASSPHRASE Wi-Fi credentials
 *    in the wifi_config_dct.h header file to match your Wi-Fi access point
 * 2. Connect a PC terminal to the serial port of the WICED Eval board,
 *    then build and download the application as described in the WICED
 *    Quick Start Guide
 *
 * After the download completes, it connects to the Wi-Fi AP specified in apps/snip/ota2_test/wifi_config_dct.h
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
#include "wiced_ota2_service.h"

#ifdef WWD_TEST_NVRAM_OVERRIDE
#include "internal/bus_protocols/wwd_bus_protocol_interface.h"
#endif

#include "wiced_ota2_image.h"

/******************************************************
 *                      Macros
 ******************************************************/
#define CHECK_IOCTL_BUFFER( buff )  if ( buff == NULL ) {  wiced_assert("Allocation failed\n", 0 == 1); return WWD_BUFFER_ALLOC_FAIL; }
#define CHECK_RETURN( expr )  { wwd_result_t check_res = (expr); if ( check_res != WWD_SUCCESS ) { wiced_assert("Command failed\n", 0 == 1); return check_res; } }

#define Mod32_GT( A, B )        ( (int32_t)( ( (uint32_t)( A ) ) - ( (uint32_t)( B ) ) ) >   0 )

#define OTA2_CONSOLE_COMMANDS \
    { (char*) "exit",           ota2_console_command,    0, NULL, NULL, (char *)"", (char *)"Exit application" }, \
    { (char*) "log",            ota2_console_command,    0, NULL, NULL, (char *)"", (char *)"Set log level (0-5)" }, \
    { (char*) "get_update",     ota2_console_command,    0, NULL, NULL, (char *)"", (char *)"Get OTA update - read from connection" }, \
    { (char*) "get_update_now", ota2_console_command,    0, NULL, NULL, (char *)"", (char *)"Get OTA update NOW! no connection made yet" }, \
    { (char*) "factory_status", ota2_console_command,    0, NULL, NULL, (char *)"", (char *)"Factory Reset - show status" }, \
    { (char*) "factory_now",    ota2_console_command,    0, NULL, NULL, (char *)"", (char *)"Factory Reset - extract NOW" }, \
    { (char*) "update_now",     ota2_console_command,    0, NULL, NULL, (char *)"", (char *)"OTA update - Update from staging now" }, \
    { (char*) "update_reboot",  ota2_console_command,    0, NULL, NULL, (char *)"", (char *)"OTA update - Update from staging on boot" }, \
    { (char*) "update_status",  ota2_console_command,    0, NULL, NULL, (char *)"", (char *)"OTA update - show status / valid" }, \
    { (char*) "config",         ota2_console_command,    0, NULL, NULL, (char *)"", (char *)"Display / change config values" }, \
    { (char*) "connect",        ota2_console_command,    0, NULL, NULL, (char *)"", (char *)"Connect <xxx.xxx.xxx.xxx> to update server" }, \
    { (char*) "disconnect",     ota2_console_command,    0, NULL, NULL, (char *)"", (char *)"Disconnect from update server" }, \
    { (char*) "status",         ota2_console_command,    0, NULL, NULL, (char *)"", (char *)"show status" }, \


/******************************************************
 *                    Constants
 ******************************************************/

#define MY_DEVICE_NAME                      "ota2_test"
#define MY_DEVICE_MODEL                     "1.0"
#define MAX_COMMAND_LENGTH                   (85)
#define CONSOLE_COMMAND_HISTORY_LENGTH      (10)

#define FIRMWARE_VERSION                    "wiced-1.0"

#define MILLISECONDS_PER_SECOND             (uint64_t)(1000)
#define SECONDS_PER_MINUTE                  (uint64_t)(60)

#define OTA2_UPDATE_FILE_NAME          "/brcmtest/OTA2_image_file.ota_image"

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum {
    OTA2_CONSOLE_CMD_EXIT = 0,

    OTA2_CONSOLE_CMD_CONFIG,

    OTA2_CONSOLE_CMD_CONNECT,
    OTA2_CONSOLE_CMD_DISCONNECT,
    OTA2_CONSOLE_CMD_STATUS,

    OTA2_CONSOLE_CMD_GET_UPDATE,
    OTA2_CONSOLE_CMD_GET_UPDATE_NOW,

    OTA2_CONSOLE_CMD_FACTORY_RESET_STATUS,
    OTA2_CONSOLE_CMD_FACTORY_NOW,

    OTA2_CONSOLE_CMD_UPDATE_NOW,
    OTA2_CONSOLE_CMD_UPDATE_REBOOT,
    OTA2_CONSOLE_CMD_UPDATE_STATUS,

    OTA2_CONSOLE_CMD_LOG_LEVEL,


    OTA2_CONSOLE_CMD_MAX,
} OTA2_CONSOLE_CMDS_T;

#define NUM_NSECONDS_IN_SECOND                      (1000000000LL)
#define NUM_USECONDS_IN_SECOND                      (1000000)
#define NUM_NSECONDS_IN_MSECOND                     (1000000)
#define NUM_NSECONDS_IN_USECOND                     (1000)
#define NUM_USECONDS_IN_MSECOND                     (1000)
#define NUM_MSECONDS_IN_SECOND                      (1000)

/******************************************************
 *                 Type Definitions
 ******************************************************/


/******************************************************
 *                    Structures
 ******************************************************/

typedef struct cmd_lookup_s {
        char *cmd;
        uint32_t event;
} cmd_lookup_t;

/******************************************************
 *               Function Declarations
 ******************************************************/

int ota2_console_command(int argc, char *argv[]);

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

static char ota2_command_buffer[MAX_COMMAND_LENGTH];
static char ota2_command_history_buffer[MAX_COMMAND_LENGTH * CONSOLE_COMMAND_HISTORY_LENGTH];

uint8_t ota2_thread_stack_buffer[OTA2_THREAD_STACK_SIZE]                               __attribute__ ((section (".bss.ccm")));

const command_t ota2_command_table[] = {
    OTA2_CONSOLE_COMMANDS
    WL_COMMANDS
    CMD_TABLE_END
};

static cmd_lookup_t command_lookup[OTA2_CONSOLE_CMD_MAX] = {
        { "exit",           PLAYER_EVENT_SHUTDOWN             },
        { "config",         0                                 },
        { "connect",        PLAYER_EVENT_CONNECT              },
        { "disconnect",     PLAYER_EVENT_DISCONNECT           },
        { "status",         PLAYER_EVENT_STATUS               },
        { "get_update",     PLAYER_EVENT_GET_UPDATE           },
        { "get_update_now", PLAYER_EVENT_GET_UPDATE_NOW       },
        { "factory_status", PLAYER_EVENT_FACTORY_RESET_STATUS },
        { "factory_now",    PLAYER_EVENT_FACTORY_RESET_NOW    },
        { "update_now",     PLAYER_EVENT_UPDATE_NOW           },
        { "update_reboot",  PLAYER_EVENT_UPDATE_REBOOT        },
        { "update_status",  PLAYER_EVENT_UPDATE_STATUS        },
        { "log",            PLAYER_EVENT_LOG_LEVEL            },
};


/* template for HTTP GET */
char ota2_get_request_template[] =
{
    "GET %s HTTP/1.1\r\n"
    "Host: %s%s \r\n"
    "\r\n"
};

const char* firmware_version = FIRMWARE_VERSION;

ota2_data_t *g_player;

char g_http_query[OTA2_HTTP_QUERY_SIZE];

/******************************************************
 *               Function Declarations
 ******************************************************/
wiced_result_t over_the_air_2_app_restore_settings_ater_update( ota2_data_t* player );

static ota2_data_t* init_player(void);
static void ota2_test_mainloop(ota2_data_t *player);
static void ota2_test_shutdown(ota2_data_t *player)
;

/******************************************************
 *               Function Definitions
 ******************************************************/

void application_start(void)
{
    ota2_data_t*   player;

    /*
     * Main initialization.
     */

    if ((player = init_player()) == NULL)
    {
        return;
    }
    g_player = player;

    wiced_time_get_time( &g_player->start_time );

    /*
     * Drop into our main loop.
     */
    ota2_test_mainloop(player);

    /*
     * Cleanup and exit.
     */

    g_player = NULL;
    ota2_test_shutdown(player);
    player = NULL;
}

/****************************************************************
 *  Console command Function Declarations
 ****************************************************************/
int printf_memory( char *message, uint8_t*addr, uint16_t length)
{
    uint16_t offset, i = 0;
    if (addr == NULL)
        return 0;

    printf("\r\nMemory  addr:%p len:%d dump: %s\r\n", addr, length, message);
    for (offset = 0; offset < length; offset += 16)
    {
        printf("%p  ", &addr[offset]);
        for (i= offset; (i < (offset + 16)) && (i < length); i++)
        {
            printf("%02x ", addr[i]);
        }
        printf("    ");
        for (i= offset; (i < (offset + 16)) && (i < length); i++)
        {
            printf("%c ", (isprint(addr[i]) ? addr[i] : ' '));
        }
        printf("\r\n");
    }

    return offset + i;
}



int ota2_console_command(int argc, char *argv[])
{
    uint32_t event = 0;
    int i;

    OTA2_APP_PRINT(OTA2_LOG_INFO, ("Received command: %s\n", argv[0]));

    if (g_player == NULL || g_player->tag != PLAYER_TAG_VALID)
    {
        OTA2_APP_PRINT(OTA2_LOG_DEBUG, ("ota2_console_command() Bad player structure\r\n"));
        return ERR_CMD_OK;
    }

    /*
     * Lookup the command in our table.
     */

    for (i = 0; i < OTA2_CONSOLE_CMD_MAX; ++i)
    {
        if (strcmp(command_lookup[i].cmd, argv[0]) == 0)
            break;
    }

    if (i >= OTA2_CONSOLE_CMD_MAX)
    {
        OTA2_APP_PRINT(OTA2_LOG_DEBUG, ("Unrecognized command: %s\n", argv[0]));
        return ERR_CMD_OK;
    }

    switch (i)
    {
        case OTA2_CONSOLE_CMD_EXIT:
            break;
        case OTA2_CONSOLE_CMD_CONNECT:
            memset(g_player->server_uri, 0, sizeof(g_player->server_uri));
            if (argc > 1)
            {
                    strncpy(g_player->server_uri, argv[1], (sizeof(g_player->server_uri) - 1) );
            }
            event = command_lookup[i].event;
            break;
        case OTA2_CONSOLE_CMD_DISCONNECT:
        case OTA2_CONSOLE_CMD_STATUS:
            event = command_lookup[i].event;
            break;
        case OTA2_CONSOLE_CMD_GET_UPDATE:
        case OTA2_CONSOLE_CMD_GET_UPDATE_NOW:
            memset(g_player->filename, 0, sizeof(g_player->filename));
            if (argc > 1)
            {
                strlcpy(g_player->filename, argv[1], (sizeof(g_player->filename) - 1) );
            }
            else
            {
                strlcpy(g_player->filename, OTA2_UPDATE_FILE_NAME, (sizeof(g_player->filename) - 1) );
            }
            OTA2_APP_PRINT(OTA2_LOG_INFO, (" Get update file:%s\r\n", g_player->filename));
            event = command_lookup[i].event;
            break;
        case OTA2_CONSOLE_CMD_LOG_LEVEL:
            if (argc > 1)
            {
                int new_level;
                new_level = atoi(argv[1]);
                if ((new_level > 0) && (new_level < OTA2_LOG_DEBUG1))
                {
                    g_player->log_level = new_level;
                }
            }
            OTA2_APP_PRINT(OTA2_LOG_ALWAYS, ("log level = %d\r\n", g_player->log_level));
            break;
        case OTA2_CONSOLE_CMD_FACTORY_RESET_STATUS:
        case OTA2_CONSOLE_CMD_FACTORY_NOW:
        case OTA2_CONSOLE_CMD_UPDATE_NOW:
        case OTA2_CONSOLE_CMD_UPDATE_REBOOT:
        case OTA2_CONSOLE_CMD_UPDATE_STATUS:
            event = command_lookup[i].event;
            break;

        case OTA2_CONSOLE_CMD_CONFIG:
            ota2_set_config(g_player, argc, argv);
            break;
    }

    if (event)
    {
        /*
         * Send off the event to the main audio loop.
         */

        wiced_rtos_set_event_flags(&g_player->events, event);
    }

    return ERR_CMD_OK;
}

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

void host_get_firmware_version(const char **firmware_string)
{
    *firmware_string = firmware_version;
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

#if 0
    if (isdigit((unsigned char)host_name[0]) &&
        (isdigit((unsigned char)host_name[1]) || (host_name[1] == '.')) &&
        (isdigit((unsigned char)host_name[2]) || (host_name[2] == '.') )
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
#endif
    {
        OTA2_APP_PRINT(OTA2_LOG_ERROR, ("Connect to: line:%d 0x%p  %c%c%c%c\r\n", __LINE__, host_name, host_name[0], host_name[1], host_name[2], host_name[3]) );
        OTA2_APP_PRINT(OTA2_LOG_INFO, ("ota2_connect() dns_client_hostname_lookup(%s)!\r\n", host_name));

        result =  dns_client_hostname_lookup( host_name, &ip_address, 10000 );
        OTA2_APP_PRINT(OTA2_LOG_ERROR, ("Connect to: line:%d\r\n", __LINE__) );
        if (result!= WICED_SUCCESS)
        {
            OTA2_APP_PRINT(OTA2_LOG_ERROR, ("ota2_connect() dns_client_hostname_lookup(%s) failed!\r\n", host_name));
            return result;
        }
    }

    OTA2_APP_PRINT(OTA2_LOG_ERROR, ("Connect to: line:%d\r\n", __LINE__) );

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

    sprintf(g_http_query, ota2_get_request_template, filename, hostname, port_name);

    OTA2_APP_PRINT(OTA2_LOG_INFO, ("calling to Send query for OTA file: [%s]\r\n", g_http_query));

    result = wiced_tcp_send_buffer( tcp_socket, g_http_query, (uint16_t)strlen(g_http_query) );
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

        result = wiced_tcp_receive( tcp_socket, &reply_packet, 1000 ); /* 1000 short timeout ? */
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
                if (result == WICED_SUCCESS)
                {
                    offset += body_length;
                }
            }
        } /* reply packet != NULL */

        if ((content_length != 0) && (offset >= content_length))    // TODO: check auto_update flag
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
        if (result == WICED_SUCCESS)
        {
            result = WICED_ERROR;
        }
        return result;
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

wiced_result_t ota2_test_get_update_now(ota2_data_t* player)
{
#if 0   // NOT_READY_YET
    wiced_result_t  result;
    void*           session_id;

    wiced_ota2_backround_service_params_t params;

    params.auto_update = 0;
    params.check_interval = 5;
    params.file_name = "brcmtest/OTA_file_image.ota_image";
    params.retry_check_interval = 0;
    params.url = "stevedefrisco.com";

    session_id = wiced_ota2_service_init(&params, player);
    if ( session_id == NULL )
    {
        OTA2_APP_PRINT(OTA2_LOG_WARNING, ("Call to wiced_ota2_service_init() failed\r\n"));
        return WICED_ERROR;
    }
    result = wiced_ota2_service_check_for_updates(session_id);

    OTA2_APP_PRINT(OTA2_LOG_WARNING, ("wiced_ota2_service_check_for_updates() returned:%d\r\n", result));

    wiced_ota2_service_deinit(session_id);

    return result;
#else
    return WICED_FALSE;
#endif
}

/****************************************************************
 *  Application Main loop Function
 ****************************************************************/

static void ota2_test_mainloop(ota2_data_t *player)
{
    wiced_result_t      result;
    uint32_t            events;

    OTA2_APP_PRINT(OTA2_LOG_DEBUG, ("Begin ota2_test mainloop\r\n"));

    /*
     * If auto play is set then start off by sending ourselves a play event.
     */

    while (player->tag == PLAYER_TAG_VALID)
    {
        events = 0;

        result = wiced_rtos_wait_for_event_flags(&player->events, PLAYER_ALL_EVENTS, &events,
                                                 WICED_TRUE, WAIT_FOR_ANY_EVENT, WICED_WAIT_FOREVER);
        if (result != WICED_SUCCESS)
        {
            continue;
        }

        if (events & PLAYER_EVENT_SHUTDOWN)
        {
            OTA2_APP_PRINT(OTA2_LOG_DEBUG, ("mainloop received EVENT_SHUTDOWN\r\n"));
            break;
        }

        if (events & PLAYER_EVENT_CONNECT)
        {
            OTA2_APP_PRINT(OTA2_LOG_DEBUG, ("mainloop received EVENT_CONNECT\r\n"));

            if (player->connect_state != WICED_TRUE)
            {
                result = ota2_connect(player);
                if (result != WICED_SUCCESS)
                {
                    OTA2_APP_PRINT(OTA2_LOG_INFO, ("mainloop ota2_connect() fail:%d %s\r\n", result, player->server_uri));
                }
            }
            else
            {
                OTA2_APP_PRINT(OTA2_LOG_ERROR, ("Already connected to %s\r\n", player->server_uri));
            }

        }

        if (events & PLAYER_EVENT_DISCONNECT)
        {
            OTA2_APP_PRINT(OTA2_LOG_DEBUG, ("mainloop received EVENT_DISCONNECT\r\n"));
            ota2_disconnect(player);
        }

        if (events & PLAYER_EVENT_STATUS)
        {
            OTA2_APP_PRINT(OTA2_LOG_DEBUG, ("mainloop received EVENT_STATUS\r\n"));
            ota2_print_connect_status(player);
        }

        if (events & PLAYER_EVENT_GET_UPDATE)
        {
            /* get the image from the server & save in staging area */
            OTA2_APP_PRINT(OTA2_LOG_ALWAYS, ("Download the OTA Image file\r\n"));

            result = ota2_check_socket_created(player);
            if (result != WICED_SUCCESS)
            {
                OTA2_APP_PRINT(OTA2_LOG_ERROR, ("Failed to create the socket! %d created:%d\r\n", result, player->tcp_socket_created));
            }
            else
            {
                result = ota2_get_OTA_file(player->last_connected_host_name, player->filename, player->last_connected_port, &player->tcp_socket);
                if (result != WICED_SUCCESS)
                {
                        OTA2_APP_PRINT(OTA2_LOG_ERROR, ("Download failed! %d \r\n", result));
                }
                else
                {
                    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("Download Success!\r\n"));
                    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("Now use update_now to extract and reboot, or just reboot (extraction will happen after reboot)!\r\n"));
                }
            }
        }

        if (events & PLAYER_EVENT_GET_UPDATE_NOW)
        {
            result = ota2_test_get_update_now(player);
            if (result != WICED_SUCCESS)
            {
                    OTA2_APP_PRINT(OTA2_LOG_ERROR, ("PLAYER_EVENT_GET_UPDATE_NOW Download failed! %d \r\n", result));
            }
            else
            {
                OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("PLAYER_EVENT_GET_UPDATE_NOW Download Success!\r\n"));
                OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("now use update_status, update_now, or update_reboot (then reboot)!\r\n"));
            }
        }

        if (events & PLAYER_EVENT_FACTORY_RESET_STATUS)
        {
            OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("PLAYER_EVENT_UPDATE Status validate\n"));
            wiced_ota2_image_validate ( WICED_OTA2_IMAGE_TYPE_FACTORY_RESET_APP );
        }

        if (events & PLAYER_EVENT_FACTORY_RESET_NOW)
        {
            /* extract the image in the staging area */
            OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("Extract the Factory Reset App NOW\r\n"));
            wiced_ota2_image_extract ( WICED_OTA2_IMAGE_TYPE_FACTORY_RESET_APP );
            OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("Reboot now to see change!\r\n"));
        }

        if (events & PLAYER_EVENT_UPDATE_NOW)
        {
            /* extract the image in the staging area */
            OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("Extract the update NOW\r\n"));
            result = wiced_dct_ota2_save_copy(OTA2_BOOT_UPDATE);
            OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("Update NOW - copied DCT:%d\r\n", result));
            wiced_ota2_image_fakery(WICED_OTA2_IMAGE_DOWNLOAD_COMPLETE);
            wiced_ota2_image_extract ( WICED_OTA2_IMAGE_TYPE_STAGED );
            OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("Reboot now to see change!\r\n"));
        }

        if (events & PLAYER_EVENT_UPDATE_REBOOT)
        {
            OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("Mark the update to be extracted on next Boot.\r\n"));
            wiced_ota2_image_fakery(WICED_OTA2_IMAGE_EXTRACT_ON_NEXT_BOOT);
            OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("ReBoot now to watch the extraction.\r\n"));
        }

        if (events & PLAYER_EVENT_UPDATE_STATUS)
        {
            OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("PLAYER_EVENT_UPDATE Status validate\n"));
            wiced_ota2_image_validate ( WICED_OTA2_IMAGE_TYPE_STAGED );
        }

    }   /* while */

    OTA2_APP_PRINT(OTA2_LOG_DEBUG, ("End ota2_test mainloop\r\n"));
}

wiced_result_t ota2_network_up_default( ota2_data_t* player, wiced_network_config_t config, const wiced_ip_setting_t* static_ip_settings )
{
    wiced_result_t result = WICED_SUCCESS;
    wiced_buffer_t buffer;
    wwd_result_t retval;
    uint32_t* data;

    /*
     * Make sure that the network interfaces are down.
     * Later we may want to just take them down, reconfigure,
     * and bring them back up but for now it's a one time operation.
     */

    if (wiced_network_is_up(WICED_AP_INTERFACE) != WICED_FALSE && wiced_network_is_up(WICED_STA_INTERFACE) != WICED_FALSE)
    {
        OTA2_APP_PRINT(OTA2_LOG_INFO, ("Network already initialized\r\n"));
        return WICED_ALREADY_INITIALIZED;
    }

    /*
     * RMC only works with an ADHOC (IBSS) network. So if the configuration is for something else
     * we can just do a standard network up.
     */

    if (player->dct_wifi->stored_ap_list[0].details.bss_type != WICED_BSS_TYPE_ADHOC)
    {
        return wiced_network_up_default( &player->dct_network->interface, static_ip_settings );
    }

    OTA2_APP_PRINT(OTA2_LOG_INFO, ("Configuring wireless interface for RMC\r\n"));

    /*
     * Bring down the WiFi interface.
     */
    wwd_wifi_set_down();

    /* Turn APSTA off */
    data = (uint32_t*)wwd_sdpcm_get_iovar_buffer(&buffer, (uint16_t)4, IOVAR_STR_APSTA);
    CHECK_IOCTL_BUFFER(data);
    *data = (uint32_t)0;
    /* This will fail on manufacturing test build since it does not have APSTA available */
    retval = wwd_sdpcm_send_iovar(SDPCM_SET, buffer, 0, WWD_STA_INTERFACE);
    if (retval != WWD_SUCCESS && retval != WWD_UNSUPPORTED)
    {
        /* Could not turn off APSTA */
        OTA2_APP_PRINT(OTA2_LOG_INFO, ("Could not turn off APSTA\r\n"));
    }

    /*
     * Make sure that AP is 0.
     */

    data = (uint32_t*)wwd_sdpcm_get_ioctl_buffer(&buffer, 4);
    CHECK_IOCTL_BUFFER(data);
    *data = (uint32_t)0;
    CHECK_RETURN(wwd_sdpcm_send_ioctl(SDPCM_SET, (uint32_t)WLC_SET_AP, buffer, 0, WWD_STA_INTERFACE));

    /*
     * Set MPC to 0.
     */

    data = (uint32_t*)wwd_sdpcm_get_iovar_buffer(&buffer, (uint16_t)8, IOVAR_STR_MPC);
    CHECK_IOCTL_BUFFER(data);
    data[0] = (uint32_t)CHIP_STA_INTERFACE;
    data[1] = (uint32_t)0;
    CHECK_RETURN(wwd_sdpcm_send_iovar(SDPCM_SET, buffer, 0, WWD_STA_INTERFACE));

    /* Set Power Management 0 */
    wiced_wifi_disable_powersave();

    /*
     * Set channel
     * First peer joining the ad-hoc network ultimately dictates which channel is going to be used by all the others
     */

    data = (uint32_t*)wwd_sdpcm_get_ioctl_buffer(&buffer, 4);
    CHECK_IOCTL_BUFFER(data);
    *data = (uint32_t)player->dct_wifi->stored_ap_list[0].details.channel;
    CHECK_RETURN(wwd_sdpcm_send_ioctl(SDPCM_SET, (uint32_t)WLC_SET_CHANNEL, buffer, 0, WWD_STA_INTERFACE));

    /*
     * Set rmc_ackreq to 1.
     */

    data = (uint32_t*) wwd_sdpcm_get_iovar_buffer(&buffer, (uint16_t)4, IOVAR_STR_RMC_ACKREQ);
    CHECK_IOCTL_BUFFER(data);
    *data = 1;
    CHECK_RETURN(wwd_sdpcm_send_iovar(SDPCM_SET, buffer, 0, WWD_STA_INTERFACE));


    /*
     * Now bring up the network.
     */
    wwd_wifi_set_up();

    result = wiced_network_up(WICED_STA_INTERFACE, config, static_ip_settings);
    if ( result != WICED_SUCCESS )
    {
        OTA2_APP_PRINT(OTA2_LOG_ERROR, ("Could not bring up network!\r\n"));
        return result;
    }
    player->dct_network->interface = WICED_STA_INTERFACE;

    return result;
}

static void ota2_test_shutdown(ota2_data_t *player)
{

    /*
     * Shutdown the console.
     */

    command_console_deinit();

    if (player->tcp_socket_created == WICED_TRUE)
    {
        wiced_tcp_delete_socket( &player->tcp_socket );
        OTA2_APP_PRINT(OTA2_LOG_INFO, ("delete_socket()\r\n"));
    }
    player->tcp_socket_created = WICED_FALSE;

    wiced_rtos_deinit_event_flags(&player->events);

    wiced_dct_read_unlock(player->dct_network, WICED_TRUE);
    wiced_dct_read_unlock(player->dct_wifi, WICED_TRUE);
    wiced_dct_read_unlock(player->dct_app, WICED_TRUE);

    player->tag = PLAYER_TAG_INVALID;
    free(player);
}

static void set_nvram_mac(void)
{
#ifdef WWD_TEST_NVRAM_OVERRIDE
    platform_dct_wifi_config_t dct_wifi;
    wiced_result_t result;
    uint32_t size;
    uint32_t i;
    char *nvram;

    result = wiced_dct_read_with_copy(&dct_wifi, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t));
    if (result != WICED_SUCCESS)
    {
        return;
    }

    if (wwd_bus_get_wifi_nvram_image(&nvram, &size) != WWD_SUCCESS)
    {
        return;
    }

    /*
     * We have the mac address from the DCT so now we just need to update the nvram image.
     * Search for the 'macaddr=' token.
     */

    for (i = 0; i < size; )
    {
        if (nvram[i] == '\0')
        {
            break;
        }

        if (nvram[i] != 'm' || nvram[i+1] != 'a' || nvram[i+2] != 'c' || nvram[i+3] != 'a' ||
            nvram[i+4] != 'd' || nvram[i+5] != 'd' || nvram[i+6] != 'r' || nvram[7] != '=')
        {
            while(i < size && nvram[i] != '\0')
            {
                i++;
            }
            i++;
            continue;
        }

        /*
         * Found the macaddr token. Now we just need to update it.
         */

        sprintf(&nvram[i+8], "%02x:%02x:%02x:%02x:%02x:%02x", dct_wifi.mac_address.octet[0],
                dct_wifi.mac_address.octet[1], dct_wifi.mac_address.octet[2], dct_wifi.mac_address.octet[3],
                dct_wifi.mac_address.octet[4], dct_wifi.mac_address.octet[5]);
        break;
    }
#endif
}


static ota2_data_t* init_player(void)
{
    ota2_data_t*        player = NULL;
    wiced_result_t      result;
    uint32_t            tag;
    ota2_boot_type_t    boot_type;

    tag = PLAYER_TAG_VALID;

    /*
     * Temporary to work around a WiFi bug with RMC.
     */

    set_nvram_mac();

    /* Initialize the device */
    result = wiced_init();
    if (result != WICED_SUCCESS)
    {
        return NULL;
    }

   /*
     * Allocate the main data structure.
     */
    player = calloc_named("ota2_test", 1, sizeof(ota2_data_t));
    if (player == NULL)
    {
        OTA2_APP_PRINT(OTA2_LOG_ALWAYS, ("Unable to allocate player structure\r\n"));
        return NULL;
    }

    player->log_level = OTA2_LOG_DEBUG;

    /*
     * Create the command console.
     */

    OTA2_APP_PRINT(OTA2_LOG_INFO, ("Start the command console\r\n"));
    result = command_console_init(STDIO_UART, sizeof(ota2_command_buffer), ota2_command_buffer, CONSOLE_COMMAND_HISTORY_LENGTH, ota2_command_history_buffer, " ");
    if (result != WICED_SUCCESS)
    {
        OTA2_APP_PRINT(OTA2_LOG_DEBUG, ("Error starting the command console\r\n"));
        free(player);
        return NULL;
    }
    console_add_cmd_table(ota2_command_table);
    /*
     * Create our event flags.
     */

    result = wiced_rtos_init_event_flags(&player->events);
    if (result != WICED_SUCCESS)
    {
        OTA2_APP_PRINT(OTA2_LOG_DEBUG, ("Error initializing event flags\r\n"));
        tag = PLAYER_TAG_INVALID;
    }

    OTA2_APP_PRINT(OTA2_LOG_INFO, ("OTA2_IMAGE_FLASH_BASE                0x%08lx\n", (uint32_t)OTA2_IMAGE_FLASH_BASE));
    OTA2_APP_PRINT(OTA2_LOG_INFO, ("OTA2_IMAGE_FACTORY_RESET_AREA_BASE   0x%08lx\n", (uint32_t)OTA2_IMAGE_FACTORY_RESET_AREA_BASE));
    OTA2_APP_PRINT(OTA2_LOG_INFO, ("OTA2_IMAGE_APP_DCT_SAVE_AREA_BASE    0x%08lx\n", (uint32_t)OTA2_IMAGE_APP_DCT_SAVE_AREA_BASE));
    OTA2_APP_PRINT(OTA2_LOG_INFO, ("OTA2_IMAGE_CURRENT_AREA_BASE         0x%08lx\n", (uint32_t)OTA2_IMAGE_CURRENT_AREA_BASE));
    OTA2_APP_PRINT(OTA2_LOG_INFO, ("OTA2_IMAGE_CURR_LUT_AREA_BASE        0x%08lx\n", (uint32_t)OTA2_IMAGE_CURR_LUT_AREA_BASE));
    OTA2_APP_PRINT(OTA2_LOG_INFO, ("OTA2_IMAGE_CURR_DCT_1_AREA_BASE      0x%08lx\n", (uint32_t)OTA2_IMAGE_CURR_DCT_1_AREA_BASE));
    OTA2_APP_PRINT(OTA2_LOG_INFO, ("OTA2_IMAGE_CURR_FS_AREA_BASE         0x%08lx\n", (uint32_t)OTA2_IMAGE_CURR_FS_AREA_BASE));
    OTA2_APP_PRINT(OTA2_LOG_INFO, ("OTA2_IMAGE_CURR_APP0_AREA_BASE       0x%08lx\n", (uint32_t)OTA2_IMAGE_CURR_APP0_AREA_BASE));
#if defined(OTA2_IMAGE_LAST_KNOWN_GOOD_AREA_BASE)
    OTA2_APP_PRINT(OTA2_LOG_INFO, ("OTA2_IMAGE_LAST_KNOWN_GOOD_AREA_BASE 0x%08lx\n", (uint32_t)OTA2_IMAGE_LAST_KNOWN_GOOD_AREA_BASE));
#endif
    OTA2_APP_PRINT(OTA2_LOG_INFO, ("OTA2_IMAGE_STAGING_AREA_BASE         0x%08lx\r\n", (uint32_t)OTA2_IMAGE_STAGING_AREA_BASE));

    /* read in our configurations */
    /* network */
    wiced_dct_read_lock( (void**)&player->dct_network, WICED_TRUE, DCT_NETWORK_CONFIG_SECTION, 0, sizeof(platform_dct_network_config_t) );

    /* wifi */
    wiced_dct_read_lock((void**)&player->dct_wifi, WICED_TRUE, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t));

    /* App */
    wiced_dct_read_lock( (void**)&player->dct_app, WICED_TRUE, DCT_APP_SECTION, 0, sizeof( ota2_dct_t ) );

    /* determine if this is a first boot, factory reset, or after an update boot */
    boot_type = wiced_ota2_get_boot_type();
    switch( boot_type )
    {
        case OTA2_BOOT_NEVER_RUN_BEFORE:
            OTA2_APP_PRINT(OTA2_LOG_INFO, ("First BOOT EVER\r\n"));
            /* Set the reboot type back to normal so we don't think we updated next reboot */
            wiced_dct_ota2_save_copy( OTA2_BOOT_NORMAL );
            break;
        case OTA2_BOOT_NORMAL:
            OTA2_APP_PRINT(OTA2_LOG_INFO, ("Normal reboot - count:%ld.\r\n", player->dct_app->reboot_count));
            break;
        case OTA2_BOOT_FACTORY_RESET:
            OTA2_APP_PRINT(OTA2_LOG_INFO, ("Factory Reset Occurred!\r\n"));
            over_the_air_2_app_restore_settings_ater_update(player);
            /* Set the reboot type back to normal so we don't think we updated next reboot */
            wiced_dct_ota2_save_copy( OTA2_BOOT_NORMAL );
            break;
        case OTA2_BOOT_UPDATE:
            OTA2_APP_PRINT(OTA2_LOG_INFO, ("Update Occurred!\r\n"));
            over_the_air_2_app_restore_settings_ater_update(player);
            /* Set the reboot type back to normal so we don't think we updated next reboot */
            wiced_dct_ota2_save_copy( OTA2_BOOT_NORMAL );
            break;
        case OTA2_BOOT_LAST_KNOWN_GOOD:
            OTA2_APP_PRINT(OTA2_LOG_INFO, ("Last Known Good used!\r\n"));
            break;
    }



    /* keep track of # of reboots */
    player->dct_app->reboot_count++;
    wiced_dct_write( (void*)player->dct_app, DCT_APP_SECTION, 0, sizeof( ota2_dct_t ) );

    player->log_level = OTA2_LOG_NOTIFY;

    /* print out our current configuration */
    ota2_config_print_info(player);

    /* Bring up the network interface */
    result = ota2_network_up_default(player, WICED_USE_EXTERNAL_DHCP_SERVER, NULL);
    if (result != WICED_SUCCESS)
    {
        /*
         * The network didn't initialize but we don't want to consider that a fatal error.
         * Make sure that autoplay is disabled to we don't try and use the network.
         */

        OTA2_APP_PRINT(OTA2_LOG_ERROR, ("Bringing up network interface failed!\r\n"));
    }
    else
    {
        /* create a socket */
        ota2_check_socket_created(player);
    }

    /* set our valid tag */
    player->tag = tag;

    return player;
}


wiced_result_t over_the_air_2_app_restore_settings_ater_update(ota2_data_t* player)
{
    platform_dct_network_config_t   dct_network = { 0 };
    platform_dct_wifi_config_t      dct_wifi = { 0 };
    ota2_dct_t                      dct_app = { 0 };

    /* read in our configurations from the DCT copy */
    /* network */
    if (wiced_dct_ota2_read_saved_copy( &dct_network, DCT_NETWORK_CONFIG_SECTION, 0, sizeof(platform_dct_network_config_t)) != WICED_SUCCESS)
    {
        OTA2_APP_PRINT(OTA2_LOG_WARNING, ("over_the_air_2_app_restore_settings_ater_update() failed reading Network Config!\r\n"));
        return WICED_ERROR;
    }

    /* wifi */
    if (wiced_dct_ota2_read_saved_copy( &dct_wifi, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t)) != WICED_SUCCESS)
    {
        OTA2_APP_PRINT(OTA2_LOG_WARNING, ("over_the_air_2_app_restore_settings_ater_update() failed reading WiFi Config!\r\n"));
        return WICED_ERROR;
    }

    /* App */
    if (wiced_dct_ota2_read_saved_copy( &dct_app, DCT_APP_SECTION, 0, sizeof(ota2_dct_t)) != WICED_SUCCESS)
    {
        OTA2_APP_PRINT(OTA2_LOG_WARNING, ("over_the_air_2_app_restore_settings_ater_update() failed reading App Config!\r\n"));
        return WICED_ERROR;
    }

    memcpy(player->dct_network, &dct_network, sizeof(platform_dct_network_config_t));
    memcpy(player->dct_wifi, &dct_wifi, sizeof(platform_dct_wifi_config_t));
    memcpy(player->dct_app, &dct_app, sizeof(ota2_dct_t));

    /* now, save them all! */
    if (ota2_save_config(player) != WICED_SUCCESS)
    {
        OTA2_APP_PRINT(OTA2_LOG_WARNING, ("over_the_air_2_app_restore_settings_ater_update() failed Saving Config!\r\n"));
        return WICED_ERROR;
    }

    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("Restored saved Configuration!\r\n"));
    return WICED_SUCCESS;
}
