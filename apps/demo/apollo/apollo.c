/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

/** @file Apollo audio application.
 *
 */

#include "wiced.h"
#include "platform_audio.h"
#include "command_console.h"
#include "console_wl.h"

#include "wifi/command_console_wifi.h"

#include "apollocore.h"
#include "apollo_log.h"
#include "apollo_config.h"
#include "apollo_cmd.h"
#include "apollo_cmd_sender.h"
#include "apollo_player.h"
#include "apollo_streamer.h"
#include "apollo_config_gatt_server.h"
#include "audio_display.h"
#include "button_manager.h"
#include "apollo_debug.h"
#include "apollo_wl_utils.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define APOLLO_CONSOLE_COMMANDS \
    { (char*) "exit",           apollo_console_command, 0, NULL, NULL, (char *)"", (char *)"Exit application" }, \
    { (char*) "start",          apollo_console_command, 0, NULL, NULL, (char *)"", (char *)"Start source/sink" }, \
    { (char*) "stop",           apollo_console_command, 0, NULL, NULL, (char *)"", (char *)"Stop source/sink" }, \
    { (char*) "volume",         apollo_console_command, 1, NULL, NULL, (char *)"", (char *)"Set the audio volume (0-100)" }, \
    { (char*) "config",         apollo_console_command, 0, NULL, NULL, (char *)"", (char *)"Display / change config values" }, \
    { (char*) "log",            apollo_console_command, 1, NULL, NULL, (char *)"", (char *)"Set the logging level" }, \
    { (char*) "ascu_time",      apollo_console_command, 0, NULL, NULL, (char *)"", (char *)"Display current time" }, \
    { (char*) "stats",          apollo_console_command, 0, NULL, NULL, (char *)"", (char *)"Display player stats" }, \
    { (char*) "netlog",         apollo_console_command, 1, NULL, NULL, (char *)"", (char *)"Turn network logging on|off" }, \
    { (char*) "rmc_status",     apollo_console_command, 0, NULL, NULL, (char *)"", (char *)"Display rmc_status info" }, \

/******************************************************
 *                    Constants
 ******************************************************/

#define APOLLO_TAG_VALID                      (0xCA11AB1E)
#define APOLLO_TAG_INVALID                    (0xDEADBEEF)

#define APOLLO_CONSOLE_COMMAND_MAX_LENGTH     (85)
#define APOLLO_CONSOLE_COMMAND_HISTORY_LENGTH (10)

#define APOLLO_TX_PACKET_POOL_COUNT           (256)
#define APOLLO_TX_PACKET_BUFFER_COUNT         (128)
#define APOLLO_TX_AUDIO_RENDER_BUFFER_NODES   (200)
#define APOLLO_RX_AUDIO_RENDER_BUFFER_NODES   (128)

#define BT_DEVICE_ADDRESS                     { 0x11, 0x22, 0x33, 0xAA, 0xBB, 0xCC }

#define BUTTON_WORKER_STACK_SIZE              ( 4096 )
#define BUTTON_WORKER_QUEUE_SIZE              ( 4 )

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
    APOLLO_CONSOLE_CMD_EXIT = 0,
    APOLLO_CONSOLE_CMD_START,
    APOLLO_CONSOLE_CMD_STOP,
    APOLLO_CONSOLE_CMD_VOLUME,
    APOLLO_CONSOLE_CMD_CONFIG,
    APOLLO_CONSOLE_CMD_LOG,
    APOLLO_CONSOLE_CMD_TIME,
    APOLLO_CONSOLE_CMD_STATS,
    APOLLO_CONSOLE_CMD_NETLOG,
    APOLLO_CONSOLE_CMD_RMC_STATUS,

    APOLLO_CONSOLE_CMD_MAX,
} APOLLO_CONSOLE_CMDS_T;

typedef enum {
    APOLLO_EVENT_SHUTDOWN           = (1 << 0),
    APOLLO_EVENT_START              = (1 << 1),
    APOLLO_EVENT_STOP               = (1 << 2),
    APOLLO_EVENT_AUTOSTOP           = (1 << 3),
    APOLLO_EVENT_CONFIG_GATT        = (1 << 4)
} APOLLO_EVENTS_T;

#define APOLLO_ALL_EVENTS       (-1)

typedef enum
{
    /* Launch BTLE GATT configuration process */
    ACTION_CONFIG_GATT_LAUNCH
} app_action_t;

typedef enum
{
    CONFIG_GATT_BUTTON
} application_button_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct
{
    char *cmd;
    uint32_t event;
} cmd_lookup_t;

typedef struct
{
    uint32_t                    tag;
    int                         stop_received;
    int                         auto_start;

    wiced_event_flags_t         events;

    apollo_dct_collection_t     dct_tables;
    apollo_streamer_params_t    streamer_params;

    wiced_ip_address_t          mrtp_ip_address;

    apollo_player_ref           player_handle;
    apollo_streamer_ref         streamer_handle;
    void*                       cmd_handle;
    void*                       cmd_sender_handle;

    button_manager_t            button_manager;
    wiced_worker_thread_t       button_worker_thread;
    wiced_semaphore_t           config_gatt_sem;
    wiced_bool_t                button_gatt_launch_was_pressed;

#ifdef USE_AUDIO_DISPLAY
    wiced_thread_t display_thread;
    apollo_player_stats_t player_stats;
    char display_name[32]; /* includes channel info */
    char display_info[32];
#endif
} apollo_app_t;

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

static int  apollo_console_command  (int argc, char *argv[]);
static void app_button_event_handler( const button_manager_button_t* button, button_manager_event_t event, button_manager_button_state_t state );

/******************************************************
 *               Variables Definitions
 ******************************************************/

static char apollo_command_buffer[APOLLO_CONSOLE_COMMAND_MAX_LENGTH];
static char apollo_command_history_buffer[APOLLO_CONSOLE_COMMAND_MAX_LENGTH * APOLLO_CONSOLE_COMMAND_HISTORY_LENGTH];

const command_t apollo_command_table[] =
{
    APOLLO_CONSOLE_COMMANDS
    WL_COMMANDS
    WIFI_COMMANDS
    CMD_TABLE_END
};

