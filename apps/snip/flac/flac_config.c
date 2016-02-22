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

#include "audio_render.h"

#include "flac_test.h"
#include "flac_config.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define APP_DCT_SPEAKER_CHANNEL_DIRTY   (1 << 0)
#define APP_DCT_BUFF_MS_DIRTY           (1 << 1)
#define APP_DCT_THRESH_MS_DIRTY         (1 << 2)
#define APP_DCT_CLOCK_ENABLE_DIRTY      (1 << 3)
#define APP_DCT_VOLUME_DIRTY            (1 << 4)
#define APP_DCT_AUDIO_DEVICE_RX_DIRTY   (1 << 5)
#define APP_DCT_AUDIO_DEVICE_TX_DIRTY   (1 << 6)

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
    CONFIG_CMD_BUFFERING_MS,
    CONFIG_CMD_AUDIO_DEVICE_RX,
    CONFIG_CMD_AUDIO_DEVICE_TX,
    CONFIG_CMD_MAC_ADDR,
    CONFIG_CMD_NETWORK_CHANNEL,
    CONFIG_CMD_NETWORK_NAME,
    CONFIG_CMD_NETWORK_PASSPHRASE,
    CONFIG_CMD_NETWORK_SECURITY_TYPE,
    CONFIG_CMD_SPEAKER_CHANNEL,
    CONFIG_CMD_VOLUME,
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

typedef struct speaker_channel_name_lookup_s {
        uint32_t channel;
        char     *name;
} speaker_channel_name_lookup_t;

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
static wiced_result_t flac_save_app_dct(flac_player_t* player);
static wiced_result_t flac_save_network_dct(flac_player_t* player);
static wiced_result_t flac_save_wifi_dct(flac_player_t* player);
static void flac_print_app_info(flac_player_t* player);
static void flac_print_network_info(flac_player_t* player);
static void flac_print_wifi_info(flac_player_t* player);
static wiced_bool_t flac_get_channel_band(int channel, int* band);
static wiced_bool_t flac_encode_speaker_channel(int argc, char *argv[], AUDIO_CHANNEL_MAP_T* channel);

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

        { "audio_in",           CONFIG_CMD_AUDIO_DEVICE_RX      },
        { "audio_out",          CONFIG_CMD_AUDIO_DEVICE_TX      },
        { "audio_rx",           CONFIG_CMD_AUDIO_DEVICE_RX      },
        { "ad_rx",              CONFIG_CMD_AUDIO_DEVICE_RX      },
        { "audio_tx",           CONFIG_CMD_AUDIO_DEVICE_TX      },
        { "ad_tx",              CONFIG_CMD_AUDIO_DEVICE_TX      },

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

        { "speaker_channel",    CONFIG_CMD_SPEAKER_CHANNEL      },
        { "speaker_chan",       CONFIG_CMD_SPEAKER_CHANNEL      },
        { "spkr_chan",          CONFIG_CMD_SPEAKER_CHANNEL      },
        { "chan",               CONFIG_CMD_SPEAKER_CHANNEL      },

        { "volume",             CONFIG_CMD_VOLUME               },
        { "vol",                CONFIG_CMD_VOLUME               },

        { "save",               CONFIG_CMD_SAVE                 },

        { "", CONFIG_CMD_NONE },
};

