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
 */

#include "wiced.h"
#include "platform_audio.h"
#include "command_console.h"

#include "ota2_test.h"
#include "ota2_test_config.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/


#define WIFI_DCT_SECURITY_KEY_DIRTY     (1 << 0)
#define WIFI_DCT_SECURITY_TYPE_DIRTY    (1 << 1)
#define WIFI_DCT_CHANNEL_DIRTY          (1 << 2)
#define WIFI_DCT_SSID_DIRTY             (1 << 3)
#define WIFI_DCT_MAC_ADDR_DIRTY         (1 << 4)

#define MAC_STR_LEN                     (18)

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum {
    CONFIG_CMD_NONE         = 0,
    CONFIG_CMD_HELP,
    CONFIG_CMD_MAC_ADDR,
    CONFIG_CMD_NETWORK_CHANNEL,
    CONFIG_CMD_NETWORK_NAME,
    CONFIG_CMD_NETWORK_PASSPHRASE,
    CONFIG_CMD_NETWORK_SECURITY_TYPE,


    CONFIG_CMD_SAVE,

    CONFIG_CMD_MAX,
} CONFIG_CMDS_T;

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct cmd_lookup_s {
        char *cmd;
        uint32_t event;
} cmd_lookup_t;

typedef struct security_lookup_s {
    char     *name;
    wiced_security_t sec_type;
} security_lookup_t;

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/
static wiced_result_t ota2_save_app_dct(ota2_data_t* player);
static wiced_result_t ota2_save_network_dct(ota2_data_t* player);
static wiced_result_t ota2_save_wifi_dct(ota2_data_t* player);
static void ota_test_print_app_info(ota2_data_t* player);
static void ota_test_print_network_info(ota2_data_t* player);
static void ota_test_print_wifi_info(ota2_data_t* player);
static wiced_bool_t ota2_get_channel_band(int channel, int* band);

/******************************************************
 *               Variables Definitions
 ******************************************************/

/* dirty flags for determining what to save */
static uint32_t app_dct_dirty = 0;
static uint32_t net_dct_dirty = 0;
static uint32_t wifi_dct_dirty = 0;


static cmd_lookup_t config_command_lookup[] = {
        { "help",               CONFIG_CMD_HELP                 },
        { "?",                  CONFIG_CMD_HELP                 },

        { "mac_addr",           CONFIG_CMD_MAC_ADDR             },
        { "mac",                CONFIG_CMD_MAC_ADDR             },

        { "mac_addr",           CONFIG_CMD_MAC_ADDR             },
        { "mac",                CONFIG_CMD_MAC_ADDR             },

        { "network_channel",    CONFIG_CMD_NETWORK_CHANNEL      },
        { "network_chan",       CONFIG_CMD_NETWORK_CHANNEL      },
        { "network_name",       CONFIG_CMD_NETWORK_NAME         },
        { "network_passphrase", CONFIG_CMD_NETWORK_PASSPHRASE   },
        { "network_pass",       CONFIG_CMD_NETWORK_PASSPHRASE   },
        { "network_security",   CONFIG_CMD_NETWORK_SECURITY_TYPE},
        { "network_sec",        CONFIG_CMD_NETWORK_SECURITY_TYPE},

        { "net_chan",           CONFIG_CMD_NETWORK_CHANNEL      },
        { "net_name",           CONFIG_CMD_NETWORK_NAME         },
        { "net_pass",           CONFIG_CMD_NETWORK_PASSPHRASE   },
        { "net_sec",            CONFIG_CMD_NETWORK_SECURITY_TYPE},

        { "ssid",               CONFIG_CMD_NETWORK_NAME         },
        { "pass",               CONFIG_CMD_NETWORK_PASSPHRASE   },

        { "save",               CONFIG_CMD_SAVE                 },

        { "", CONFIG_CMD_NONE },
};