static cmd_lookup_t command_lookup[APOLLO_CONSOLE_CMD_MAX] =
{
    { "exit",           APOLLO_EVENT_SHUTDOWN   },
    { "start",          APOLLO_EVENT_START      },
    { "stop",           APOLLO_EVENT_STOP       },
    { "volume",         0                       },
    { "config",         0                       },
    { "log",            0                       },
    { "ascu_time",      0                       },
    { "stats",          0                       },
    { "netlog",         0                       },
    { "rmc_status",     0                       },
};

static const wiced_ip_setting_t ap_ip_settings =
{
    INITIALISER_IPV4_ADDRESS( .ip_address, MAKE_IPV4_ADDRESS( 192,168,  0,  1 ) ),
    INITIALISER_IPV4_ADDRESS( .netmask,    MAKE_IPV4_ADDRESS( 255,255,255,  0 ) ),
    INITIALISER_IPV4_ADDRESS( .gateway,    MAKE_IPV4_ADDRESS( 192,168,  0,  1 ) ),
};

static const wiced_button_manager_configuration_t button_manager_configuration =
{
    .short_hold_duration     = 500  * MILLISECONDS,
    .debounce_duration       = 150  * MILLISECONDS,

    .event_handler           = app_button_event_handler,
};

/* Static button configuration */
static const wiced_button_configuration_t button_configurations[] =
{
#if (WICED_PLATFORM_BUTTON_COUNT > 0)
    [ CONFIG_GATT_BUTTON ] = { PLATFORM_BUTTON_1, BUTTON_CLICK_EVENT , ACTION_CONFIG_GATT_LAUNCH },
#endif
};

/* Button objects for the button manager */
static button_manager_button_t buttons[] =
{
#if (WICED_PLATFORM_BUTTON_COUNT > 0)
    [ CONFIG_GATT_BUTTON ] = { &button_configurations[ CONFIG_GATT_BUTTON ] },
#endif
};

static apollo_app_t *g_apollo;

/******************************************************
 *               Function Definitions
 ******************************************************/

static void app_button_event_handler( const button_manager_button_t* button, button_manager_event_t event, button_manager_button_state_t state )
{
    if ( ( g_apollo != NULL ) && ( g_apollo->button_gatt_launch_was_pressed != WICED_TRUE ) &&
         ( button->configuration->application_event == ACTION_CONFIG_GATT_LAUNCH ) &&
         ( event == BUTTON_CLICK_EVENT ) )
    {
        g_apollo->button_gatt_launch_was_pressed = WICED_TRUE;
        wiced_rtos_set_event_flags(&g_apollo->events, APOLLO_EVENT_CONFIG_GATT);
        apollo_log_msg(APOLLO_LOG_ERR, "*** Received button press for GATT configuration request ! ***\r\n");
    }
    else if ( g_apollo == NULL )
    {
        apollo_log_msg(APOLLO_LOG_ERR, "*** App not fully initialized yet; please try again later. ***\r\n");
    }
    return;
}


static wiced_result_t apollo_button_handler_init(apollo_app_t* apollo)
{
    wiced_result_t result;

    result = wiced_rtos_create_worker_thread( &apollo->button_worker_thread, WICED_DEFAULT_WORKER_PRIORITY, BUTTON_WORKER_STACK_SIZE, BUTTON_WORKER_QUEUE_SIZE );
    wiced_action_jump_when_not_true( result == WICED_SUCCESS, _exit, apollo_log_msg(APOLLO_LOG_ERR, "wiced_rtos_create_worker_thread() failed !\r\n") );
    result = button_manager_init( &apollo->button_manager, &button_manager_configuration, &apollo->button_worker_thread, buttons, ARRAY_SIZE( buttons ) );
    wiced_action_jump_when_not_true( result == WICED_SUCCESS, _exit, apollo_log_msg(APOLLO_LOG_ERR, "button_manager_init() failed !\r\n") );

 _exit:
    return result;
}


static void apollo_button_handler_deinit(apollo_app_t* apollo)
{
    button_manager_deinit( &apollo->button_manager );
    wiced_rtos_delete_worker_thread( &apollo->button_worker_thread );
}


static int apollo_log_output_handler(APOLLO_LOG_LEVEL_T level, char *logmsg)
{
    WPRINT_APP_INFO(("%s", logmsg));

    return 0;
}