static void flac_config_command_help( void )
{
    WPRINT_APP_INFO(("Config commands:\r\n"));
    WPRINT_APP_INFO(("    config                              : output current config\r\n"));
    WPRINT_APP_INFO(("    config <?|help>                     : show this list\r\n"));
    WPRINT_APP_INFO(("    config audio_in  <0-?>              : audio input device select\r\n"));
    WPRINT_APP_INFO(("    config audio_out <0-?>              : audio output device select\r\n"));
    WPRINT_APP_INFO(("    config audio_rx <device_X>          : enter X as in WICED_AUDIO_X, X starts at 0 for all WICED platforms\r\n"));
    WPRINT_APP_INFO(("    config ad_rx    <device_X>\r\n"));
    WPRINT_APP_INFO(("    config audio_tx <device_X>          : enter X as in WICED_AUDIO_X, X starts at 0 for all WICED platforms\r\n"));
    WPRINT_APP_INFO(("    config ad_tx    <device_X>\r\n"));
    WPRINT_APP_INFO(("    config mac_addr <xx:xx:xx:xx:xx:xx> : xx:xx:xx:xx:xx:xx = new MAC address\r\n"));
    WPRINT_APP_INFO(("    config mac <xx:xx:xx:xx:xx:xx>      : Shortcut:\r\n"));
    WPRINT_APP_INFO(("    config mac <xx>                     :   enter 1 octet to change last octet\r\n"));

    WPRINT_APP_INFO(("    config network_channel <xxx>        : xxx = channel\r\n"));
    WPRINT_APP_INFO(("           net_chan <xxx>               :  (1-11,36,40,44,48,52,56,60,64,\r\n"));
    WPRINT_APP_INFO(("                                        :   100,104,108,112,116,120,124,128,\r\n"));
    WPRINT_APP_INFO(("                                        :   132,136,140,149,153,157,161,165)\r\n"));
    WPRINT_APP_INFO(("    config network_name <ssid_name>     : name of AP (max %d characters)\r\n", SSID_NAME_SIZE));
    WPRINT_APP_INFO(("           net_name <ssid_name>\r\n"));
    WPRINT_APP_INFO(("           ssid <ssid_name>\r\n"));
    WPRINT_APP_INFO(("    config network_passphrase <pass>    : passkey/password (max %d characters)\r\n", SECURITY_KEY_SIZE));
    WPRINT_APP_INFO(("           net_pass <pass>\r\n"));
    WPRINT_APP_INFO(("           pass <pass>\r\n"));
    WPRINT_APP_INFO(("    config network_security <type>      : security type is one of: \r\n"));
    WPRINT_APP_INFO(("           net_sec <type>               :   open,none,ibss,wep,wepshared,wpatkip,wpaaes,\r\n"));
    WPRINT_APP_INFO(("                                        :   wpa2tkip,wpa2aes,wpa2mix,wpsopen,wpsaes\r\n"));

    WPRINT_APP_INFO(("    config speaker_channel <ch> [ch]... : channel mix - all will be OR'ed together\r\n"));
    WPRINT_APP_INFO(("           spkr_chan <ch> [ch]...       :    FL,FR,FC,LFE1,BL,BR,FLC,FRC,BC,LFE2,\r\n"));
    WPRINT_APP_INFO(("           chan <ch> [ch]...            :    SIL,SIR,TPFL,TPFR,TPFC,TPC,TPBL,TPBR,\r\n"));
    WPRINT_APP_INFO(("                                        :    TPSIL,TPSIR,TPBC,BTFC,BTFL,BTFR\r\n"));

    WPRINT_APP_INFO(("    config save                         : save data to flash NOTE: Changes not \r\n"));
    WPRINT_APP_INFO(("                                        :   automatically saved to flash!\r\n"));
}