static void ota2_config_command_help( void )
{
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("Config commands:\r\n"));
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("    config                              : output current config\r\n"));
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("    config <?|help>                     : show this list\r\n"));
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("    config mac_addr <xx:xx:xx:xx:xx:xx> : xx:xx:xx:xx:xx:xx = new MAC address\r\n"));
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("    config mac <xx:xx:xx:xx:xx:xx>      : Shortcut:\r\n"));
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("    config mac <xx>                     :   enter 1 octet to change last octet\r\n"));

    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("    config network_channel <xxx>        : xxx = channel\r\n"));
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("           net_chan <xxx>               :  (1-11,36,40,44,48,52,56,60,64,\r\n"));
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("                                        :   100,104,108,112,116,120,124,128,\r\n"));
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("                                        :   132,136,140,149,153,157,161,165)\r\n"));
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("    config network_name <ssid_name>     : name of AP (max %d characters)\r\n", SSID_NAME_SIZE));
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("           net_name <ssid_name>\r\n"));
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("           ssid <ssid_name>\r\n"));
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("    config network_passphrase <pass>    : passkey/password (max %d characters)\r\n", SECURITY_KEY_SIZE));
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("           net_pass <pass>\r\n"));
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("           pass <pass>\r\n"));
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("    config network_security <type>      : security type is one of: \r\n"));
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("           net_sec <type>               :   open,none,ibss,wep,wepshared,wpatkip,wpaaes,\r\n"));
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("                                        :   wpa2tkip,wpa2aes,wpa2mix,wpsopen,wpsaes\r\n"));

    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("    config save                         : save data to flash NOTE: Changes not \r\n"));
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("                                        :   automatically saved to flash!\r\n"));
}

static security_lookup_t ota2_security_name_table[] =
{
        { "open",       WICED_SECURITY_OPEN           },
        { "none",       WICED_SECURITY_OPEN           },
        { "wep",        WICED_SECURITY_WEP_PSK        },
        { "shared",     WICED_SECURITY_WEP_SHARED     },
        { "wpatkip",    WICED_SECURITY_WPA_TKIP_PSK   },
        { "wpaaes",     WICED_SECURITY_WPA_AES_PSK    },
        { "wpa2aes",    WICED_SECURITY_WPA2_AES_PSK   },
        { "wpa2tkip",   WICED_SECURITY_WPA2_TKIP_PSK  },
        { "wpa2mix",    WICED_SECURITY_WPA2_MIXED_PSK },
        { "wpa2aesent", WICED_SECURITY_WPA2_AES_ENT   },
        { "ibss",       WICED_SECURITY_IBSS_OPEN      },
        { "wpsopen",    WICED_SECURITY_WPS_OPEN       },
        { "wpsnone",    WICED_SECURITY_WPS_OPEN       },
        { "wpsaes",     WICED_SECURITY_WPS_SECURE     },
        { "invalid",    WICED_SECURITY_UNKNOWN        },
};

static uint32_t default_24g_channel_list[] =
{
        /* 2.4 GHz channels */
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
};

static uint32_t default_5g_channel_list[] =
{
        /* 5 GHz non-DFS channels */
        36, 40, 44, 48,

        /* DFS channels */
        52, 56, 60, 64,
        100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140,

        /* more non-DFS channels */
        149, 153, 157, 161, 165,
};


/******************************************************
 *               Function Definitions
 ******************************************************/
static wiced_result_t ota2_save_app_dct(ota2_data_t* player)
{
    return wiced_dct_write( (void*)player->dct_app, DCT_APP_SECTION, 0, sizeof(ota2_dct_t) );
}

static wiced_result_t ota2_save_network_dct(ota2_data_t* player)
{
    return wiced_dct_write( (void*)player->dct_network, DCT_NETWORK_CONFIG_SECTION, 0, sizeof(platform_dct_network_config_t) );
}

static wiced_result_t ota2_save_wifi_dct(ota2_data_t* player)
{
    return wiced_dct_write( (void*)player->dct_wifi, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t) );
}

/* validate the channel and return the band
 *
 * in - channel
 * out - band
 *
 * return WICED_TRUE or WICED_FALSE
 */

static wiced_bool_t ota2_get_channel_band(int channel, int* band)
{
    uint32_t i;

    if (band == NULL)
    {
        return WICED_FALSE;
    }

    /* TODO: get country code and channels in country */

    for (i = 0; i < sizeof(default_24g_channel_list)/sizeof(uint32_t) ; i++)
    {
        if (default_24g_channel_list[i] == (uint32_t)channel)
        {
            *band = WICED_802_11_BAND_2_4GHZ;
            return WICED_TRUE;
        }
    }

    for (i = 0; i < sizeof(default_5g_channel_list)/sizeof(uint32_t) ; i++)
    {
        if (default_5g_channel_list[i] == (uint32_t)channel)
        {
            *band = WICED_802_11_BAND_5GHZ;
            return WICED_TRUE;
        }
    }

    return WICED_FALSE;
}