int apollo_console_command(int argc, char *argv[])
{
    apollo_player_stats_t stats;
    uint32_t master_secs;
    uint32_t master_nanosecs;
    uint32_t local_secs;
    uint32_t local_nanosecs;
    uint32_t event = 0;
    int volume;
    int log_level;
    int result;
    int i;

    apollo_log_msg(APOLLO_LOG_DEBUG0, "Apollo console received command: %s\r\n", argv[0]);

    if (g_apollo == NULL || g_apollo->tag != APOLLO_TAG_VALID)
    {
        apollo_log_msg(APOLLO_LOG_ERR, "Bad app structure\r\n");
        return ERR_CMD_OK;
    }

    /*
     * Lookup the command in our table.
     */

    for (i = 0; i < APOLLO_CONSOLE_CMD_MAX; ++i)
    {
        if (strcmp(command_lookup[i].cmd, argv[0]) == 0)
            break;
    }

    if (i >= APOLLO_CONSOLE_CMD_MAX)
    {
        apollo_log_msg(APOLLO_LOG_ERR, "Unrecognized command: %s\r\n", argv[0]);
        return ERR_CMD_OK;
    }

    switch (i)
    {
        case APOLLO_CONSOLE_CMD_EXIT:
        case APOLLO_CONSOLE_CMD_START:
        case APOLLO_CONSOLE_CMD_STOP:
            event = command_lookup[i].event;
            break;

        case APOLLO_CONSOLE_CMD_VOLUME:
            volume = atoi(argv[1]);
            if (g_apollo->player_handle)
            {
                apollo_player_set_volume(g_apollo->player_handle, volume);
            }
            else if (g_apollo->streamer_handle)
            {
                apollo_streamer_set_volume(g_apollo->streamer_handle, volume);

                /*
                 * Since we're the streamer, we want to tell all the sinks about the volume change.
                 */

                if (g_apollo->cmd_sender_handle != NULL)
                {
                    apollo_cmd_sender_command(g_apollo->cmd_sender_handle, APOLLO_CMD_SENDER_COMMAND_VOLUME, WICED_TRUE, NULL, (void*)volume);
                }
            }
            break;

        case APOLLO_CONSOLE_CMD_CONFIG:
            apollo_set_config(&g_apollo->dct_tables, argc, argv);
            break;

        case APOLLO_CONSOLE_CMD_LOG:
            log_level = atoi(argv[1]);
            apollo_log_printf("Setting new log level to %d (0 - off, %d - max debug)\r\n", log_level, APOLLO_LOG_DEBUG4);
            apollo_log_set_level(log_level);
            break;

        case APOLLO_CONSOLE_CMD_TIME:
            result = wiced_time_read_8021as(&master_secs, &master_nanosecs, &local_secs, &local_nanosecs);

            if (result == WICED_SUCCESS)
            {
                wiced_time_t wtime;

                wiced_time_get_time(&wtime);
                apollo_log_printf("Master time = %u.%09u secs\n", (unsigned int)master_secs, (unsigned int)master_nanosecs);
                apollo_log_printf("Current local time = %u.%09u secs\n", (unsigned int)local_secs, (unsigned int)local_nanosecs);
                apollo_log_printf("wtime is %d\n", (int)wtime);
            }
            else
            {
                apollo_log_printf("Error returned from wiced_time_read_8021as\n");
            }
            break;

        case APOLLO_CONSOLE_CMD_STATS:
            if (g_apollo->player_handle)
            {
                if (apollo_player_get_stats(g_apollo->player_handle, &stats) == WICED_SUCCESS)
                {
                    apollo_log_printf("Player stats: packets received %lu, packets dropped %lu\n", stats.rtp_packets_received, stats.rtp_packets_dropped);
                    apollo_log_printf("              total bytes received %lu, audio bytes received %lu\n", (uint32_t)stats.total_bytes_received, (uint32_t)stats.audio_bytes_received);
                    apollo_log_printf("              payload size %lu\n", stats.payload_size);
                }
            }
            break;

        case APOLLO_CONSOLE_CMD_NETLOG:
            if (!strcmp(argv[1], "on"))
            {
                if (apollo_debug_create_tcp_data_socket(NULL, 0) == WICED_SUCCESS)
                {
                    apollo_log_set_platform_output(apollo_debug_tcp_log_output_handler);
                    {
                        uint32_t master_secs2;
                        uint32_t master_nanosecs2;

                        wiced_time_read_8021as(&master_secs, &master_nanosecs, &local_secs, &local_nanosecs);
                        apollo_log_msg(APOLLO_LOG_ERR, "Network logging enabled\n");
                        wiced_time_read_8021as(&master_secs2, &master_nanosecs2, &local_secs, &local_nanosecs);
                        local_secs = master_secs2 - master_secs;
                        if (master_nanosecs2 < master_nanosecs)
                        {
                            local_secs--;
                            local_nanosecs = 1000000000 - master_nanosecs + master_nanosecs2;
                        }
                        else
                        {
                            local_nanosecs = master_nanosecs2 - master_nanosecs;
                        }
                        apollo_log_msg(APOLLO_LOG_ERR, "Log time delay is %lu.%09lu\n", local_secs, local_nanosecs);
                    }
                }
            }
            else
            {
                apollo_log_set_platform_output(apollo_log_output_handler);
                apollo_debug_close_tcp_data_socket();
            }
            break;

        case APOLLO_CONSOLE_CMD_RMC_STATUS:
        {
            wl_relmcast_status_t rmc_status;

            if (apollo_wl_get_rmc_status(&rmc_status) < 0)
            {
                apollo_log_msg(APOLLO_LOG_ERR, "Error returned from apollo_wl_get_rmc_status\r\n");
                break;
            }
            apollo_log_printf("%d peers associated\r\n", rmc_status.num);
            for (i = 0; i < rmc_status.num; i++)
            {
                apollo_log_printf("  %02x:%02x:%02x:%02x:%02x:%02x  %3d  %c %c %c\r\n",
                                  rmc_status.clients[i].addr.octet[0], rmc_status.clients[i].addr.octet[1],
                                  rmc_status.clients[i].addr.octet[2], rmc_status.clients[i].addr.octet[3],
                                  rmc_status.clients[i].addr.octet[4], rmc_status.clients[i].addr.octet[5],
                                  rmc_status.clients[i].rssi,
                                  rmc_status.clients[i].flag & WL_RMC_FLAG_ACTIVEACKER ? 'A' : ' ',
                                  rmc_status.clients[i].flag & WL_RMC_FLAG_RELMCAST    ? 'R' : ' ',
                                  rmc_status.clients[i].flag & WL_RMC_FLAG_INBLACKLIST ? 'B' : ' ');
            }
            break;
        }
    }

    if (event)
    {
        /*
         * Send off the event to the main loop.
         */

        wiced_rtos_set_event_flags(&g_apollo->events, event);
    }

    return ERR_CMD_OK;
}


static int apollo_player_event_callback(apollo_player_ref handle, void* userdata, APOLLO_PLAYER_EVENT_T event, void* arg)
{
    apollo_app_t* apollo = (apollo_app_t*)userdata;
    const platform_audio_device_info_t* audio_device;
    apollo_audio_format_t* format;
    apollo_seq_err_t* seq_err;
    apollo_player_stats_t* stats;

    if (apollo == NULL || apollo->tag != APOLLO_TAG_VALID)
    {
        return -1;
    }

    switch (event)
    {
        case APOLLO_PLAYER_EVENT_PLAYBACK_STARTED:
            format = (apollo_audio_format_t*)arg;
            audio_device = platform_audio_device_get_info_by_id(apollo->dct_tables.dct_app->audio_device_tx);
            apollo_log_msg(APOLLO_LOG_INFO, "Playback started using device %s\r\n", audio_device ? audio_device->device_name : "");
            apollo_log_msg(APOLLO_LOG_INFO, "Audio format is %u channels, %lu kHz, %u bps\r\n",
                           format->num_channels, format->sample_rate, format->bits_per_sample);
            break;

        case APOLLO_PLAYER_EVENT_PLAYBACK_STOPPED:
            stats = (apollo_player_stats_t*)arg;
            wiced_rtos_set_event_flags(&apollo->events, APOLLO_EVENT_AUTOSTOP);
            apollo_log_msg(APOLLO_LOG_NOTICE, "%lu bytes received in %lu packets\r\n", (uint32_t)stats->total_bytes_received, stats->rtp_packets_received);
            break;

        case APOLLO_PLAYER_EVENT_SEQ_ERROR:
            seq_err = (apollo_seq_err_t*)arg;
            apollo_log_msg(APOLLO_LOG_WARNING, "SEQ error - cur %u, last %u (lost %u)\r\n", seq_err->cur_seq,
                           seq_err->last_valid_seq, (seq_err->cur_seq - 1 - seq_err->last_valid_seq));

#ifdef USE_AUDIO_DISPLAY
            apollo_player_get_stats(apollo->player_handle, &apollo->player_stats);
            if (audio_display_get_footer_options() & FOOTER_OPTION_APOLLO_TX)
                audio_display_footer_update_time_info(0 /* pkts tx'd */, 0);
            else {
                audio_display_footer_update_time_info(apollo->player_stats.rtp_packets_received, apollo->player_stats.rtp_packets_dropped);
            }
#endif
            break;
    }

    return 0;
}