static speaker_channel_name_lookup_t speaker_channel_name[] =
{
        { CHANNEL_MAP_NONE  , "NONE"   },  /* None or undefined    */
        { CHANNEL_MAP_FL    , "FL"     },  /* Front Left           */
        { CHANNEL_MAP_FR    , "FR"     },  /* Front Right          */
        { CHANNEL_MAP_FC    , "FC"     },  /* Front Center         */
        { CHANNEL_MAP_LFE1  , "LFE1"   },  /* LFE-1                */
        { CHANNEL_MAP_BL    , "BL"     },  /* Back Left            */
        { CHANNEL_MAP_BR    , "BR"     },  /* Back Right           */
        { CHANNEL_MAP_FLC   , "FLC"    },  /* Front Left Center    */
        { CHANNEL_MAP_FRC   , "FRC"    },  /* Front Right Center   */
        { CHANNEL_MAP_BC    , "BC"     },  /* Back Center          */
        { CHANNEL_MAP_LFE2  , "LFE2"   },  /* LFE-2                */
        { CHANNEL_MAP_SIL   , "SIL"    },  /* Side Left            */
        { CHANNEL_MAP_SIR   , "SIR"    },  /* Side Right           */
        { CHANNEL_MAP_TPFL  , "TPFL"   },  /* Top Front Left       */
        { CHANNEL_MAP_TPFR  , "TPFR"   },  /* Top Front Right      */
        { CHANNEL_MAP_TPFC  , "TPFC"   },  /* Top Front Center     */
        { CHANNEL_MAP_TPC   , "TPC"    },  /* Top Center           */
        { CHANNEL_MAP_TPBL  , "TPBL"   },  /* Top Back Left        */
        { CHANNEL_MAP_TPBR  , "TPBR"   },  /* Top Back Right       */
        { CHANNEL_MAP_TPSIL , "TPSIL"  },  /* Top Side Left        */
        { CHANNEL_MAP_TPSIR , "TPSIR"  },  /* Top Side Right       */
        { CHANNEL_MAP_TPBC  , "TPBC"   },  /* Top Back Center      */
        { CHANNEL_MAP_BTFC  , "BTFC"   },  /* Bottom Front Center  */
        { CHANNEL_MAP_BTFL  , "BTFL"   },  /* Bottom Front Left    */
        { CHANNEL_MAP_BTFR  , "BTFR"   },  /* Bottom Front Right   */

};

static security_lookup_t flac_security_name_table[] =
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
static wiced_result_t flac_save_app_dct(flac_player_t* player)
{
    return wiced_dct_write( (void*)player->dct_app, DCT_APP_SECTION, 0, sizeof(flac_app_dct_t) );
}

static wiced_result_t flac_save_network_dct(flac_player_t* player)
{
    return wiced_dct_write( (void*)player->dct_network, DCT_NETWORK_CONFIG_SECTION, 0, sizeof(platform_dct_network_config_t) );
}

static wiced_result_t flac_save_wifi_dct(flac_player_t* player)
{
    return wiced_dct_write( (void*)player->dct_wifi, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t) );
}

/* Prints null-terminated speaker channel type in the provided buffer */
static void flac_channel_name_get_text(uint32_t channel, char *buff, int buff_len)
{
    int table_index, outdex;

    if ((buff == NULL) || (buff_len < 3))
    {
        return;
    }

    buff[0] = 0;
    for (table_index = 0; table_index < sizeof(speaker_channel_name)/sizeof(speaker_channel_name_lookup_t); table_index++)
    {
        if (speaker_channel_name[table_index].channel & channel)
        {
            outdex = strlen(buff);
            if (strlen(speaker_channel_name[table_index].name) < (buff_len - (outdex + 1)))
            {
                strcat(buff, speaker_channel_name[table_index].name);
                strcat(buff, " ");
            }
            else
            {
                return;
            }
        }
    }
    if ((strlen(buff) == 0) && (buff_len > strlen("Invalid")))
    {
        strcpy(buff, "Invalid");
    }
}

static wiced_bool_t flac_encode_speaker_channel(int argc, char *argv[], AUDIO_CHANNEL_MAP_T* channel)
{
    AUDIO_CHANNEL_MAP_T    new_speaker_channel_map;
    int table_index, arg_index;

    if ((argc < 2) || (argv == NULL) || (channel == NULL))
    {
        return WICED_FALSE;
    }

    /* go through all argv's after 2 (config speaker_channel FL FR FC) and OR in the bit(s) */
    new_speaker_channel_map = 0;
    for (arg_index = 2; arg_index < argc; arg_index++)
    {
        for (table_index = 0; table_index < sizeof(speaker_channel_name)/sizeof(speaker_channel_name_lookup_t); table_index++)
        {
            if (strcasecmp(speaker_channel_name[table_index].name, argv[arg_index]) == 0)
            {
                new_speaker_channel_map |= speaker_channel_name[table_index].channel;
            }
        }
        WPRINT_APP_DEBUG(("\r\n   chan: 0x%08x\r\n", new_speaker_channel_map));
    }
    *channel = new_speaker_channel_map;

    return WICED_TRUE;
}

/* validate the channel and return the band
 *
 * in - channel
 * out - band
 *
 * return WICED_TRUE or WICED_FALSE
 */