static wiced_security_t ota2_parse_security_type(char* security_str)
{
    int table_index;

    for (table_index = 0; table_index < sizeof(ota2_security_name_table)/sizeof(security_lookup_t); table_index++)
    {
        if (strcasecmp(ota2_security_name_table[table_index].name, security_str) == 0)
        {
            return ota2_security_name_table[table_index].sec_type;
        }
    }

    return WICED_SECURITY_UNKNOWN;
}

static char* ota2_get_security_type_name(wiced_security_t type)
{
    int table_index;

    for (table_index = 0; table_index < sizeof(ota2_security_name_table)/sizeof(security_lookup_t); table_index++)
    {
        if (ota2_security_name_table[table_index].sec_type == type)
        {
            return ota2_security_name_table[table_index].name;
        }
    }

    return "";
}

static wiced_bool_t ota2_parse_mac_addr(char* in, wiced_mac_t* mac)
{
    char* colon;
    wiced_mac_t new_mac = {{0}};
    int octet_count;
    wiced_bool_t allow_one_octet = WICED_FALSE;

    if ((in == NULL) || (mac == NULL))
    {
        return WICED_FALSE;
    }

    /* We want to allow a "shortcut" of only supplying the last octet
     *  - Only if the DCT mac is non-zero
     */
    if (memcmp(&new_mac, mac, sizeof(wiced_mac_t)) != 0)
    {
        allow_one_octet = WICED_TRUE;
    }

    octet_count = 0;
    while((in != NULL) && (*in != 0) && (octet_count < 6))
    {
        colon = strchr(in, ':');
        if (colon != NULL)
        {
            *colon++ = 0;
        }

        /* convert the hex data */
        new_mac.octet[octet_count++] = hex_str_to_int( in );

        in = colon;
    }

    if(octet_count == 6)
    {
        memcpy(mac, &new_mac, sizeof(wiced_mac_t));

        return WICED_TRUE;
    }
    else if ((allow_one_octet == WICED_TRUE) && (octet_count == 1))
    {
        /* only one octet provided! */
        mac->octet[5] = new_mac.octet[0];
        return WICED_TRUE;
    }

    return WICED_FALSE;
}

static wiced_bool_t ota2_get_mac_addr_text(wiced_mac_t *mac, char* out, int out_len)
{
    if((mac == NULL) || (out == NULL) || (out_len < MAC_STR_LEN))
    {
        return WICED_FALSE;
    }

    sprintf(out, "%02x:%02x:%02x:%02x:%02x:%02x",
            mac->octet[0], mac->octet[1], mac->octet[2], mac->octet[3], mac->octet[4], mac->octet[5]);

    return WICED_TRUE;
}