static wiced_result_t gatt_event_callback(apollo_config_gatt_event_t event, apollo_config_gatt_server_dct_t *dct,  void *user_context)
{
    wiced_result_t result = WICED_SUCCESS;
    apollo_app_t *apollo = (apollo_app_t *)user_context;

    switch ( event)
    {
        case APOLLO_CONFIG_GATT_EVENT_DCT_READ:
            dct->is_configured   = 1;
            dct->mode            = apollo->dct_tables.dct_app->apollo_role;
            dct->spk_channel_map = apollo->dct_tables.dct_app->speaker_channel;
            dct->spk_vol         = apollo->dct_tables.dct_app->volume;
            dct->src_type        = apollo->dct_tables.dct_app->source_type;
            dct->security        = apollo->dct_tables.dct_wifi->stored_ap_list[0].details.security;

            strncpy(dct->nw_ssid_name,
                    (char *)apollo->dct_tables.dct_wifi->stored_ap_list[0].details.SSID.value,
                    MIN(sizeof(dct->nw_ssid_name), apollo->dct_tables.dct_wifi->stored_ap_list[0].details.SSID.length));
            strncpy(dct->nw_pass_phrase,
                    (char *)apollo->dct_tables.dct_wifi->stored_ap_list[0].security_key,
                    MIN(sizeof(dct->nw_pass_phrase), apollo->dct_tables.dct_wifi->stored_ap_list[0].security_key_length));
            break;

        case APOLLO_CONFIG_GATT_EVENT_DCT_WRITE:
            apollo->dct_tables.dct_app->is_configured                       = dct->is_configured;
            apollo->dct_tables.dct_app->apollo_role                         = dct->mode;
            apollo->dct_tables.dct_app->speaker_channel                     = dct->spk_channel_map;
            apollo->dct_tables.dct_app->volume                              = dct->spk_vol;
            apollo->dct_tables.dct_app->source_type                         = dct->src_type;
            apollo->dct_tables.dct_wifi->stored_ap_list[0].details.security = dct->security;

            apollo->dct_tables.dct_wifi->stored_ap_list[0].details.SSID.length = strlen(dct->nw_ssid_name);
            strncpy((char *)apollo->dct_tables.dct_wifi->stored_ap_list[0].details.SSID.value,
                    dct->nw_ssid_name,
                    apollo->dct_tables.dct_wifi->stored_ap_list[0].details.SSID.length);
            apollo->dct_tables.dct_wifi->stored_ap_list[0].security_key_length = strlen(dct->nw_pass_phrase);
            strncpy((char *)apollo->dct_tables.dct_wifi->stored_ap_list[0].security_key,
                    dct->nw_pass_phrase,
                    apollo->dct_tables.dct_wifi->stored_ap_list[0].security_key_length);
            break;

        case APOLLO_CONFIG_GATT_EVENT_DCT_WRITE_COMPLETED:
            apollo_config_save(&apollo->dct_tables);
            apollo_config_deinit(&apollo->dct_tables);
            apollo_config_gatt_server_stop();

            /*
             * Reboot after GATT configuration is over
             */

            wiced_framework_reboot();
            break;

        default:
            result = WICED_ERROR;
            break;
    }

    return result;
}


static void apollo_config_gatt_launch(apollo_app_t* apollo)
{
#ifdef USE_ALTERNATE_GATT_SCRIPT
    if ( (apollo->dct_tables.dct_app->apollo_role != APOLLO_ROLE_SOURCE) || (apollo->dct_tables.dct_app->source_type != APOLLO_AUDIO_SOURCE_BT) )
#else
        if ( (apollo->dct_tables.dct_app->is_configured == 0))
#endif
        {
            apollo_config_gatt_server_params_t gatt_server_params =
            {
                    .user_context      = apollo,
                    .event_cbf         = gatt_event_callback,
                    .init_bt_stack     = WICED_TRUE,
                    .bt_device_address = BT_DEVICE_ADDRESS,
            };

#ifdef WICED_DCT_INCLUDE_BT_CONFIG
            memcpy( gatt_server_params.bt_device_address, apollo->dct_tables.dct_bt->bluetooth_device_address,
                    MIN(sizeof(gatt_server_params.bt_device_address), sizeof(apollo->dct_tables.dct_bt->bluetooth_device_address)) );
#endif
            apollo_config_gatt_server_start( &gatt_server_params );

            /*
             * Setting global apollo app pointer; allowing us to use commmand console to force 'is_configured' status if needed
             */

            apollo->tag = APOLLO_TAG_VALID;
            g_apollo = apollo;

            /*
             * Wait forever or until GATT config is over; otherwise, BT stack may be initialized a 2nd time when device is in SOURCE role;
             * leading to a crash...
             */
            apollo_log_msg(APOLLO_LOG_ERR, "*** Waiting for GATT configuration... ***\r\n");
            wiced_rtos_get_semaphore(&apollo->config_gatt_sem, WICED_WAIT_FOREVER);
        }
}


static void apollo_config_gatt_launch_from_bt_a2dp_source(apollo_app_t* apollo)
{
#ifdef USE_ALTERNATE_GATT_SCRIPT
            apollo_config_gatt_server_params_t gatt_server_params =
            {
                    .user_context      = apollo,
                    .event_cbf         = gatt_event_callback,
                    .init_bt_stack     = WICED_FALSE,
                    .bt_device_address = {0,},
            };
            apollo_config_gatt_server_start( &gatt_server_params );
            apollo_log_msg(APOLLO_LOG_ERR, "*** Waiting for GATT configuration... ***\r\n");
#else
            UNUSED_PARAMETER(apollo);
#endif
}