static wiced_bool_t flac_get_channel_band(int channel, int* band)
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

static wiced_security_t flac_parse_security_type(char* security_str)
{
    int table_index;

    for (table_index = 0; table_index < sizeof(flac_security_name_table)/sizeof(security_lookup_t); table_index++)
    {
        if (strcasecmp(flac_security_name_table[table_index].name, security_str) == 0)
        {
            return flac_security_name_table[table_index].sec_type;
        }
    }

    return WICED_SECURITY_UNKNOWN;
}

static char* flac_get_security_type_name(wiced_security_t type)
{
    int table_index;

    for (table_index = 0; table_index < sizeof(flac_security_name_table)/sizeof(security_lookup_t); table_index++)
    {
        if (flac_security_name_table[table_index].sec_type == type)
        {
            return flac_security_name_table[table_index].name;
        }
    }

    return "";
}

static wiced_bool_t flac_parse_mac_addr(char* in, wiced_mac_t* mac)
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

static wiced_bool_t flac_get_mac_addr_text(wiced_mac_t *mac, char* out, int out_len)
{
    if((mac == NULL) || (out == NULL) || (out_len < MAC_STR_LEN))
    {
        return WICED_FALSE;
    }

    sprintf(out, "%02x:%02x:%02x:%02x:%02x:%02x",
            mac->octet[0], mac->octet[1], mac->octet[2], mac->octet[3], mac->octet[4], mac->octet[5]);

    return WICED_TRUE;
}