void ota2_set_config(ota2_data_t* player, int argc, char *argv[])
{
    int i;
    CONFIG_CMDS_T           cmd;

    if (argc < 2)
    {
        ota2_config_print_info(player);
        return;
    }

    cmd = CONFIG_CMD_NONE;
    for (i = 0; i < (sizeof(config_command_lookup) / sizeof(cmd_lookup_t)); ++i)
    {
        if (strcasecmp(config_command_lookup[i].cmd, argv[1]) == 0)
        {
            cmd = config_command_lookup[i].event;
            break;
        }
    }

    switch( cmd )
    {
        case CONFIG_CMD_HELP:
            ota2_config_command_help();
            break;

        case CONFIG_CMD_MAC_ADDR:
        {
            char mac_str[20];
            if (argc > 2)
            {
                char new_mac_str[20];
                wiced_mac_t new_mac;

                /* start with current DCT mac value to allow one-octet shortcut */
                memcpy(&new_mac, &player->dct_wifi->mac_address, sizeof(wiced_mac_t));
                if (ota2_parse_mac_addr(argv[2], &new_mac) != WICED_TRUE)
                {
                    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("Error: %s not a valid mac.\r\n", argv[2]));
                    break;
                }

                if (memcmp(&new_mac, &player->dct_wifi->mac_address, sizeof(wiced_mac_t)) != 0)
                {
                    ota2_get_mac_addr_text(&player->dct_wifi->mac_address, mac_str, sizeof(mac_str));
                    ota2_get_mac_addr_text(&new_mac, new_mac_str, sizeof(new_mac_str));
                    OTA2_APP_PRINT(OTA2_LOG_DEBUG, ("mac_addr: %s -> %s\r\n", mac_str, new_mac_str));
                    memcpy(&player->dct_wifi->mac_address, &new_mac, sizeof(wiced_mac_t));
                    wifi_dct_dirty |= WIFI_DCT_MAC_ADDR_DIRTY;
                    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("After save, you MUST reboot for MAC address change to take effect\r\n"));
                }
            }
            ota2_get_mac_addr_text(&player->dct_wifi->mac_address, mac_str, sizeof(mac_str));
            OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("MAC address:%s\r\n", mac_str));

            {
                wiced_mac_t wiced_mac;
                wwd_wifi_get_mac_address( &wiced_mac, player->dct_network->interface );
                ota2_get_mac_addr_text(&wiced_mac, mac_str, sizeof(mac_str));
                if (memcmp(&wiced_mac, &player->dct_wifi->mac_address, sizeof(wiced_mac_t) != 0))
                {
                    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("  WICED MAC:%s\r\n", mac_str));
                }
            }
            break;
        }

        case CONFIG_CMD_NETWORK_NAME:
        {
            char name[SSID_NAME_SIZE + 1];
            int  name_len;
            if (argc > 2)
            {
                if (strlen(argv[2]) > SSID_NAME_SIZE)
                {
                    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("Error: too long (max: %d)\r\n", SSID_NAME_SIZE));
                    break;
                }

                memset(name, 0, sizeof(name));
                memcpy(name, player->dct_wifi->stored_ap_list[0].details.SSID.value, player->dct_wifi->stored_ap_list[0].details.SSID.length);
                if(strcmp(name, argv[2])  != 0)
                {
                    OTA2_APP_PRINT(OTA2_LOG_DEBUG, ("name: %s -> %s\r\n", name, argv[2]));
                    memset(player->dct_wifi->stored_ap_list[0].details.SSID.value, 0, sizeof(player->dct_wifi->stored_ap_list[0].details.SSID.value));
                    name_len = strlen(argv[2]);
                    memcpy(player->dct_wifi->stored_ap_list[0].details.SSID.value, argv[2], name_len);
                    player->dct_wifi->stored_ap_list[0].details.SSID.length = name_len;
                    wifi_dct_dirty |= WIFI_DCT_SSID_DIRTY;
                }
            }
            memset(name, 0, sizeof(name));
            memcpy(name, player->dct_wifi->stored_ap_list[0].details.SSID.value, player->dct_wifi->stored_ap_list[0].details.SSID.length);
            OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("Network name: (%d) %s\r\n", player->dct_wifi->stored_ap_list[0].details.SSID.length, name));
            break;
        }

        case CONFIG_CMD_NETWORK_CHANNEL:
        {
            int new_channel, new_band;
            if (argc > 2)
            {
                new_channel = atoi(argv[2]);
                if (ota2_get_channel_band(new_channel, &new_band) != WICED_TRUE)
                {
                    /* TODO: get country code for output */
                    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("Error: %d not a valid channel.\r\n", new_channel));
                    break;
                }
                if ((player->dct_wifi->stored_ap_list[0].details.channel != new_channel) ||
                    (player->dct_wifi->stored_ap_list[0].details.band != new_band))
                {
                    OTA2_APP_PRINT(OTA2_LOG_DEBUG, ("channel: %d -> %d\r\n  Band: %d -> %d \r\n",
                            player->dct_wifi->stored_ap_list[0].details.channel, new_channel,
                            player->dct_wifi->stored_ap_list[0].details.band, new_band));
                    player->dct_wifi->stored_ap_list[0].details.channel = new_channel;
                    player->dct_wifi->stored_ap_list[0].details.band = new_band;
                    wifi_dct_dirty |= WIFI_DCT_CHANNEL_DIRTY;
                }
            }
            OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("Network channel:%d  band:%s\r\n", player->dct_wifi->stored_ap_list[0].details.channel,
                    (player->dct_wifi->stored_ap_list[0].details.band == WICED_802_11_BAND_2_4GHZ) ? "2.4GHz" : "5GHz"));
            break;
        }

        case CONFIG_CMD_NETWORK_SECURITY_TYPE:
            if (argc > 2)
            {
                wiced_security_t new_sec_type;
                new_sec_type = ota2_parse_security_type(argv[2]);
                if (new_sec_type == WICED_SECURITY_UNKNOWN)
                {
                    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("Error: unknown security type: %s\r\n", (argv[2][0] != 0) ? argv[2] : ""));
                    break;
                }
                if (player->dct_wifi->stored_ap_list[0].details.security != new_sec_type)
                {
                    OTA2_APP_PRINT(OTA2_LOG_DEBUG, ("network security %s -> %s\r\n",
                            ota2_get_security_type_name( player->dct_wifi->stored_ap_list[0].details.security),
                            ota2_get_security_type_name(new_sec_type)));
                    player->dct_wifi->stored_ap_list[0].details.security = new_sec_type;
                    wifi_dct_dirty |= WIFI_DCT_SECURITY_TYPE_DIRTY;
                }
            }
            OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("Network security type: %s\r\n", ota2_get_security_type_name(player->dct_wifi->stored_ap_list[0].details.security)));
            break;

        case CONFIG_CMD_NETWORK_PASSPHRASE:
        {
            char pass[SECURITY_KEY_SIZE + 1];
            if (argc > 2)
            {
                if (strlen(argv[2]) > SECURITY_KEY_SIZE)
                {
                    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("Error: too long (max %d)\r\n", SECURITY_KEY_SIZE));
                    break;
                }
                memset(pass, 0, sizeof(pass));
                memcpy(pass, player->dct_wifi->stored_ap_list[0].security_key, player->dct_wifi->stored_ap_list[0].security_key_length);
                if(strcmp(pass, argv[2])  != 0)
                {
                    OTA2_APP_PRINT(OTA2_LOG_DEBUG, ("passphrase: %s -> %s \r\n", pass, argv[2]));
                    memset(pass, 0, sizeof(pass));
                    memcpy(pass, argv[2], SECURITY_KEY_SIZE);
                    memset(player->dct_wifi->stored_ap_list[0].security_key, 0, sizeof(player->dct_wifi->stored_ap_list[0].security_key));
                    memcpy(player->dct_wifi->stored_ap_list[0].security_key, argv[2], SECURITY_KEY_SIZE);
                    player->dct_wifi->stored_ap_list[0].security_key_length = strlen(pass);
                    wifi_dct_dirty |= WIFI_DCT_SECURITY_KEY_DIRTY;
                }
            }
            memset(pass, 0, sizeof(pass));
            memcpy(pass, player->dct_wifi->stored_ap_list[0].security_key, player->dct_wifi->stored_ap_list[0].security_key_length);
            OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("Network passphrase: %s \r\n", pass));
            break;
        }

        case CONFIG_CMD_SAVE:
            if (app_dct_dirty != 0)
            {
                OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("Saving App DCT:\r\n"));
                ota2_save_app_dct(player);
                app_dct_dirty = 0;
                ota_test_print_app_info(player);
            }
            if (net_dct_dirty != 0)
            {
                OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("Saving Network DCT:\r\n"));
                ota2_save_network_dct(player);
                net_dct_dirty = 0;
                ota_test_print_network_info(player);
            }
            if (wifi_dct_dirty != 0)
            {
                OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("Saving WiFi DCT:\r\n"));
                ota2_save_wifi_dct(player);
                if ((wifi_dct_dirty & WIFI_DCT_MAC_ADDR_DIRTY) != 0)
                {
                    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("You MUST reboot for MAC address change to take effect\r\n"));
                }
                wifi_dct_dirty = 0;
                ota_test_print_wifi_info(player);
            }
            break;

        default:
            OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("Unrecognized config command: %s\r\n", (argv[1][0] != 0) ? argv[1] : ""));
            ota2_config_command_help();
            break;
    }
}