static int apollo_streamer_event_notification(apollo_streamer_ref handle, void* user_context, apollo_streamer_event_t event, void* arg)
{
    int rc = 0;
    apollo_app_t* apollo = (apollo_app_t*)user_context;
    wiced_action_jump_when_not_true((apollo != NULL) && (apollo->tag == APOLLO_TAG_VALID), _exit, rc = -1);

    switch (event)
    {
        case APOLLO_STREAMER_EVENT_BT_ENABLED:
#ifdef USE_AUDIO_DISPLAY
            audio_display_header_update_options(BATTERY_ICON_IS_VISIBLE | BATTERY_ICON_SHOW_PERCENT | SIGNAL_STRENGTH_IS_VISIBLE | BLUETOOTH_IS_CONNECTED | INVERT_BLUETOOTH_ICON_COLORS);
#endif
            apollo_log_msg(APOLLO_LOG_ERR, "BT device is enabled.\r\n");
            break;

        case APOLLO_STREAMER_EVENT_BT_CONNECTED:
#ifdef USE_AUDIO_DISPLAY
            audio_display_header_update_options(BATTERY_ICON_IS_VISIBLE | BATTERY_ICON_SHOW_PERCENT | SIGNAL_STRENGTH_IS_VISIBLE | BLUETOOTH_IS_CONNECTED);
#endif
            apollo_log_msg(APOLLO_LOG_ERR, "BT device is connected.\r\n");
            apollo_config_gatt_launch_from_bt_a2dp_source(apollo);
            break;

        case APOLLO_STREAMER_EVENT_BT_DISCONNECTED:
#ifdef USE_AUDIO_DISPLAY
            audio_display_header_update_options(BATTERY_ICON_IS_VISIBLE | BATTERY_ICON_SHOW_PERCENT | SIGNAL_STRENGTH_IS_VISIBLE);
#endif
            apollo_log_msg(APOLLO_LOG_ERR, "BT device is disconnected.\r\n");
            break;

        case APOLLO_STREAMER_EVENT_BT_PLAYBACK_STARTED:
            apollo_log_msg(APOLLO_LOG_INFO, "BT playback has started.\r\n");
            break;

        case APOLLO_STREAMER_EVENT_BT_PLAYBACK_STOPPED:
            apollo_log_msg(APOLLO_LOG_INFO, "BT playback has stopped.\r\n");
            break;

        default:
            break;
    }

 _exit:
    return rc;
}


static wiced_result_t apollo_stream_start(apollo_app_t* apollo)
{
    wiced_result_t result;

    /*
     * Set streaming options (most from DCT)
     */

    memset(&apollo->streamer_params, 0, sizeof(apollo->streamer_params));

    apollo->streamer_params.user_context        = apollo;
    apollo->streamer_params.event_cb            = apollo_streamer_event_notification;
    apollo->streamer_params.source_type         = (apollo_audio_source_type_t)apollo->dct_tables.dct_app->source_type;
    apollo->streamer_params.iface               = apollo->dct_tables.dct_network->interface;
    apollo->streamer_params.port                = 0;
    apollo->streamer_params.num_pool_packets    = APOLLO_TX_PACKET_POOL_COUNT;
    apollo->streamer_params.num_packets         = APOLLO_TX_PACKET_BUFFER_COUNT;
    apollo->streamer_params.payload_size        = apollo->dct_tables.dct_app->payload_size;
    apollo->streamer_params.fec_order           = apollo->dct_tables.dct_app->fec_order;
    apollo->streamer_params.fec_length          = apollo->dct_tables.dct_app->fec_length;
    apollo->streamer_params.audio_device_rx     = apollo->dct_tables.dct_app->audio_device_rx;
    apollo->streamer_params.audio_device_tx     = apollo->dct_tables.dct_app->audio_device_tx;
    apollo->streamer_params.input_sample_rate   = apollo->dct_tables.dct_app->input_sample_rate;
    apollo->streamer_params.input_sample_size   = apollo->dct_tables.dct_app->input_sample_size;
    apollo->streamer_params.input_channel_count = apollo->dct_tables.dct_app->input_channel_count;

    SET_IPV4_ADDRESS(apollo->streamer_params.clientaddr, GET_IPV4_ADDRESS(apollo->dct_tables.dct_app->clientaddr));

    apollo->streamer_params.volume              = apollo->dct_tables.dct_app->volume;
    apollo->streamer_params.buffer_nodes        = APOLLO_TX_AUDIO_RENDER_BUFFER_NODES;
    apollo->streamer_params.buffer_ms           = apollo->dct_tables.dct_app->buffering_ms;
    apollo->streamer_params.threshold_ms        = apollo->dct_tables.dct_app->threshold_ms;
    apollo->streamer_params.clock_enable        = apollo->dct_tables.dct_app->clock_enable;
    apollo->streamer_params.pll_tuning_enable   = apollo->dct_tables.dct_app->pll_tuning_enable;

    /*
     * Provide offset at the END of APP section; otherwise, BT stack will overwrite valid/current APP section !
     */

    apollo->streamer_params.app_dct_offset_for_bt = sizeof(apollo_dct_t);

    /*
     * Fire off the apollo streamer.
     */

    result = apollo_streamer_init(&apollo->streamer_params, &apollo->streamer_handle);
    if (result != WICED_SUCCESS)
    {
        apollo_log_msg(APOLLO_LOG_ERR, "Unable to initialize apollo_streamer\r\n");
    }

    return result;
}


static void apollo_stream_stop(apollo_app_t* apollo)
{
    if (apollo == NULL || apollo->tag != APOLLO_TAG_VALID || apollo->streamer_handle == NULL)
    {
        return;
    }

    apollo_streamer_deinit(apollo->streamer_handle);
    apollo->streamer_handle = NULL;
}


static wiced_result_t apollo_play_start(apollo_app_t* apollo)
{
    apollo_player_params_t params;

    params.event_cb          = apollo_player_event_callback;
    params.userdata          = apollo;
    params.interface         = apollo->dct_tables.dct_network->interface;
    params.rtp_port          = apollo->dct_tables.dct_app->rtp_port;
    params.channel           = apollo->dct_tables.dct_app->speaker_channel;
    params.volume            = apollo->dct_tables.dct_app->volume;
    params.device_id         = apollo->dct_tables.dct_app->audio_device_tx;
    params.buffer_nodes      = APOLLO_RX_AUDIO_RENDER_BUFFER_NODES;
    params.buffer_ms         = apollo->dct_tables.dct_app->buffering_ms;
    params.threshold_ms      = apollo->dct_tables.dct_app->threshold_ms;
    params.clock_enable      = apollo->dct_tables.dct_app->clock_enable;
    params.pll_tuning_enable = apollo->dct_tables.dct_app->pll_tuning_enable;

    /*
     * Fire off the apollo player.
     */

    apollo->player_handle = apollo_player_init(&params);
    if (apollo->player_handle == NULL)
    {
        apollo_log_msg(APOLLO_LOG_ERR, "Unable to initialize apollo_player\r\n");
        return WICED_ERROR;
    }

    return WICED_SUCCESS;
}