void flac_set_config(flac_player_t* player, int argc, char *argv[])
{
    int i;
    int volume;
    CONFIG_CMDS_T cmd;
    AUDIO_CHANNEL_MAP_T    new_speaker_channel;

    if (argc < 2)
    {
        flac_config_print_info(player);
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
            flac_config_command_help();
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
                if (flac_parse_mac_addr(argv[2], &new_mac) != WICED_TRUE)
                {
                    WPRINT_APP_INFO(("Error: %s not a valid mac.\r\n", argv[2]));
                    break;
                }

                if (memcmp(&new_mac, &player->dct_wifi->mac_address, sizeof(wiced_mac_t)) != 0)
                {
                    flac_get_mac_addr_text(&player->dct_wifi->mac_address, mac_str, sizeof(mac_str));
                    flac_get_mac_addr_text(&new_mac, new_mac_str, sizeof(new_mac_str));
                    WPRINT_APP_DEBUG(("mac_addr: %s -> %s\r\n", mac_str, new_mac_str));
                    memcpy(&player->dct_wifi->mac_address, &new_mac, sizeof(wiced_mac_t));
                    wifi_dct_dirty |= WIFI_DCT_MAC_ADDR_DIRTY;
                    WPRINT_APP_INFO(("After save, you MUST reboot for MAC address change to take effect\r\n"));
                }
            }
            flac_get_mac_addr_text(&player->dct_wifi->mac_address, mac_str, sizeof(mac_str));
            WPRINT_APP_INFO(("MAC address:%s\r\n", mac_str));

            {
                wiced_mac_t wiced_mac;
                wwd_wifi_get_mac_address( &wiced_mac, player->dct_network->interface );
                flac_get_mac_addr_text(&wiced_mac, mac_str, sizeof(mac_str));
                if (memcmp(&wiced_mac, &player->dct_wifi->mac_address, sizeof(wiced_mac_t) != 0))
                {
                    WPRINT_APP_INFO(("  WICED MAC:%s\r\n", mac_str));
                }
            }
            break;
        }

        case CONFIG_CMD_AUDIO_DEVICE_RX:
        {
            platform_audio_device_id_t device = player->dct_app->audio_device_rx;
            if (argc > 2)
            {
                int device_index = atoi(argv[2]);
                if ((device_index >= 0) && (device_index < PLATFORM_AUDIO_NUM_INPUTS))
                {
                    device = platform_audio_input_devices[device_index].device_id;
                    if (device != player->dct_app->audio_device_rx)
                    {
                        WPRINT_APP_DEBUG(("Audio device RX ID: 0x%x -> 0x%x %s\r\n",player->dct_app->audio_device_rx, device,
                                                           platform_audio_input_devices[device_index].device_name));
                        player->dct_app->audio_device_rx = device;
                        app_dct_dirty |= APP_DCT_AUDIO_DEVICE_RX_DIRTY;
                    }
                }
            }
            platform_audio_print_device_list(player->dct_app->audio_device_rx, (app_dct_dirty & APP_DCT_AUDIO_DEVICE_RX_DIRTY),
                                             AUDIO_DEVICE_ID_NONE, 0, 1);
            break;
        }

        case CONFIG_CMD_AUDIO_DEVICE_TX:
        {
            platform_audio_device_id_t device = player->dct_app->audio_device_tx;
            if (argc > 2)
            {
                int device_index = atoi(argv[2]);
                if ((device_index >= 0) && (device_index < PLATFORM_AUDIO_NUM_OUTPUTS))
                {
                    device = platform_audio_output_devices[device_index].device_id;
                    if (device != player->dct_app->audio_device_tx)
                    {
                        WPRINT_APP_DEBUG(("Audio device TX ID: 0x%x -> 0x%x %s\r\n", player->dct_app->audio_device_tx, device,
                                                           platform_audio_output_devices[device_index].device_name));
                        player->dct_app->audio_device_tx = device;
                        app_dct_dirty |= APP_DCT_AUDIO_DEVICE_TX_DIRTY;
                    }
                }
            }
            platform_audio_print_device_list(AUDIO_DEVICE_ID_NONE, 0,
                                             player->dct_app->audio_device_tx, (app_dct_dirty & APP_DCT_AUDIO_DEVICE_TX_DIRTY), 1);
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
                    WPRINT_APP_INFO(("Error: too long (max: %d)\r\n", SSID_NAME_SIZE));
                    break;
                }

                memset(name, 0, sizeof(name));
                memcpy(name, player->dct_wifi->stored_ap_list[0].details.SSID.value, player->dct_wifi->stored_ap_list[0].details.SSID.length);
                if(strcmp(name, argv[2])  != 0)
                {
                    WPRINT_APP_DEBUG(("name: %s -> %s\r\n", name, argv[2]));
                    memset(player->dct_wifi->stored_ap_list[0].details.SSID.value, 0, sizeof(player->dct_wifi->stored_ap_list[0].details.SSID.value));
                    name_len = strlen(argv[2]);
                    memcpy(player->dct_wifi->stored_ap_list[0].details.SSID.value, argv[2], name_len);
                    player->dct_wifi->stored_ap_list[0].details.SSID.length = name_len;
                    wifi_dct_dirty |= WIFI_DCT_SSID_DIRTY;
                }
            }
            memset(name, 0, sizeof(name));
            memcpy(name, player->dct_wifi->stored_ap_list[0].details.SSID.value, player->dct_wifi->stored_ap_list[0].details.SSID.length);
            WPRINT_APP_INFO(("Network name: (%d) %s\r\n", player->dct_wifi->stored_ap_list[0].details.SSID.length, name));
            break;
        }

        case CONFIG_CMD_NETWORK_CHANNEL:
        {
            int new_channel, new_band;
            if (argc > 2)
            {
                new_channel = atoi(argv[2]);
                if (flac_get_channel_band(new_channel, &new_band) != WICED_TRUE)
                {
                    /* TODO: get country code for output */
                    WPRINT_APP_INFO(("Error: %d not a valid channel.\r\n", new_channel));
                    break;
                }
                if ((player->dct_wifi->stored_ap_list[0].details.channel != new_channel) ||
                    (player->dct_wifi->stored_ap_list[0].details.band != new_band))
                {
                    WPRINT_APP_DEBUG(("channel: %d -> %d\r\n  Band: %d -> %d \r\n",
                            player->dct_wifi->stored_ap_list[0].details.channel, new_channel,
                            player->dct_wifi->stored_ap_list[0].details.band, new_band));
                    player->dct_wifi->stored_ap_list[0].details.channel = new_channel;
                    player->dct_wifi->stored_ap_list[0].details.band = new_band;
                    wifi_dct_dirty |= WIFI_DCT_CHANNEL_DIRTY;
                }
            }
            WPRINT_APP_INFO(("Network channel:%d  band:%s\r\n", player->dct_wifi->stored_ap_list[0].details.channel,
                    (player->dct_wifi->stored_ap_list[0].details.band == WICED_802_11_BAND_2_4GHZ) ? "2.4GHz" : "5GHz"));
            break;
        }

        case CONFIG_CMD_NETWORK_SECURITY_TYPE:
            if (argc > 2)
            {
                wiced_security_t new_sec_type;
                new_sec_type = flac_parse_security_type(argv[2]);
                if (new_sec_type == WICED_SECURITY_UNKNOWN)
                {
                    WPRINT_APP_INFO(("Error: unknown security type: %s\r\n", (argv[2][0] != 0) ? argv[2] : ""));
                    break;
                }
                if (player->dct_wifi->stored_ap_list[0].details.security != new_sec_type)
                {
                    WPRINT_APP_DEBUG(("network security %s -> %s\r\n",
                            flac_get_security_type_name( player->dct_wifi->stored_ap_list[0].details.security),
                            flac_get_security_type_name(new_sec_type)));
                    player->dct_wifi->stored_ap_list[0].details.security = new_sec_type;
                    wifi_dct_dirty |= WIFI_DCT_SECURITY_TYPE_DIRTY;
                }
            }
            WPRINT_APP_INFO(("Network security type: %s\r\n", flac_get_security_type_name(player->dct_wifi->stored_ap_list[0].details.security)));
            break;

        case CONFIG_CMD_NETWORK_PASSPHRASE:
        {
            char pass[SECURITY_KEY_SIZE + 1];
            if (argc > 2)
            {
                if (strlen(argv[2]) > SECURITY_KEY_SIZE)
                {
                    WPRINT_APP_INFO(("Error: too long (max %d)\r\n", SECURITY_KEY_SIZE));
                    break;
                }
                memset(pass, 0, sizeof(pass));
                memcpy(pass, player->dct_wifi->stored_ap_list[0].security_key, player->dct_wifi->stored_ap_list[0].security_key_length);
                if(strcmp(pass, argv[2])  != 0)
                {
                    WPRINT_APP_DEBUG(("passphrase: %s -> %s \r\n", pass, argv[2]));
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
            WPRINT_APP_INFO(("Network passphrase: %s \r\n", pass));
            break;
        }

        case CONFIG_CMD_SPEAKER_CHANNEL:
        {
            char buff[10], buff2[10];
            if (argc > 2)
            {
                /* convert channel string to the bit fields */
                if( flac_encode_speaker_channel(argc, argv, &new_speaker_channel) == WICED_TRUE)
                {
                    flac_channel_name_get_text(player->dct_app->channel, buff, sizeof(buff));
                    flac_channel_name_get_text(new_speaker_channel, buff2, sizeof(buff2));
                    if (player->dct_app->channel != new_speaker_channel)
                    {
                        WPRINT_APP_DEBUG(("Speaker channel: (0x%08x) %s -> (0x%08x) %s\r\n", player->dct_app->channel, buff, new_speaker_channel, buff2));
                        player->dct_app->channel = new_speaker_channel;
                        app_dct_dirty |= APP_DCT_SPEAKER_CHANNEL_DIRTY;
                    }
                }
            }
            flac_channel_name_get_text(player->dct_app->channel, buff, sizeof(buff));
            WPRINT_APP_INFO(("Speaker channel: (0x%08x) %s\r\n", player->dct_app->channel, buff));
            break;
        }

        case CONFIG_CMD_VOLUME:
            if (argc > 2)
            {
                volume = atoi(argv[2]);
                /* validity check */
                if (volume < FLAC_VOLUME_MIN || volume > FLAC_VOLUME_MAX)
                {
                    WPRINT_APP_INFO(("Error: out of range min: %d  max: %d\r\n", FLAC_VOLUME_MIN, FLAC_VOLUME_MAX));
                    break;
                }
                if (player->dct_app->volume != volume)
                {
                    WPRINT_APP_INFO(("CONFIG_CMD_VOLUME: %d -> %d\r\n", player->dct_app->volume, volume));
                    player->dct_app->volume = volume;
                }
            }
            WPRINT_APP_INFO(("Volume: %d\r\n", player->dct_app->volume));
            app_dct_dirty |= APP_DCT_VOLUME_DIRTY;

            /* if playing, change volume :) */
            if (player->audio_renderer != NULL)
            {
                wiced_audio_render_set_volume(player->audio_renderer, player->dct_app->volume);
            }

            break;

        case CONFIG_CMD_SAVE:
            if (app_dct_dirty != 0)
            {
                WPRINT_APP_INFO(("Saving App DCT:\r\n"));
                flac_save_app_dct(player);
                app_dct_dirty = 0;
                flac_print_app_info(player);
            }
            if (net_dct_dirty != 0)
            {
                WPRINT_APP_INFO(("Saving Network DCT:\r\n"));
                flac_save_network_dct(player);
                net_dct_dirty = 0;
                flac_print_network_info(player);
            }
            if (wifi_dct_dirty != 0)
            {
                WPRINT_APP_INFO(("Saving WiFi DCT:\r\n"));
                flac_save_wifi_dct(player);
                if ((wifi_dct_dirty & WIFI_DCT_MAC_ADDR_DIRTY) != 0)
                {
                    WPRINT_APP_INFO(("You MUST reboot for MAC address change to take effect\r\n"));
                }
                wifi_dct_dirty = 0;
                flac_print_wifi_info(player);
            }
            break;

        default:
            WPRINT_APP_INFO(("Unrecognized config command: %s\r\n", (argv[1][0] != 0) ? argv[1] : ""));
            flac_config_command_help();
            break;
    }
}