static void ota_test_print_app_info(ota2_data_t* player)
{
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("  OTA Image app DCT:\r\n"));
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("       reboot_count: %ld\r\n", player->dct_app->reboot_count));
}

static void ota_test_print_network_info(ota2_data_t* player)
{
    wiced_ip_address_t ip_addr;

    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("  Network DCT:\r\n"));
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("       Interface: %s\r\n",
           (player->dct_network->interface == (wiced_interface_t)WWD_STA_INTERFACE)      ? "STA" :
           (player->dct_network->interface == (wiced_interface_t)WWD_AP_INTERFACE)       ? "AP" :
           (player->dct_network->interface == (wiced_interface_t)WWD_ETHERNET_INTERFACE) ? "Ethernet" :
           "Unknown"));
    wiced_ip_get_ipv4_address(player->dct_network->interface, &ip_addr);
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("         IP addr: %d.%d.%d.%d\r\n",
           (int)((ip_addr.ip.v4 >> 24) & 0xFF), (int)((ip_addr.ip.v4 >> 16) & 0xFF),
           (int)((ip_addr.ip.v4 >> 8) & 0xFF),  (int)(ip_addr.ip.v4 & 0xFF)));
}

static void ota_test_print_wifi_info(ota2_data_t* player)
{
    wiced_security_t sec;
    uint32_t channel;
    int band;
    char mac_str[MAC_STR_LEN], wiced_mac_str[MAC_STR_LEN];
    wiced_mac_t wiced_mac;

    wwd_wifi_get_mac_address( &wiced_mac, WICED_STA_INTERFACE );
    ota2_get_mac_addr_text(&wiced_mac, wiced_mac_str, sizeof(wiced_mac_str));
    ota2_get_mac_addr_text(&player->dct_wifi->mac_address, mac_str, sizeof(mac_str));

    if (player->dct_network->interface == WICED_STA_INTERFACE)
    {
        sec = player->dct_wifi->stored_ap_list[0].details.security;
        OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("  WiFi DCT:\r\n"));

        OTA2_APP_PRINT(OTA2_LOG_NOTIFY, (" WICED MAC (STA): %s\r\n", wiced_mac_str));

        OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("             MAC: %s %c\r\n", mac_str,
                                                        (wifi_dct_dirty & WIFI_DCT_MAC_ADDR_DIRTY) ? '*' : ' '));
        OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("            SSID: %.*s %c\r\n", player->dct_wifi->stored_ap_list[0].details.SSID.length,
                                                        player->dct_wifi->stored_ap_list[0].details.SSID.value,
                                                        (wifi_dct_dirty & WIFI_DCT_SSID_DIRTY) ? '*' : ' '));
        OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("        Security: %s %c\r\n", ota2_get_security_type_name(sec), (wifi_dct_dirty & WIFI_DCT_SECURITY_TYPE_DIRTY) ? '*' : ' '));

        if (player->dct_wifi->stored_ap_list[0].details.security != WICED_SECURITY_OPEN)
        {
            OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("      Passphrase: %.*s %c\r\n", player->dct_wifi->stored_ap_list[0].security_key_length,
                   player->dct_wifi->stored_ap_list[0].security_key, (wifi_dct_dirty & WIFI_DCT_SECURITY_KEY_DIRTY) ? '*' : ' '));
        }
        else
        {
            OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("      Passphrase: none\r\n"));
        }

        channel = player->dct_wifi->stored_ap_list[0].details.channel;
        band = player->dct_wifi->stored_ap_list[0].details.band;
        OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("         Channel: %d %c\r\n", (int)channel, (wifi_dct_dirty & WIFI_DCT_CHANNEL_DIRTY) ? '*' : ' '));
        OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("            Band: %s %c\r\n", (band == WICED_802_11_BAND_2_4GHZ) ? "2.4GHz" : "5GHz",
                                                        (wifi_dct_dirty & WIFI_DCT_CHANNEL_DIRTY) ? '*' : ' '));
    }
    else
    {
        /*
         * Nothing for AP interface yet.
         */
    }
}