static void apollo_play_stop(apollo_app_t* apollo)
{
    if (apollo == NULL || apollo->tag != APOLLO_TAG_VALID || apollo->player_handle == NULL)
    {
        return;
    }

    apollo_player_deinit(apollo->player_handle);
    apollo->player_handle = NULL;
}


static void apollo_service_start(apollo_app_t* apollo)
{
    wiced_result_t result;

    if (apollo->dct_tables.dct_app->apollo_role == APOLLO_ROLE_SINK)
    {
        if (apollo->player_handle == NULL)
        {
            result = apollo_play_start(apollo);
            if (result != WICED_SUCCESS)
            {
                apollo_log_msg(APOLLO_LOG_ERR, "Error creating apollo player service\r\n");
            }
        }
        else
        {
            apollo_log_msg(APOLLO_LOG_ERR, "Apollo player service already running\r\n");
        }
    }
    else
    {
        if (apollo->streamer_handle == NULL)
        {
            result = apollo_stream_start(apollo);
            if (result != WICED_SUCCESS)
            {
                apollo_log_msg(APOLLO_LOG_ERR, "Error creating apollo streamer service\r\n");
            }
        }
        else
        {
            apollo_log_msg(APOLLO_LOG_ERR, "Apollo streamer service already running\r\n");
        }
    }
}


static void apollo_service_stop(apollo_app_t* apollo)
{
    if (apollo->dct_tables.dct_app->apollo_role == APOLLO_ROLE_SINK)
    {
        apollo_play_stop(apollo);
    }
    else
    {
        apollo_stream_stop(apollo);
    }
}


static void apollo_mainloop(apollo_app_t* apollo)
{
    wiced_result_t result;
    uint32_t events;

    apollo_log_msg(APOLLO_LOG_INFO, "Begin apollo mainloop\r\n");

    /*
     * If auto start is set then start off by sending ourselves a start event.
     */

    if (apollo->auto_start != WICED_FALSE)
    {
        wiced_rtos_set_event_flags(&apollo->events, APOLLO_EVENT_START);
    }

    while (apollo->tag == APOLLO_TAG_VALID)
    {
        events = 0;
        result = wiced_rtos_wait_for_event_flags(&apollo->events, APOLLO_ALL_EVENTS, &events,
                                                 WICED_TRUE, WAIT_FOR_ANY_EVENT, WICED_WAIT_FOREVER);
        if (result != WICED_SUCCESS)
        {
            continue;
        }

        if (events & APOLLO_EVENT_SHUTDOWN)
        {
            break;
        }

        if (events & APOLLO_EVENT_START)
        {
            apollo->stop_received = 0;
            apollo_service_start(apollo);
        }

        if (events & (APOLLO_EVENT_STOP | APOLLO_EVENT_AUTOSTOP))
        {
            apollo_service_stop(apollo);

            if (events & APOLLO_EVENT_STOP)
            {
                apollo->stop_received = 1;
            }
            else if (!apollo->stop_received)
            {
                /*
                 * Start playing again automatically.
                 */

                wiced_rtos_set_event_flags(&apollo->events, APOLLO_EVENT_START);
            }
        }

        if (events & APOLLO_EVENT_CONFIG_GATT)
        {
            apollo_service_stop(apollo);
            apollo->dct_tables.dct_app->is_configured = 0;
            apollo_config_save(&apollo->dct_tables);
            apollo_config_deinit(&apollo->dct_tables);
            wiced_framework_reboot();
        }
    };

    /*
     * Make sure that playback has been shut down.
     */

    apollo_service_stop(apollo);

    apollo_log_msg(APOLLO_LOG_INFO, "End apollo mainloop\r\n");
}


static wiced_result_t apollo_cmd_sender_callback(void* handle, void* userdata, APOLLO_CMD_SENDER_EVENT_T event, void* arg)
{
    apollo_app_t* apollo = (apollo_app_t*)userdata;
    apollo_peers_t* peers;
    int i;

    if (apollo == NULL || apollo->tag != APOLLO_TAG_VALID)
    {
        /*
         * Bad player handle so just return.
         */

        return WICED_SUCCESS;
    }

    if (event == APOLLO_CMD_SENDER_EVENT_DISCOVER_RESULTS)
    {
        peers = (apollo_peers_t*)arg;
        apollo_log_msg(APOLLO_LOG_INFO, "Found %d speakers on our network\r\n", peers->num_speakers);
        for (i = 0; i < peers->num_speakers; i++)
        {
            apollo_log_msg(APOLLO_LOG_INFO, "  Speaker name: %s, channel 0x%08lx, MAC %02x:%02x:%02x:%02x:%02x:%02x, IP %ld.%ld.%ld.%ld\r\n",
                           peers->speakers[i].config.speaker_name, peers->speakers[i].config.speaker_channel,
                           peers->speakers[i].mac.octet[0], peers->speakers[i].mac.octet[1], peers->speakers[i].mac.octet[2],
                           peers->speakers[i].mac.octet[3], peers->speakers[i].mac.octet[4], peers->speakers[i].mac.octet[5],
                           (peers->speakers[i].ipaddr >> 24) & 0xFF, (peers->speakers[i].ipaddr >> 16) & 0xFF,
                           (peers->speakers[i].ipaddr >>  8) & 0xFF, peers->speakers[i].ipaddr & 0xFF);
            }
    }
    else if (event == APOLLO_CMD_SENDER_EVENT_COMMAND_STATUS)
    {
        apollo_log_msg(APOLLO_LOG_INFO, "%d peers replied\r\n", (int)arg);
    }

    return WICED_SUCCESS;
}