static void flac_print_app_info(flac_player_t* player)
{
    char buff[10];
    flac_channel_name_get_text(player->dct_app->channel, buff, sizeof(buff));

    WPRINT_APP_INFO(("  FLAC app DCT:\r\n"));
    WPRINT_APP_INFO(("         channel: (0x%08x) %s %c\r\n", player->dct_app->channel, buff, (app_dct_dirty & APP_DCT_SPEAKER_CHANNEL_DIRTY) ? '*' : ' '));
    WPRINT_APP_INFO(("       buffering: %d ms %c\r\n", player->dct_app->buffering_ms, (app_dct_dirty & APP_DCT_BUFF_MS_DIRTY) ? '*' : ' '));
    WPRINT_APP_INFO(("       threshold: %d ms %c\r\n", player->dct_app->threshold_ms, (app_dct_dirty & APP_DCT_THRESH_MS_DIRTY) ? '*' : ' '));
    WPRINT_APP_INFO(("    clock enable: %s %c\r\n", player->dct_app->clock_enable == 0 ? "disable" : "enable", (app_dct_dirty & APP_DCT_CLOCK_ENABLE_DIRTY) ? '*' : ' '));
    WPRINT_APP_INFO(("          volume: %d %c\r\n", player->dct_app->volume, (app_dct_dirty & APP_DCT_VOLUME_DIRTY) ? '*' : ' '));

    platform_audio_print_device_list(player->dct_app->audio_device_rx, (app_dct_dirty & APP_DCT_AUDIO_DEVICE_RX_DIRTY),
                                     player->dct_app->audio_device_tx, (app_dct_dirty & APP_DCT_AUDIO_DEVICE_TX_DIRTY), 0);
}