static void ota_test_print_current_info(ota2_data_t* player)
{
    uint32_t channel;

    wiced_wifi_get_channel(&channel);
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, (" Current:\r\n"));
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("         Channel: %lu\r\n            Band: %s\r\n",
                     channel, channel <= 13 ? "2.4GHz" : "5GHz"));
}

void ota2_config_print_info(ota2_data_t* player)
{
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("\r\nConfig Info: * = dirty\r\n"));
    ota_test_print_app_info(player);
    ota_test_print_network_info(player);
    ota_test_print_wifi_info(player);
    ota_test_print_current_info(player);
    OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("\r\n"));
}

wiced_result_t ota2_save_config(ota2_data_t* player)
{
    wiced_result_t result;
    /* save all the configuration info */
    result = ota2_save_app_dct(player);
    if (result != WICED_SUCCESS)
    {
        OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("ota2_save_config() ota2_save_app_dct() failed:%d\r\n", result));
        return result;
    }
    result = ota2_save_network_dct(player);
    if (result != WICED_SUCCESS)
    {
        OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("ota2_save_config() ota2_save_network_dct() failed:%d\r\n", result));
        return result;
    }
    result = ota2_save_wifi_dct(player);
    if (result != WICED_SUCCESS)
    {
        OTA2_APP_PRINT(OTA2_LOG_NOTIFY, ("ota2_save_config() ota2_save_wifi_dct() failed:%d\r\n", result));
        return result;
    }

    return result;
}