static wiced_result_t apollo_cmd_callback(void* handle, void* userdata, APOLLO_CMD_EVENT_T event, void* arg)
{
    apollo_app_t* apollo = (apollo_app_t*)userdata;
    apollo_cmd_speaker_t* cmd_speaker;
    int volume;
    int len;

    if (apollo == NULL || apollo->tag != APOLLO_TAG_VALID)
    {
        /*
         * Bad player handle so just return.
         */

        return WICED_SUCCESS;
    }

    switch (event)
    {
        case APOLLO_CMD_EVENT_QUERY_SPEAKER:
            cmd_speaker = (apollo_cmd_speaker_t*)arg;
            if (cmd_speaker)
            {
                cmd_speaker->speaker_name     = apollo->dct_tables.dct_app->speaker_name;
                cmd_speaker->speaker_name_len = strlen(apollo->dct_tables.dct_app->speaker_name);
                cmd_speaker->speaker_channel  = apollo->dct_tables.dct_app->speaker_channel;
            }
            break;

        case APOLLO_CMD_EVENT_SET_SPEAKER:
            cmd_speaker = (apollo_cmd_speaker_t*)arg;
            if (cmd_speaker == NULL)
            {
                break;
            }

            len = cmd_speaker->speaker_name_len;
            if (len > sizeof(apollo->dct_tables.dct_app->speaker_name) - 1)
            {
                len = sizeof(apollo->dct_tables.dct_app->speaker_name) - 1;
            }

            /*
             * If the speaker channel or name has changed than update our configuration.
             */

            if (cmd_speaker->speaker_channel != apollo->dct_tables.dct_app->speaker_channel ||
                strncmp(cmd_speaker->speaker_name, apollo->dct_tables.dct_app->speaker_name, len))
            {
                apollo->dct_tables.dct_app->speaker_channel = cmd_speaker->speaker_channel;
                strncpy(apollo->dct_tables.dct_app->speaker_name, cmd_speaker->speaker_name, len);
                apollo->dct_tables.dct_app->speaker_name[len] = '\0';
                wiced_dct_write((void*)apollo->dct_tables.dct_app, DCT_APP_SECTION, 0, sizeof(apollo_dct_t));
            }
            break;

        case APOLLO_CMD_EVENT_SET_VOLUME:
            volume = (int)arg;
            if (volume < APOLLO_VOLUME_MIN || volume > APOLLO_VOLUME_MAX)
            {
                volume = apollo->dct_tables.dct_app->volume;
            }

            /*
             * Do we need to adjust the volume?
             */

            if (volume != apollo->dct_tables.dct_app->volume)
            {
                /*
                 * If we're currently playing audio then adjust the volume.
                 */

                if (apollo->player_handle)
                {
                    apollo_player_set_volume(apollo->player_handle, volume);
                }

                /*
                 * And save the new volume information.
                 */

                apollo->dct_tables.dct_app->volume = volume;
                wiced_dct_write((void*)apollo->dct_tables.dct_app, DCT_APP_SECTION, 0, sizeof(apollo_dct_t));
            }
            break;

        default:
            apollo_log_msg(APOLLO_LOG_ERR, "Unknown command event: %d\r\n", event);
            break;
    }

    return WICED_SUCCESS;
}


static void apollo_shutdown(apollo_app_t* apollo)
{
    /*
     * Mark the app structure as invalid since we are shutting down.
     */

    apollo->tag = APOLLO_TAG_INVALID;

    /*
     * Shutdown the console.
     */

    command_console_deinit();

    /*
     * Shutdown button handler
     */

    apollo_button_handler_deinit(apollo);

    if (apollo->cmd_handle != NULL)
    {
        apollo_cmd_deinit(apollo->cmd_handle);
        apollo->cmd_handle = NULL;
    }

    if (apollo->cmd_sender_handle != NULL)
    {
        apollo_cmd_sender_deinit(apollo->cmd_sender_handle);
        apollo->cmd_sender_handle = NULL;
    }

    wiced_rtos_deinit_semaphore(&apollo->config_gatt_sem);
    wiced_rtos_deinit_event_flags(&apollo->events);

    free(apollo);
}