static void flac_print_network_info(flac_player_t* player)
{
    wiced_ip_address_t ip_addr;

    WPRINT_APP_INFO(("  Network DCT:\r\n"));
    WPRINT_APP_INFO(("       Interface: %s\r\n",
           (player->dct_network->interface == (wiced_interface_t)WWD_STA_INTERFACE)      ? "STA" :
           (player->dct_network->interface == (wiced_interface_t)WWD_AP_INTERFACE)       ? "AP" :
           (player->dct_network->interface == (wiced_interface_t)WWD_ETHERNET_INTERFACE) ? "Ethernet" :
           "Unknown"));
    wiced_ip_get_ipv4_address(player->dct_network->interface, &ip_addr);
    WPRINT_APP_INFO(("         IP addr: %d.%d.%d.%d\r\n",
           (int)((ip_addr.ip.v4 >> 24) & 0xFF), (int)((ip_addr.ip.v4 >> 16) & 0xFF),
           (int)((ip_addr.ip.v4 >> 8) & 0xFF),  (int)(ip_addr.ip.v4 & 0xFF)));
}

static void flac_print_wifi_info(flac_player_t* player)
{
    wiced_security_t sec;
    uint32_t channel;
    int band;
    char mac_str[MAC_STR_LEN], wiced_mac_str[MAC_STR_LEN];
    wiced_mac_t wiced_mac;

    wwd_wifi_get_mac_address( &wiced_mac, WICED_STA_INTERFACE );
    flac_get_mac_addr_text(&wiced_mac, wiced_mac_str, sizeof(wiced_mac_str));
    flac_get_mac_addr_text(&player->dct_wifi->mac_address, mac_str, sizeof(mac_str));

    if (player->dct_network->interface == WICED_STA_INTERFACE)
    {
        sec = player->dct_wifi->stored_ap_list[0].details.security;
        WPRINT_APP_INFO(("  WiFi DCT:\r\n"));

        WPRINT_APP_INFO((" WICED MAC (STA): %s\r\n", wiced_mac_str));

        WPRINT_APP_INFO(("             MAC: %s %c\r\n", mac_str,
                                                        (wifi_dct_dirty & WIFI_DCT_MAC_ADDR_DIRTY) ? '*' : ' '));
        WPRINT_APP_INFO(("            SSID: %.*s %c\r\n", player->dct_wifi->stored_ap_list[0].details.SSID.length,
                                                        player->dct_wifi->stored_ap_list[0].details.SSID.value,
                                                        (wifi_dct_dirty & WIFI_DCT_SSID_DIRTY) ? '*' : ' '));
        WPRINT_APP_INFO(("        Security: %s %c\r\n", flac_get_security_type_name(sec), (wifi_dct_dirty & WIFI_DCT_SECURITY_TYPE_DIRTY) ? '*' : ' '));

        if (player->dct_wifi->stored_ap_list[0].details.security != WICED_SECURITY_OPEN)
        {
            WPRINT_APP_INFO(("      Passphrase: %.*s %c\r\n", player->dct_wifi->stored_ap_list[0].security_key_length,
                   player->dct_wifi->stored_ap_list[0].security_key, (wifi_dct_dirty & WIFI_DCT_SECURITY_KEY_DIRTY) ? '*' : ' '));
        }
        else
        {
            WPRINT_APP_INFO(("      Passphrase: none\r\n"));
        }

        channel = player->dct_wifi->stored_ap_list[0].details.channel;
        band = player->dct_wifi->stored_ap_list[0].details.band;
        WPRINT_APP_INFO(("         Channel: %d %c\r\n", (int)channel, (wifi_dct_dirty & WIFI_DCT_CHANNEL_DIRTY) ? '*' : ' '));
        WPRINT_APP_INFO(("            Band: %s %c\r\n", (band == WICED_802_11_BAND_2_4GHZ) ? "2.4GHz" : "5GHz",
                                                        (wifi_dct_dirty & WIFI_DCT_CHANNEL_DIRTY) ? '*' : ' '));
    }
    else
    {
        /*
         * Nothing for AP interface yet.
         */
    }
}

static void flac_print_current_info(flac_player_t* player)
{
    uint32_t channel;

    wiced_wifi_get_channel(&channel);
    WPRINT_APP_INFO((" Current:\r\n"));
    WPRINT_APP_INFO(("         Channel: %lu\r\n            Band: %s\r\n",
                     channel, channel <= 13 ? "2.4GHz" : "5GHz"));
}

void flac_config_print_info(flac_player_t* player)
{
    WPRINT_APP_INFO(("\r\nConfig Info: * = dirty\r\n"));
    flac_print_app_info(player);
    flac_print_network_info(player);
    flac_print_wifi_info(player);
    flac_print_current_info(player);
    WPRINT_APP_INFO(("\r\n"));
}