static apollo_app_t* apollo_init(void)
{
    apollo_app_t* apollo;
    wiced_result_t result;
    uint32_t tag = APOLLO_TAG_VALID;
    wiced_interface_t interface;
    wiced_mac_t mac_address;

    /*
     * Initialize the logging subsystem.
     */

    result = apollo_log_init(APOLLO_LOG_ERR, apollo_log_output_handler);
    if (result != WICED_SUCCESS)
    {
        WPRINT_APP_INFO(("apollo_log_init() failed !\r\n"));
    }

    /*
     * Temporary - set the device MAC address in the NVRAM.
     */

    apollo_set_nvram_mac();

    /* Initialize the device */
    result = wiced_init();
    if (result != WICED_SUCCESS)
    {
        return NULL;
    }

    /*
     * Allocate the main application structure.
     */

    apollo = calloc_named("apollo", 1, sizeof(apollo_app_t));
    if (apollo == NULL)
    {
        apollo_log_msg(APOLLO_LOG_ERR, "Unable to allocate apollo structure\r\n");
        return NULL;
    }

    /*
     * Create the command console.
     */

    apollo_log_msg(APOLLO_LOG_INFO, "Start the command console\r\n");
    result = command_console_init(STDIO_UART, sizeof(apollo_command_buffer), apollo_command_buffer, APOLLO_CONSOLE_COMMAND_HISTORY_LENGTH, apollo_command_history_buffer, " ");
    if (result != WICED_SUCCESS)
    {
        apollo_log_msg(APOLLO_LOG_ERR, "Error starting the command console\r\n");
        free(apollo);
        return NULL;
    }
    console_add_cmd_table(apollo_command_table);

    /*
     * Create our event flags.
     */

    result = wiced_rtos_init_event_flags(&apollo->events);
    if (result != WICED_SUCCESS)
    {
        apollo_log_msg(APOLLO_LOG_ERR, "Error initializing event flags\r\n");
        tag = APOLLO_TAG_INVALID;
    }

    result = wiced_rtos_init_semaphore(&apollo->config_gatt_sem);
    if (result != WICED_SUCCESS)
    {
        apollo_log_msg(APOLLO_LOG_ERR, "Error initializing config GATT semaphore\r\n");
        tag = APOLLO_TAG_INVALID;
    }

    /*
     * Get DCT
     */

    result = apollo_config_init(&apollo->dct_tables);
    if (result != WICED_SUCCESS)
    {
        apollo_log_msg(APOLLO_LOG_ERR, "apollo_config_init() failed !\r\n");
        apollo_config_deinit(&apollo->dct_tables);
        tag = APOLLO_TAG_INVALID;
    }
    else
    {
        /*
         * Launch GATT config if needed
         */

        apollo_config_gatt_launch(apollo);
    }
    apollo->auto_start = apollo->dct_tables.dct_app->auto_start;

    /*
     * Init button handler
     */

    apollo_button_handler_init(apollo);


    /*
     * Set the current logging level.
     */

    apollo_log_set_level(apollo->dct_tables.dct_app->log_level);

    /*
     * Set our hostname to the speaker name - this way DHCP servers will see which unique speaker acquired a lease
     * once the network init runs ...
     */

    result = wiced_network_set_hostname(apollo->dct_tables.dct_app->speaker_name);
    if (result != WICED_SUCCESS)
    {
        apollo_log_msg(APOLLO_LOG_ERR, "Can't set hostname for dhcp_client_init!\r\n");
    }

    /*
     * Make sure that 802.1AS time is enabled.
     */

    wiced_time_enable_8021as();

    /*
     * Initialize nanosecond clock for later use
     */

    wiced_init_nanosecond_clock();

    /* Initialize platform audio */
    platform_init_audio();

    /* print out our current configuration */
    apollo_config_print_info(&apollo->dct_tables);

    /* Bring up the network interface */
    if (apollo->dct_tables.dct_app->apollo_role == APOLLO_ROLE_SOURCE)
    {
        result = apollo_network_up_default(&interface, WICED_USE_INTERNAL_DHCP_SERVER, &ap_ip_settings);
    }
    else
    {
        result = apollo_network_up_default(&interface, WICED_USE_EXTERNAL_DHCP_SERVER, NULL);
    }

    if (result == WICED_SUCCESS)
    {
        SET_IPV4_ADDRESS(apollo->mrtp_ip_address, APOLLO_MULTICAST_IPV4_ADDRESS_DEFAULT);
        apollo_log_msg(APOLLO_LOG_INFO, "Joining multicast group %d.%d.%d.%d\n",
                (int)((apollo->mrtp_ip_address.ip.v4 >> 24) & 0xFF), (int)((apollo->mrtp_ip_address.ip.v4 >> 16) & 0xFF),
                (int)((apollo->mrtp_ip_address.ip.v4 >> 8) & 0xFF),  (int)(apollo->mrtp_ip_address.ip.v4 & 0xFF));
        result = wiced_multicast_join(apollo->dct_tables.dct_network->interface, &apollo->mrtp_ip_address);
    }

    if (result != WICED_SUCCESS)
    {
        /*
         * The network didn't initialize but we don't want to consider that a fatal error.
         * Make sure that auto start is disabled to we don't try and use the network.
         */

        apollo_log_msg(APOLLO_LOG_ERR, "Bringing up network interface failed!\r\n");
        apollo->auto_start = WICED_FALSE;
    }

    /*
     * Create the Apollo command instance.
     */

    wwd_wifi_get_mac_address(&mac_address, apollo->dct_tables.dct_network->interface);
    apollo->cmd_handle = apollo_cmd_init(apollo->dct_tables.dct_network->interface, &mac_address, apollo, apollo_cmd_callback);
    if (apollo->cmd_handle == NULL)
    {
        apollo_log_msg(APOLLO_LOG_ERR, "Unable to create Apollo command instance\r\n");
    }

    /*
     * And the command sender instance.
     */

    apollo->cmd_sender_handle = apollo_cmd_sender_init(apollo->dct_tables.dct_network->interface, apollo, apollo_cmd_sender_callback);

    /*
     * Set the application tag to the correct state.
     */

    apollo->tag = tag;

#ifdef USE_AUDIO_DISPLAY
    {
        uint32_t wifi_channel;
        apollo_dct_collection_t* dct_tables = &apollo->dct_tables;

        wiced_wifi_get_channel(&wifi_channel);
        audio_display_create_management_thread(&apollo->display_thread);

        memset(apollo->display_name, 0, sizeof(apollo->display_name));
        memset(apollo->display_info, 0, sizeof(apollo->display_info));

        memcpy(apollo->display_name, dct_tables->dct_wifi->stored_ap_list[0].details.SSID.value, dct_tables->dct_wifi->stored_ap_list[0].details.SSID.length);
        sprintf(apollo->display_info, "%s - %d/%d %s", (dct_tables->dct_app->apollo_role != APOLLO_ROLE_SOURCE ? "Sink" : "Source"),
                (int)wifi_channel, dct_tables->dct_wifi->stored_ap_list[0].details.channel,
                ((dct_tables->dct_wifi->stored_ap_list[0].details.band == WICED_802_11_BAND_2_4GHZ) ? "2.4GHz" : "5GHz"));

        audio_display_header_update_options(BATTERY_ICON_IS_VISIBLE |BATTERY_ICON_SHOW_PERCENT |SIGNAL_STRENGTH_IS_VISIBLE);
        audio_display_footer_update_song_info(apollo->display_name, apollo->display_info);
        audio_display_footer_update_options(FOOTER_IS_VISIBLE | FOOTER_CENTER_ALIGN | ((dct_tables->dct_app->apollo_role == APOLLO_ROLE_SOURCE) ? FOOTER_OPTION_APOLLO_TX : FOOTER_OPTION_APOLLO_RX));

    }
#endif

    {
        uint32_t *chipregs = (uint32_t *)PLATFORM_CHIPCOMMON_REGBASE(0x0);
        uint8_t version;

        version = (*chipregs >> 16) & 0xF;
        apollo_log_msg(APOLLO_LOG_ERR, "**********Chip version is %s\n",
                       version == 1 ? "B0" : (version == 2 ? "B1" : (version == 3 ? "B2" : "Unknown")));
    }

    return apollo;
}


void application_start(void)
{
    apollo_app_t* apollo;

    /*
     * Main initialization.
     */

    if ((apollo = apollo_init()) == NULL)
    {
        return;
    }

    if (apollo->tag != APOLLO_TAG_VALID)
    {
        apollo_shutdown(apollo);
        return;
    }
    g_apollo = apollo;

    /*
     * Drop into our main loop.
     */

    apollo_mainloop(apollo);

    /*
     * Cleanup and exit.
     */

    g_apollo = NULL;
    apollo_shutdown(apollo);
}
