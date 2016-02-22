/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

/** @file Apollo configuration routines.
 *
 */

#include "wiced.h"
#include "platform_audio.h"
#include "command_console.h"

#include "audio_render.h"

#include "apollocore.h"
#include "apollo_log.h"
#include "apollo_config.h"
#include "apollo_streamer.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define PRINT_MAC(mac)                              mac.octet[0],mac.octet[1],mac.octet[2],mac.octet[3],mac.octet[4],mac.octet[5]

#define TABLE_ENTRY_GET_TEXT(table, value)          apollo_lookup_table_get_text(table, sizeof(table)/sizeof(lookup_table_entry_t), value)
#define TABLE_ENTRY_GET_VALUE(table, str, value)    apollo_lookup_table_get_value(table, sizeof(table)/sizeof(lookup_table_entry_t), str, value)

#define DIRTY_CHECK(flags,bit)                      ((flags & bit) != 0 ? '*' : ' ')

/******************************************************
 *                    Constants
 *
 ******************************************************/

#define APP_DCT_SPEAKER_NAME_DIRTY      (1 << 0)
#define APP_DCT_SPEAKER_CHANNEL_DIRTY   (1 << 1)
#define APP_DCT_AUTO_START_DIRTY        (1 << 2)
#define APP_DCT_BUFF_MS_DIRTY           (1 << 3)
#define APP_DCT_THRESH_MS_DIRTY         (1 << 4)
#define APP_DCT_CLOCK_ENABLE_DIRTY      (1 << 5)
#define APP_DCT_VOLUME_DIRTY            (1 << 6)
#define APP_DCT_PAYLOAD_SIZE_DIRTY      (1 << 7)
#define APP_DCT_FEC_ORDER_DIRTY         (1 << 8)
#define APP_DCT_FEC_LENGTH_DIRTY        (1 << 9)
#define APP_DCT_SOURCE_TYPE_DIRTY       (1 << 10)
#define APP_DCT_AUDIO_DEVICE_RX_DIRTY   (1 << 11)
#define APP_DCT_AUDIO_DEVICE_TX_DIRTY   (1 << 12)
#define APP_DCT_APOLLO_ROLE_DIRTY       (1 << 13)
#define APP_DCT_CLIENTADDR_DIRTY        (1 << 14)
#define APP_DCT_RTP_PORT_DIRTY          (1 << 15)
#define APP_DCT_LOG_LEVEL_DIRTY         (1 << 16)
#define APP_DCT_IS_CONFIGURED_DIRTY     (1 << 17)
#define APP_DCT_PLL_TUNING_ENABLE_DIRTY (1 << 18)
#define APP_DCT_INPUT_SAMPLE_RATE_DIRTY (1 << 19)

#define WIFI_DCT_SECURITY_KEY_DIRTY     (1 << 0)
#define WIFI_DCT_SECURITY_TYPE_DIRTY    (1 << 1)
#define WIFI_DCT_CHANNEL_DIRTY          (1 << 2)
#define WIFI_DCT_SSID_DIRTY             (1 << 3)
#define WIFI_DCT_MAC_ADDR_DIRTY         (1 << 4)

#define BT_DCT_MAC_ADDR_DIRTY           (1 << 0)
#define BT_DCT_DEV_NAME_DIRTY           (1 << 1)
#define BT_DCT_DEV_CLASS_DIRTY          (1 << 2)

#define MAC_STR_LEN                     (18)

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
    CONFIG_CMD_NONE         = 0,
    CONFIG_CMD_HELP,
    CONFIG_CMD_AUTO_START,
    CONFIG_CMD_BUFFERING_MS,
    CONFIG_CMD_CLOCK_ENABLE,
    CONFIG_CMD_MAC_ADDR,
    CONFIG_CMD_NETWORK_CHANNEL,
    CONFIG_CMD_NETWORK_NAME,
    CONFIG_CMD_NETWORK_PASSPHRASE,
    CONFIG_CMD_NETWORK_SECURITY_TYPE,
    CONFIG_CMD_SPEAKER_CHANNEL,
    CONFIG_CMD_SPEAKER_NAME,
    CONFIG_CMD_THRESHOLD_MS,
    CONFIG_CMD_VOLUME,
    CONFIG_CMD_PAYLOAD_SIZE,
    CONFIG_CMD_FEC_ORDER,
    CONFIG_CMD_FEC_LENGTH,
    CONFIG_CMD_SOURCE_TYPE,
    CONFIG_CMD_AUDIO_DEVICE_RX,
    CONFIG_CMD_AUDIO_DEVICE_TX,
    CONFIG_CMD_APOLLO_ROLE,
    CONFIG_CMD_CLIENTADDR,
    CONFIG_CMD_RTP_PORT,
    CONFIG_CMD_LOG_LEVEL,
    CONFIG_CMD_IS_CONFIGURED,
    CONFIG_CMD_PLL_TUNING_ENABLE,
    CONFIG_CMD_INPUT_SAMPLE_RATE,
#ifdef WICED_DCT_INCLUDE_BT_CONFIG
    CONFIG_CMD_BT_DEVICE_NAME,
    CONFIG_CMD_BT_DEVICE_MAC_ADDR,
    CONFIG_CMD_BT_DEVICE_CLASS,
#endif
    CONFIG_CMD_SAVE,

    CONFIG_CMD_MAX,
} CONFIG_CMDS_T;

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct
{
    char *name;
    uint32_t value;
} lookup_table_entry_t;

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

static wiced_result_t apollo_save_app_dct(apollo_dct_collection_t* dct_tables);
static wiced_result_t apollo_save_network_dct(apollo_dct_collection_t* dct_tables);
static wiced_result_t apollo_save_wifi_dct(apollo_dct_collection_t* dct_tables);
static void apollo_print_app_info(apollo_dct_collection_t* dct_tables);
static void apollo_print_network_info(apollo_dct_collection_t* dct_tables);
static void apollo_print_wifi_info(apollo_dct_collection_t* dct_tables);
static wiced_bool_t apollo_encode_speaker_channel(int argc, char *argv[], APOLLO_CHANNEL_MAP_T* channel);
static wiced_bool_t apollo_get_channel_band(int channel, int* band);

#ifdef WICED_DCT_INCLUDE_BT_CONFIG
static wiced_result_t apollo_save_bt_dct(apollo_dct_collection_t* dct_tables);
static void apollo_print_bt_info(apollo_dct_collection_t* dct_tables);
#endif

/******************************************************
 *               Variables Definitions
 ******************************************************/

/* dirty flags for determining what to save */
static uint32_t app_dct_dirty;
static uint32_t net_dct_dirty;
static uint32_t wifi_dct_dirty;
#ifdef WICED_DCT_INCLUDE_BT_CONFIG
static uint32_t bt_dct_dirty;
#endif

static lookup_table_entry_t config_command_lookup[] = {
    { "help",               CONFIG_CMD_HELP                 },
    { "?",                  CONFIG_CMD_HELP                 },
    { "auto_start",         CONFIG_CMD_AUTO_START           },
    { "auto",               CONFIG_CMD_AUTO_START           },

    { "apollo_role",        CONFIG_CMD_APOLLO_ROLE          },
    { "role",               CONFIG_CMD_APOLLO_ROLE          },

    { "buffering_ms",       CONFIG_CMD_BUFFERING_MS         },
    { "buff_ms",            CONFIG_CMD_BUFFERING_MS         },

    { "clock",              CONFIG_CMD_CLOCK_ENABLE         },
    { "pll_tuning",         CONFIG_CMD_PLL_TUNING_ENABLE    },
    { "pll",                CONFIG_CMD_PLL_TUNING_ENABLE    },

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

    { "speaker_name",       CONFIG_CMD_SPEAKER_NAME         },
    { "speaker_channel",    CONFIG_CMD_SPEAKER_CHANNEL      },
    { "speaker_chan",       CONFIG_CMD_SPEAKER_CHANNEL      },

    { "spkr_name",          CONFIG_CMD_SPEAKER_NAME         },
    { "spkr_chan",          CONFIG_CMD_SPEAKER_CHANNEL      },

    { "threshold_ms",       CONFIG_CMD_THRESHOLD_MS         },
    { "thresh_ms",          CONFIG_CMD_THRESHOLD_MS         },

    { "volume",             CONFIG_CMD_VOLUME               },
    { "vol",                CONFIG_CMD_VOLUME               },

    { "payload_size",       CONFIG_CMD_PAYLOAD_SIZE         },
    { "fec_order",          CONFIG_CMD_FEC_ORDER            },
    { "fec_length",         CONFIG_CMD_FEC_LENGTH           },
    { "fec",                CONFIG_CMD_FEC_LENGTH           },

    { "source_type",        CONFIG_CMD_SOURCE_TYPE          },
    { "src_t",              CONFIG_CMD_SOURCE_TYPE          },
    { "audio_device_rx",    CONFIG_CMD_AUDIO_DEVICE_RX      },
    { "ad_rx",              CONFIG_CMD_AUDIO_DEVICE_RX      },
    { "audio_device_tx",    CONFIG_CMD_AUDIO_DEVICE_TX      },
    { "ad_tx",              CONFIG_CMD_AUDIO_DEVICE_TX      },
    { "input_sample_rate",  CONFIG_CMD_INPUT_SAMPLE_RATE    },
    { "sr_rx",              CONFIG_CMD_INPUT_SAMPLE_RATE    },

    { "clientaddr",         CONFIG_CMD_CLIENTADDR           },
    { "addr",               CONFIG_CMD_CLIENTADDR           },

    { "rtp_port",           CONFIG_CMD_RTP_PORT             },
    { "port",               CONFIG_CMD_RTP_PORT             },

    { "log_level",          CONFIG_CMD_LOG_LEVEL            },
    { "log",                CONFIG_CMD_LOG_LEVEL            },
    { "is_configured",      CONFIG_CMD_IS_CONFIGURED        },
    { "is_conf",            CONFIG_CMD_IS_CONFIGURED        },

#ifdef WICED_DCT_INCLUDE_BT_CONFIG
    { "bt_name",            CONFIG_CMD_BT_DEVICE_NAME       },
    { "bt_dev",             CONFIG_CMD_BT_DEVICE_NAME       },
    { "bt_mac",             CONFIG_CMD_BT_DEVICE_MAC_ADDR   },
    { "bt_class",           CONFIG_CMD_BT_DEVICE_CLASS      },
#endif

    { "save",               CONFIG_CMD_SAVE                 },

    { "", CONFIG_CMD_NONE },
};

static lookup_table_entry_t speaker_channel_name[] =
{
    { "NONE",   CHANNEL_MAP_NONE    },  /* None or undefined    */
    { "FL",     CHANNEL_MAP_FL      },  /* Front Left           */
    { "FR",     CHANNEL_MAP_FR      },  /* Front Right          */
    { "FC",     CHANNEL_MAP_FC      },  /* Front Center         */
    { "LFE1",   CHANNEL_MAP_LFE1    },  /* LFE-1                */
    { "BL",     CHANNEL_MAP_BL      },  /* Back Left            */
    { "BR",     CHANNEL_MAP_BR      },  /* Back Right           */
    { "FLC",    CHANNEL_MAP_FLC     },  /* Front Left Center    */
    { "FRC",    CHANNEL_MAP_FRC     },  /* Front Right Center   */
    { "BC",     CHANNEL_MAP_BC      },  /* Back Center          */
    { "LFE2",   CHANNEL_MAP_LFE2    },  /* LFE-2                */
    { "SIL",    CHANNEL_MAP_SIL     },  /* Side Left            */
    { "SIR",    CHANNEL_MAP_SIR     },  /* Side Right           */
    { "TPFL",   CHANNEL_MAP_TPFL    },  /* Top Front Left       */
    { "TPFR",   CHANNEL_MAP_TPFR    },  /* Top Front Right      */
    { "TPFC",   CHANNEL_MAP_TPFC    },  /* Top Front Center     */
    { "TPC",    CHANNEL_MAP_TPC     },  /* Top Center           */
    { "TPBL",   CHANNEL_MAP_TPBL    },  /* Top Back Left        */
    { "TPBR",   CHANNEL_MAP_TPBR    },  /* Top Back Right       */
    { "TPSIL",  CHANNEL_MAP_TPSIL   },  /* Top Side Left        */
    { "TPSIR",  CHANNEL_MAP_TPSIR   },  /* Top Side Right       */
    { "TPBC",   CHANNEL_MAP_TPBC    },  /* Top Back Center      */
    { "BTFC",   CHANNEL_MAP_BTFC    },  /* Bottom Front Center  */
    { "BTFL",   CHANNEL_MAP_BTFL    },  /* Bottom Front Left    */
    { "BTFR",   CHANNEL_MAP_BTFR    },  /* Bottom Front Right   */
};

static lookup_table_entry_t apollo_security_name_table[] =
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

static lookup_table_entry_t apollo_source_type_table[] =
{
    { "capture",    APOLLO_AUDIO_SOURCE_CAPTURE      },
    { "bt",         APOLLO_AUDIO_SOURCE_BT           },
};

static lookup_table_entry_t apollo_fec_order_table[] =
{
    { "pre",        RTP_AUDIO_FEC_PRIOR           },
    { "post",       RTP_AUDIO_FEC_POST            },
};

static lookup_table_entry_t apollo_role_type_table[] =
{
    { "source",     APOLLO_ROLE_SOURCE      },
    { "sink",       APOLLO_ROLE_SINK        },
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

static wiced_result_t apollo_save_app_dct(apollo_dct_collection_t* dct_tables)
{
    return wiced_dct_write((void*)dct_tables->dct_app, DCT_APP_SECTION, 0, sizeof(apollo_dct_t));
}

static wiced_result_t apollo_save_network_dct(apollo_dct_collection_t* dct_tables)
{
    return wiced_dct_write((void*)dct_tables->dct_network, DCT_NETWORK_CONFIG_SECTION, 0, sizeof(platform_dct_network_config_t));
}

static wiced_result_t apollo_save_wifi_dct(apollo_dct_collection_t* dct_tables)
{
    return wiced_dct_write((void*)dct_tables->dct_wifi, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t));
}

#ifdef WICED_DCT_INCLUDE_BT_CONFIG
static wiced_result_t apollo_save_bt_dct(apollo_dct_collection_t* dct_tables)
{
    return wiced_dct_write((void*)dct_tables->dct_bt, DCT_BT_CONFIG_SECTION, 0, sizeof(platform_dct_bt_config_t));
}
#endif

static char* apollo_lookup_table_get_text(lookup_table_entry_t *table, int table_entries, uint32_t value)
{
    int table_index;

    for (table_index = 0; table_index < table_entries; table_index++)
    {
        if (table[table_index].value == value)
        {
            return table[table_index].name;
        }
    }

    return "";
}

static wiced_result_t apollo_lookup_table_get_value(lookup_table_entry_t *table, int table_entries, char* str, uint32_t *value)
{
    wiced_result_t result = WICED_ERROR;
    int table_index;

    for (table_index = 0; table_index < table_entries; table_index++)
    {
        if (strcasecmp(table[table_index].name, str) == 0)
        {
            *value = table[table_index].value;
            result = WICED_SUCCESS;
            break;
        }
    }

    return result;
}

/* Prints null-terminated speaker channel type in the provided buffer */
static void apollo_channel_name_get_text(uint32_t channel, char *buff, int buff_len)
{
    int table_index, outdex;

    if ((buff == NULL) || (buff_len < 3))
    {
        return;
    }

    buff[0] = 0;
    for (table_index = 0; table_index < sizeof(speaker_channel_name)/sizeof(lookup_table_entry_t); table_index++)
    {
        if (speaker_channel_name[table_index].value & channel)
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

static wiced_bool_t apollo_encode_speaker_channel(int argc, char *argv[], APOLLO_CHANNEL_MAP_T* channel)
{
    APOLLO_CHANNEL_MAP_T new_speaker_channel_map;
    int table_index, arg_index;

    if ((argc < 2) || (argv == NULL) || (channel == NULL))
    {
        return WICED_FALSE;
    }

    /* go through all argv's after 2 (config speaker_channel FL FR FC) and OR in the bit(s) */
    new_speaker_channel_map = 0;
    for (arg_index = 2; arg_index < argc; arg_index++)
    {
        for (table_index = 0; table_index < sizeof(speaker_channel_name)/sizeof(lookup_table_entry_t); table_index++)
        {
            if (strcasecmp(speaker_channel_name[table_index].name, argv[arg_index]) == 0)
            {
                new_speaker_channel_map |= speaker_channel_name[table_index].value;
            }
        }
        apollo_log_msg(APOLLO_LOG_DEBUG0, "\r\n   chan: 0x%08x\r\n", new_speaker_channel_map);
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

static wiced_bool_t apollo_get_channel_band(int channel, int* band)
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

static wiced_security_t apollo_security_type_parse(char* security_str)
{
    wiced_security_t security_type;

    if (TABLE_ENTRY_GET_VALUE(apollo_security_name_table, security_str, (uint32_t*)&security_type) != WICED_SUCCESS)
    {
        security_type = WICED_SECURITY_UNKNOWN;
    }

    return security_type;
}

static char* apollo_security_type_get_name(wiced_security_t type)
{
    return TABLE_ENTRY_GET_TEXT(apollo_security_name_table, type);
}

#ifdef WICED_DCT_INCLUDE_BT_CONFIG
static wiced_bool_t apollo_bt_device_class_parse(char* in, uint8_t* class)
{
    char* colon;
    uint8_t new_class[3] = { 0 };
    int octet_count;

    if ((in == NULL) || (class == NULL))
    {
        return WICED_FALSE;
    }

    octet_count = 0;
    while ((in != NULL) && (*in != 0) && (octet_count < 3))
    {
        colon = strchr(in, ':');
        if (colon != NULL)
        {
            *colon++ = 0;
        }

        /* convert the hex data */
        new_class[octet_count++] = hex_str_to_int(in);

        in = colon;
    }

    if (octet_count == 3)
    {
        memcpy(class, new_class, sizeof(new_class));

        return WICED_TRUE;
    }

    return WICED_FALSE;
}
#endif

static wiced_bool_t apollo_mac_addr_parse(char* in, uint8_t* octet)
{
    char* colon;
    wiced_mac_t new_mac = {{0}};
    int octet_count;
    wiced_bool_t allow_one_octet = WICED_FALSE;

    if ((in == NULL) || (octet == NULL))
    {
        return WICED_FALSE;
    }

    /* We want to allow a "shortcut" of only supplying the last octet
     *  - Only if the DCT mac is non-zero
     */
    if (memcmp(&new_mac, octet, sizeof(wiced_mac_t)) != 0)
    {
        allow_one_octet = WICED_TRUE;
    }

    octet_count = 0;
    while ((in != NULL) && (*in != 0) && (octet_count < 6))
    {
        colon = strchr(in, ':');
        if (colon != NULL)
        {
            *colon++ = 0;
        }

        /* convert the hex data */
        new_mac.octet[octet_count++] = hex_str_to_int(in);

        in = colon;
    }

    if (octet_count == 6)
    {
        memcpy(octet, &new_mac, sizeof(wiced_mac_t));

        return WICED_TRUE;
    }
    else if ((allow_one_octet == WICED_TRUE) && (octet_count == 1))
    {
        /* only one octet provided! */
        octet[5] = new_mac.octet[0];
        return WICED_TRUE;
    }

    return WICED_FALSE;
}

static wiced_result_t apollo_fec_order_get(char* fec_order_str, uint32_t* fec_order)
{
    return TABLE_ENTRY_GET_VALUE(apollo_fec_order_table, fec_order_str, fec_order);
}

static char* apollo_fec_order_get_text(uint32_t fec_order)
{
    return TABLE_ENTRY_GET_TEXT(apollo_fec_order_table, fec_order);
}

static wiced_result_t apollo_source_type_get(char* source_type_str, uint32_t* source_type)
{
    return TABLE_ENTRY_GET_VALUE(apollo_source_type_table, source_type_str, source_type);
}

static char* apollo_source_type_get_text(int source_type)
{
    return TABLE_ENTRY_GET_TEXT(apollo_source_type_table, source_type);
}

static wiced_result_t apollo_role_type_get(char* apollo_role_str, uint32_t* apollo_role)
{
    return TABLE_ENTRY_GET_VALUE(apollo_role_type_table, apollo_role_str, apollo_role);
}

static char* apollo_role_type_get_text(uint32_t apollo_role)
{
    return TABLE_ENTRY_GET_TEXT(apollo_role_type_table, apollo_role);
}

static platform_audio_sample_rates_t apollo_sample_rate_type_get(int sample_rate)
{
    platform_audio_sample_rates_t sr = (platform_audio_sample_rates_t)0;

    switch(sample_rate)
    {
        case 8000:
            sr = PLATFORM_AUDIO_SAMPLE_RATE_8KHZ;
            break;
        case 12000:
            sr = PLATFORM_AUDIO_SAMPLE_RATE_12KHZ;
            break;
        case 16000:
            sr = PLATFORM_AUDIO_SAMPLE_RATE_16KHZ;
            break;
        case 24000:
            sr = PLATFORM_AUDIO_SAMPLE_RATE_24KHZ;
            break;
        case 32000:
            sr = PLATFORM_AUDIO_SAMPLE_RATE_32KHZ;
            break;
        case 48000:
            sr = PLATFORM_AUDIO_SAMPLE_RATE_48KHZ;
            break;
        case 64000:
            sr = PLATFORM_AUDIO_SAMPLE_RATE_64KHZ;
            break;
        case 96000:
            sr = PLATFORM_AUDIO_SAMPLE_RATE_96KHZ;
            break;
        case 128000:
            sr = PLATFORM_AUDIO_SAMPLE_RATE_128KHZ;
            break;
        case 192000:
            sr = PLATFORM_AUDIO_SAMPLE_RATE_192KHZ;
            break;

        case 11025:
            sr = PLATFORM_AUDIO_SAMPLE_RATE_11_025KHZ;
            break;
        case 22050:
            sr = PLATFORM_AUDIO_SAMPLE_RATE_22_05KHZ;
            break;
        case 44100:
            sr = PLATFORM_AUDIO_SAMPLE_RATE_44_1KHZ;
            break;
        case 88200:
            sr = PLATFORM_AUDIO_SAMPLE_RATE_88_2KHZ;
            break;
        case 176400:
            sr = PLATFORM_AUDIO_SAMPLE_RATE_176_4KHZ;
            break;

        default:
            break;
    }

    return sr;
}


static void apollo_config_command_help(void)
{
    apollo_log_printf("Config commands:\r\n");
    apollo_log_printf("    config                              : output current config\r\n");
    apollo_log_printf("    config <?|help>                     : show this list\r\n");
    apollo_log_printf("    config auto_start <0|off|1|on>      : 0 = auto start off, 1 = auto start on\r\n");
    apollo_log_printf("           auto       <0|off|1|on>\r\n");
    apollo_log_printf("    config buffering_ms <xxx>           : xxx = milliseconds\r\n");
    apollo_log_printf("           buff_ms <xxx>                :       (range:%d <= xx <= %d)\r\n",
                                                                APOLLO_BUFFERING_MS_MIN, APOLLO_BUFFERING_MS_MAX);
    apollo_log_printf("    config clock <0|disable|1|enable>   : 0 = disable AS clock, 1 = enable\r\n");
    apollo_log_printf("    config pll   <0|disable|1|enable>   : 0 = disable audio PLL tuning, 1 = enable\r\n");
    apollo_log_printf("    config pll_tuning <0|disable|1|enable>\r\n");

    apollo_log_printf("    config mac_addr <xx:xx:xx:xx:xx:xx> : xx:xx:xx:xx:xx:xx = new MAC address\r\n");
    apollo_log_printf("    config mac <xx:xx:xx:xx:xx:xx>      : Shortcut:\r\n");
    apollo_log_printf("    config mac <xx>                     :   enter 1 octet to change last octet\r\n");

    apollo_log_printf("    config network_channel <xxx>        : xxx = channel\r\n");
    apollo_log_printf("           net_chan <xxx>               :  (1-11,36,40,44,48,52,56,60,64,\r\n");
    apollo_log_printf("                                        :   100,104,108,112,116,120,124,128,\r\n");
    apollo_log_printf("                                        :   132,136,140,149,153,157,161,165)\r\n");
    apollo_log_printf("    config network_name <ssid_name>     : name of AP (max %d characters)\r\n", SSID_NAME_SIZE);
    apollo_log_printf("           net_name <ssid_name>\r\n");
    apollo_log_printf("           ssid <ssid_name>\r\n");
    apollo_log_printf("    config network_passphrase <pass>    : passkey/password (max %d characters)\r\n", SECURITY_KEY_SIZE);
    apollo_log_printf("           net_pass <pass>\r\n");
    apollo_log_printf("           pass <pass>\r\n");
    apollo_log_printf("    config network_security <type>      : security type is one of: \r\n");
    apollo_log_printf("           net_sec <type>               :   open,none,ibss,wep,wepshared,wpatkip,wpaaes,\r\n");
    apollo_log_printf("                                        :   wpa2tkip,wpa2aes,wpa2mix,wpsopen,wpsaes\r\n");

    apollo_log_printf("    config speaker_name <name>          : speaker name (max %d characters)\r\n", APOLLO_SPEAKER_NAME_LENGTH);
    apollo_log_printf("           spkr_name <name>\r\n");
    apollo_log_printf("    config speaker_channel <ch> [ch]... : channel mix - all will be OR'ed together\r\n");
    apollo_log_printf("           spkr_chan <ch> [ch]...       :    FL,FR,FC,LFE1,BL,BR,FLC,FRC,BC,LFE2,\r\n");
    apollo_log_printf("                                        :    SIL,SIR,TPFL,TPFR,TPFC,TPC,TPBL,TPBR,\r\n");
    apollo_log_printf("                                        :    TPSIL,TPSIR,TPBC,BTFC,BTFL,BTFR\r\n");

    apollo_log_printf("    config threshold_ms <xx>            : xx = milliseconds\r\n");
    apollo_log_printf("           thresh_ms <xx>               :      (range:%d <= xx <= %d)\r\n",
                                                                APOLLO_THRESHOLD_MS_MIN, APOLLO_THRESHOLD_MS_MAX);

    apollo_log_printf("    config volume <xx>                  : xx = volume level\r\n");
    apollo_log_printf("           vol <xx>                     :      (range:%d <= xx <= %d)\r\n", APOLLO_VOLUME_MIN, APOLLO_VOLUME_MAX);

    apollo_log_printf("    config payload_size <size_in_bytes> : from %d to %d bytes\r\n", RTP_PACKET_MIN_DATA, RTP_PACKET_MAX_DATA);
    apollo_log_printf("    config fec_order    <string>        : \"pre\" = before audio, \"post\" = after audio\r\n" );
    apollo_log_printf("    config fec_length   <packet_count>  : from %d to %d\r\n", 0, RTP_AUDIO_FEC_MAX_LENGTH);
    apollo_log_printf("    config fec          <packet_count>\r\n" );

    apollo_log_printf("    config source_type  <string>        : \"bt\" = BT A2DP audio, \"capture\" = using local ADC\r\n");
    apollo_log_printf("    config src_t        <string> \r\n");

    apollo_log_printf("    config audio_device_rx <device_X>   : enter X as in WICED_AUDIO_X, X starts at 0 for all WICED platforms\r\n");
    apollo_log_printf("    config ad_rx           <device_X>\r\n");
    apollo_log_printf("    config audio_device_tx <device_X>   : enter X as in WICED_AUDIO_X, X starts at 0 for all WICED platforms\r\n");
    apollo_log_printf("    config ad_tx           <device_X>\r\n");
    apollo_log_printf("    config input_sample_rate <in_Hz>    : enter input sample rate in Hertz for the RX/capture audio device\r\n");
    apollo_log_printf("    config sr_rx             <in_Hz>\r\n");

    apollo_log_printf("    config apollo_role <source | sink>  : Configure as a source or a sink\r\n");
    apollo_log_printf("    config role <source | sink>\r\n");

    apollo_log_printf("    config clientaddr <IP address>      : Client IP address for sender to use\r\n");
    apollo_log_printf("    config addr <IP address>\r\n");

    apollo_log_printf("    config rtp_port <port number>       : RTP port number\r\n");
    apollo_log_printf("    config port <port number>\r\n");

    apollo_log_printf("    config log_level <level>            : Set the default application logging level\r\n");
    apollo_log_printf("    config log <level>\r\n");

    apollo_log_printf("    config is_configured <0|no|1|yes>   : Set to 0 to force BT GATT configuration\r\n");
    apollo_log_printf("    config is_conf       <0|no|1|yes>\r\n");

#ifdef WICED_DCT_INCLUDE_BT_CONFIG
    apollo_log_printf("    config bt_name <name>               : Set the bluetooth device name\r\n");
    apollo_log_printf("    config bt_dev <name>\r\n");
    apollo_log_printf("    config bt_mac <xx:xx:xx:xx:xx:xx>   : xx:xx:xx:xx:xx:xx = new Bluetooth MAC address\r\n");
    apollo_log_printf("    config bt_mac <xx>                  :   enter 1 octet to change last octet\r\n");
    apollo_log_printf("    config bt_class <xx:xx:xx>          : xx:xx:xx = new Bluetooth device class\r\n");
#endif

    apollo_log_printf("    config save                         : save data to flash NOTE: Changes not \r\n");
    apollo_log_printf("                                        :   automatically saved to flash!\r\n");
}

void apollo_set_config(apollo_dct_collection_t* dct_tables, int argc, char *argv[])
{
    int i, auto_start, ms, clock_enable, pll_enable;
    int volume;
    int port;
    int level;
    int is_configured;
    APOLLO_CHANNEL_MAP_T new_speaker_channel;
    APOLLO_LOG_LEVEL_T log_save;
    CONFIG_CMDS_T cmd;

    if (argc < 2)
    {
        apollo_config_print_info(dct_tables);
        return;
    }

    cmd = CONFIG_CMD_NONE;
    for (i = 0; i < (sizeof(config_command_lookup) / sizeof(config_command_lookup[0])); ++i)
    {
        if (strcasecmp(config_command_lookup[i].name, argv[1]) == 0)
        {
            cmd = config_command_lookup[i].value;
            break;
        }
    }

    /*
     * Make sure the notices about setting changes get displayed.
     */

    log_save = apollo_log_get_level();
    if (log_save < APOLLO_LOG_NOTICE)
    {
        apollo_log_set_level(APOLLO_LOG_NOTICE);
    }

    switch(cmd)
    {
        case CONFIG_CMD_HELP:
            apollo_config_command_help();
            break;

        case CONFIG_CMD_AUTO_START:
            auto_start = dct_tables->dct_app->auto_start;
            if (argc > 2)
            {
                if (strcasecmp(argv[2], "off") == 0)
                {
                    auto_start = 0;
                }
                else if (strcasecmp(argv[2], "on") == 0)
                {
                    auto_start = 1;
                }
                else
                {
                    auto_start = atoi(argv[2]);
                }
                auto_start = (auto_start == 0) ? 0 : 1;
                if (dct_tables->dct_app->auto_start != auto_start)
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "auto start: %d -> %d\r\n", dct_tables->dct_app->auto_start, auto_start);
                    dct_tables->dct_app->auto_start = auto_start;
                    app_dct_dirty |= APP_DCT_AUTO_START_DIRTY;
                }
            }
            apollo_log_msg(APOLLO_LOG_NOTICE, "Auto start: %d\r\n", dct_tables->dct_app->auto_start);
            break;

        case CONFIG_CMD_BUFFERING_MS:
            if (argc > 2)
            {
                ms = atoi(argv[2]);
                if ((ms < APOLLO_BUFFERING_MS_MIN) || (ms > APOLLO_BUFFERING_MS_MAX))
                {
                    apollo_log_msg(APOLLO_LOG_ERR, "Error: out of range min:%d  max:%d\r\n", APOLLO_BUFFERING_MS_MIN, APOLLO_BUFFERING_MS_MAX);
                    break;
                }
                ms = MAX(ms, APOLLO_BUFFERING_MS_MIN);
                ms = MIN(ms, APOLLO_BUFFERING_MS_MAX);
                if (dct_tables->dct_app->buffering_ms != ms)
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "buffering: %d -> %d (ms)\r\n", dct_tables->dct_app->buffering_ms, ms);
                    dct_tables->dct_app->buffering_ms = ms;
                    app_dct_dirty |= APP_DCT_BUFF_MS_DIRTY;
                }
            }
            apollo_log_msg(APOLLO_LOG_NOTICE, "Buffering: %d ms\r\n", dct_tables->dct_app->buffering_ms);
            break;

        case CONFIG_CMD_CLOCK_ENABLE:
            clock_enable = dct_tables->dct_app->clock_enable;
            if (argc > 2)
            {
                if (strcasecmp(argv[2], "disable") == 0)
                {
                    clock_enable = 0;
                }
                else if (strcasecmp(argv[2], "enable") == 0)
                {
                    clock_enable = 1;
                }
                else
                {
                    clock_enable = atoi(argv[2]);
                }
                clock_enable = (clock_enable == 0) ? 0 : 1;
                if (dct_tables->dct_app->clock_enable != clock_enable)
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "clock enable: %d -> %d\r\n", dct_tables->dct_app->clock_enable, clock_enable);
                    dct_tables->dct_app->clock_enable = clock_enable;
                    app_dct_dirty |= APP_DCT_CLOCK_ENABLE_DIRTY;
                }
            }
            apollo_log_msg(APOLLO_LOG_NOTICE, "Clock enable: %d\r\n", dct_tables->dct_app->clock_enable);
            break;

        case CONFIG_CMD_PLL_TUNING_ENABLE:
            pll_enable = dct_tables->dct_app->pll_tuning_enable;
            if (argc > 2)
            {
                if (strcasecmp(argv[2], "disable") == 0)
                {
                    pll_enable = 0;
                }
                else if (strcasecmp(argv[2], "enable") == 0)
                {
                    pll_enable = 1;
                }
                else
                {
                    pll_enable = atoi(argv[2]);
                }
                pll_enable = (pll_enable == 0) ? 0 : 1;
                if (dct_tables->dct_app->pll_tuning_enable != pll_enable)
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "audio PLL tuning enable: %d -> %d\r\n", dct_tables->dct_app->pll_tuning_enable, pll_enable);
                    dct_tables->dct_app->pll_tuning_enable = pll_enable;
                    app_dct_dirty |= APP_DCT_PLL_TUNING_ENABLE_DIRTY;
                }
            }
            apollo_log_msg(APOLLO_LOG_NOTICE, "Audio PLL tuning enable: %d\r\n", dct_tables->dct_app->pll_tuning_enable);
            break;

        case CONFIG_CMD_NETWORK_NAME:
        {
            char name[SSID_NAME_SIZE + 1];
            int  name_len;
            if (argc > 2)
            {
                if (strlen(argv[2]) > SSID_NAME_SIZE)
                {
                    apollo_log_msg(APOLLO_LOG_ERR, "Error: too long (max: %d)\r\n", SSID_NAME_SIZE);
                    break;
                }

                memset(name, 0, sizeof(name));
                memcpy(name, dct_tables->dct_wifi->stored_ap_list[0].details.SSID.value, dct_tables->dct_wifi->stored_ap_list[0].details.SSID.length);
                if (strcmp(name, argv[2])  != 0)
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "name: %s -> %s\r\n", name, argv[2]);
                    memset(dct_tables->dct_wifi->stored_ap_list[0].details.SSID.value, 0, sizeof(dct_tables->dct_wifi->stored_ap_list[0].details.SSID.value));
                    name_len = strlen(argv[2]);
                    if (name_len > sizeof(dct_tables->dct_wifi->stored_ap_list[0].details.SSID.value))
                    {
                        name_len = sizeof(dct_tables->dct_wifi->stored_ap_list[0].details.SSID.value);
                    }
                    memcpy(dct_tables->dct_wifi->stored_ap_list[0].details.SSID.value, argv[2], name_len);
                    dct_tables->dct_wifi->stored_ap_list[0].details.SSID.length = name_len;
                    wifi_dct_dirty |= WIFI_DCT_SSID_DIRTY;
                }
            }
            memset(name, 0, sizeof(name));
            memcpy(name, dct_tables->dct_wifi->stored_ap_list[0].details.SSID.value, dct_tables->dct_wifi->stored_ap_list[0].details.SSID.length);
            apollo_log_msg(APOLLO_LOG_NOTICE, "Network name: (%d) %.*s\r\n", dct_tables->dct_wifi->stored_ap_list[0].details.SSID.length,
                           dct_tables->dct_wifi->stored_ap_list[0].details.SSID.length, name);
            break;
        }

        case CONFIG_CMD_MAC_ADDR:
        {
            if (argc > 2)
            {
                wiced_mac_t new_mac;

                /* start with current DCT mac value to allow one-octet shortcut */
                memcpy(&new_mac, &dct_tables->dct_wifi->mac_address, sizeof(wiced_mac_t));
                if (apollo_mac_addr_parse(argv[2], new_mac.octet) != WICED_TRUE)
                {
                    apollo_log_msg(APOLLO_LOG_ERR, "Error: %s not a valid mac.\r\n", argv[2]);
                    break;
                }

                if (memcmp(&new_mac, &dct_tables->dct_wifi->mac_address, sizeof(wiced_mac_t)) != 0)
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "mac_addr: %02x:%02x:%02x:%02x:%02x:%02x -> %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                                   PRINT_MAC(dct_tables->dct_wifi->mac_address), PRINT_MAC(new_mac));
                    memcpy(&dct_tables->dct_wifi->mac_address, &new_mac, sizeof(wiced_mac_t));
                    wifi_dct_dirty |= WIFI_DCT_MAC_ADDR_DIRTY;
                    apollo_log_msg(APOLLO_LOG_NOTICE, "After save, you MUST reboot for MAC address change to take effect\r\n");
                }
            }
            apollo_log_msg(APOLLO_LOG_NOTICE, "MAC address: %02x:%02x:%02x:%02x:%02x:%02x\r\n", PRINT_MAC(dct_tables->dct_wifi->mac_address));

            {
                wiced_mac_t wiced_mac;
                wwd_wifi_get_mac_address(&wiced_mac, dct_tables->dct_network->interface);
                if (memcmp(&wiced_mac, &dct_tables->dct_wifi->mac_address, sizeof(wiced_mac_t) != 0))
                {
                    apollo_log_msg(APOLLO_LOG_NOTICE, "  WICED MAC: %02x:%02x:%02x:%02x:%02x:%02x\r\n", PRINT_MAC(wiced_mac));
                }
            }
            break;
        }

        case CONFIG_CMD_NETWORK_CHANNEL:
        {
            int new_channel, new_band;
            if (argc > 2)
            {
                new_channel = atoi(argv[2]);
                if (apollo_get_channel_band(new_channel, &new_band) != WICED_TRUE)
                {
                    /* TODO: get country code for output */
                    apollo_log_msg(APOLLO_LOG_ERR, "Error: %d not a valid channel.\r\n", new_channel);
                    break;
                }
                if ((dct_tables->dct_wifi->stored_ap_list[0].details.channel != new_channel) ||
                    (dct_tables->dct_wifi->stored_ap_list[0].details.band != new_band))
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "channel: %d -> %d\r\n  Band: %d -> %d \r\n",
                            dct_tables->dct_wifi->stored_ap_list[0].details.channel, new_channel,
                            dct_tables->dct_wifi->stored_ap_list[0].details.band, new_band);
                    dct_tables->dct_wifi->stored_ap_list[0].details.channel = new_channel;
                    dct_tables->dct_wifi->stored_ap_list[0].details.band = new_band;
                    wifi_dct_dirty |= WIFI_DCT_CHANNEL_DIRTY;
                }
            }
            apollo_log_msg(APOLLO_LOG_NOTICE, "Network channel:%d  band:%s\r\n", dct_tables->dct_wifi->stored_ap_list[0].details.channel,
                    (dct_tables->dct_wifi->stored_ap_list[0].details.band == WICED_802_11_BAND_2_4GHZ) ? "2.4GHz" : "5GHz");
            break;
        }

        case CONFIG_CMD_NETWORK_SECURITY_TYPE:
            if (argc > 2)
            {
                wiced_security_t new_sec_type;

                new_sec_type = apollo_security_type_parse(argv[2]);
                if (new_sec_type == WICED_SECURITY_UNKNOWN)
                {
                    apollo_log_msg(APOLLO_LOG_ERR, "Error: unknown security type: %s\r\n", (argv[2][0] != 0) ? argv[2] : "");
                    break;
                }
                if (dct_tables->dct_wifi->stored_ap_list[0].details.security != new_sec_type)
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "network security %s -> %s\r\n",
                            apollo_security_type_get_name(dct_tables->dct_wifi->stored_ap_list[0].details.security),
                            apollo_security_type_get_name(new_sec_type));
                    dct_tables->dct_wifi->stored_ap_list[0].details.security = new_sec_type;
                    wifi_dct_dirty |= WIFI_DCT_SECURITY_TYPE_DIRTY;
                }
            }
            apollo_log_msg(APOLLO_LOG_NOTICE, "Network security type: %s\r\n", apollo_security_type_get_name(dct_tables->dct_wifi->stored_ap_list[0].details.security));
            break;

        case CONFIG_CMD_NETWORK_PASSPHRASE:
        {
            char pass[SECURITY_KEY_SIZE + 1];
            if (argc > 2)
            {
                if (strlen(argv[2]) > SECURITY_KEY_SIZE)
                {
                    apollo_log_msg(APOLLO_LOG_ERR, "Error: too long (max %d)\r\n", SECURITY_KEY_SIZE);
                    break;
                }
                memset(pass, 0, sizeof(pass));
                memcpy(pass, dct_tables->dct_wifi->stored_ap_list[0].security_key, dct_tables->dct_wifi->stored_ap_list[0].security_key_length);
                if (strcmp(pass, argv[2]) != 0)
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "passphrase: %s -> %s \r\n", pass, argv[2]);
                    memset(pass, 0, sizeof(pass));
                    memcpy(pass, argv[2], SECURITY_KEY_SIZE);
                    memset(dct_tables->dct_wifi->stored_ap_list[0].security_key, 0, sizeof(dct_tables->dct_wifi->stored_ap_list[0].security_key));
                    memcpy(dct_tables->dct_wifi->stored_ap_list[0].security_key, argv[2], SECURITY_KEY_SIZE);
                    dct_tables->dct_wifi->stored_ap_list[0].security_key_length = strlen(pass);
                    wifi_dct_dirty |= WIFI_DCT_SECURITY_KEY_DIRTY;
                }
            }
            memset(pass, 0, sizeof(pass));
            memcpy(pass, dct_tables->dct_wifi->stored_ap_list[0].security_key, dct_tables->dct_wifi->stored_ap_list[0].security_key_length);
            apollo_log_msg(APOLLO_LOG_NOTICE, "Network passphrase: %s \r\n", pass);
            break;
        }

        case CONFIG_CMD_SPEAKER_NAME:
        {
            char name[APOLLO_SPEAKER_NAME_LENGTH + 1];
            if (argc > 2)
            {
                if (strlen(argv[2]) >= APOLLO_SPEAKER_NAME_LENGTH)
                {
                    apollo_log_msg(APOLLO_LOG_ERR, "Error: too long (max %d)\r\n", APOLLO_SPEAKER_NAME_LENGTH);
                    break;
                }
                memset(name, 0, sizeof(name));
                memcpy(name, dct_tables->dct_app->speaker_name, APOLLO_SPEAKER_NAME_LENGTH);
                if (strcmp(name, argv[2]) != 0)
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "Speaker name: %s -> %s\r\n", name, argv[2]);
                    memset(dct_tables->dct_app->speaker_name, 0, sizeof(dct_tables->dct_app->speaker_name));
                    strcpy(dct_tables->dct_app->speaker_name, argv[2]);
                    app_dct_dirty |= APP_DCT_SPEAKER_NAME_DIRTY;
                }
            }
            apollo_log_msg(APOLLO_LOG_NOTICE, "Speaker name: %s\r\n", dct_tables->dct_app->speaker_name);
            break;
        }

        case CONFIG_CMD_SPEAKER_CHANNEL:
        {
            char buff[10], buff2[10];
            if (argc > 2)
            {
                /* convert channel string to the bit fields */
                if (apollo_encode_speaker_channel(argc, argv, &new_speaker_channel) == WICED_TRUE)
                {
                    apollo_channel_name_get_text(dct_tables->dct_app->speaker_channel, buff, sizeof(buff));
                    apollo_channel_name_get_text(new_speaker_channel, buff2, sizeof(buff2));
                    if (dct_tables->dct_app->speaker_channel != new_speaker_channel)
                    {
                        apollo_log_msg(APOLLO_LOG_DEBUG0, "Speaker channel: (0x%08x) %s -> (0x%08x) %s\r\n",
                                       dct_tables->dct_app->speaker_channel, buff, new_speaker_channel, buff2);
                        dct_tables->dct_app->speaker_channel = new_speaker_channel;
                        app_dct_dirty |= APP_DCT_SPEAKER_CHANNEL_DIRTY;
                    }
                }
            }
            apollo_channel_name_get_text(dct_tables->dct_app->speaker_channel, buff, sizeof(buff));
            apollo_log_msg(APOLLO_LOG_NOTICE, "Speaker channel: (0x%08x) %s\r\n", dct_tables->dct_app->speaker_channel, buff);
            break;
        }

        case CONFIG_CMD_THRESHOLD_MS:
            if (argc > 2)
            {
                ms = atoi(argv[2]);
                /* validity check */
                if ((ms < APOLLO_THRESHOLD_MS_MIN) || (ms > APOLLO_THRESHOLD_MS_MAX))
                {
                    apollo_log_msg(APOLLO_LOG_ERR, "Error: out of range min: %d  max: %d\r\n", APOLLO_THRESHOLD_MS_MIN, APOLLO_THRESHOLD_MS_MAX);
                    break;
                }
                ms = MAX(ms, APOLLO_THRESHOLD_MS_MIN);
                ms = MIN(ms, APOLLO_THRESHOLD_MS_MAX);
                if (dct_tables->dct_app->threshold_ms != ms)
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "CONFIG_CMD_THRESHOLD_MS: %d -> %d\r\n", dct_tables->dct_app->threshold_ms, ms);
                    dct_tables->dct_app->threshold_ms = ms;
                    app_dct_dirty |= APP_DCT_THRESH_MS_DIRTY;
                }
            }
            apollo_log_msg(APOLLO_LOG_NOTICE, "Threshold: %d ms\r\n", dct_tables->dct_app->threshold_ms);
            break;

        case CONFIG_CMD_VOLUME:
            if (argc > 2)
            {
                volume = atoi(argv[2]);
                /* validity check */
                if (volume < APOLLO_VOLUME_MIN || volume > APOLLO_VOLUME_MAX)
                {
                    apollo_log_msg(APOLLO_LOG_ERR, "Error: out of range min: %d  max: %d\r\n", APOLLO_VOLUME_MIN, APOLLO_VOLUME_MAX);
                    break;
                }
                if (dct_tables->dct_app->volume != volume)
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "CONFIG_CMD_VOLUME: %d -> %d\r\n", dct_tables->dct_app->volume, volume);
                    dct_tables->dct_app->volume = volume;
                    app_dct_dirty |= APP_DCT_VOLUME_DIRTY;
                }
            }
            apollo_log_msg(APOLLO_LOG_NOTICE, "Volume: %d\r\n", dct_tables->dct_app->volume);
            break;

        case CONFIG_CMD_PAYLOAD_SIZE:
        {
            int payload_size = dct_tables->dct_app->payload_size;
            if (argc > 2)
            {
                payload_size = atoi(argv[2]);
                if ((payload_size >= RTP_PACKET_MIN_DATA) && (payload_size <= RTP_PACKET_MAX_DATA) && (dct_tables->dct_app->payload_size != payload_size))
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "Payload size: %d -> %d\r\n", dct_tables->dct_app->payload_size, payload_size);
                    dct_tables->dct_app->payload_size = payload_size;
                    app_dct_dirty |= APP_DCT_PAYLOAD_SIZE_DIRTY;
                }
            }
            apollo_log_msg(APOLLO_LOG_NOTICE, "Payload size: %d\r\n", dct_tables->dct_app->payload_size);
        }
            break;

        case CONFIG_CMD_FEC_ORDER:
        {
            wiced_result_t result = WICED_ERROR;
            uint32_t fec_order = dct_tables->dct_app->fec_order;
            if (argc > 2)
            {
                result = apollo_fec_order_get(argv[2], &fec_order);
                if ((result == WICED_SUCCESS) && (fec_order != dct_tables->dct_app->fec_order))
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "FEC order: 0x%lx -> 0x%lx\r\n", dct_tables->dct_app->fec_order, fec_order);
                    dct_tables->dct_app->fec_order = fec_order;
                    app_dct_dirty |= APP_DCT_FEC_ORDER_DIRTY;
                }
            }
            apollo_log_msg(APOLLO_LOG_NOTICE, "FEC order: %s\r\n", apollo_fec_order_get_text(dct_tables->dct_app->fec_order));
            break;
        }

        case CONFIG_CMD_FEC_LENGTH:
        {
            int fec_length = dct_tables->dct_app->fec_length;
            if (argc > 2)
            {
                fec_length = atoi(argv[2]);
                if ((fec_length >= 0) && (fec_length <= RTP_AUDIO_FEC_MAX_LENGTH) && (dct_tables->dct_app->fec_length != fec_length))
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "FEC length: %d -> %d\r\n", dct_tables->dct_app->fec_length, fec_length);
                    dct_tables->dct_app->fec_length = fec_length;
                    app_dct_dirty |= APP_DCT_FEC_LENGTH_DIRTY;
                }
            }
            apollo_log_msg(APOLLO_LOG_NOTICE, "FEC length: %d\r\n", dct_tables->dct_app->fec_length);
            break;
        }

        case CONFIG_CMD_SOURCE_TYPE:
        {
            wiced_result_t result = WICED_ERROR;
            uint32_t source_type = dct_tables->dct_app->source_type;
            if (argc > 2)
            {
                result = apollo_source_type_get(argv[2], &source_type);
                if ((result == WICED_SUCCESS) && (source_type != dct_tables->dct_app->source_type))
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "Audio source type: %d -> %d\r\n", dct_tables->dct_app->source_type, source_type);
                    dct_tables->dct_app->source_type = source_type;
                    app_dct_dirty |= APP_DCT_SOURCE_TYPE_DIRTY;
                }
            }
            apollo_log_msg(APOLLO_LOG_NOTICE, "Audio source type: %s\r\n", apollo_source_type_get_text(dct_tables->dct_app->source_type));
            break;
        }

        case CONFIG_CMD_AUDIO_DEVICE_RX:
        {
            platform_audio_device_id_t device = dct_tables->dct_app->audio_device_rx;
            if (argc > 2)
            {
                int device_index = atoi(argv[2]);
                if ((device_index >= 0) && (device_index < PLATFORM_AUDIO_NUM_INPUTS))
                {
                    device = platform_audio_input_devices[device_index].device_id;
                    if (device != dct_tables->dct_app->audio_device_rx)
                    {
                        apollo_log_msg(APOLLO_LOG_NOTICE, "Audio device RX ID: 0x%x -> 0x%x\r\n", dct_tables->dct_app->audio_device_rx,
                                                           device, platform_audio_input_devices[device_index].device_name);
                        dct_tables->dct_app->audio_device_rx = device;
                        app_dct_dirty |= APP_DCT_AUDIO_DEVICE_RX_DIRTY;
                    }
                }
            }
            platform_audio_print_device_list(dct_tables->dct_app->audio_device_rx, (app_dct_dirty & APP_DCT_AUDIO_DEVICE_RX_DIRTY),
                                             AUDIO_DEVICE_ID_NONE, 0, 1);
            break;
        }

        case CONFIG_CMD_AUDIO_DEVICE_TX:
        {
            platform_audio_device_id_t device = dct_tables->dct_app->audio_device_tx;
            if (argc > 2)
            {
                int device_index = atoi(argv[2]);
                if ((device_index >= 0) && (device_index < PLATFORM_AUDIO_NUM_OUTPUTS))
                {
                    device = platform_audio_output_devices[device_index].device_id;
                    if (device != dct_tables->dct_app->audio_device_tx)
                    {
                        apollo_log_msg(APOLLO_LOG_NOTICE, "Audio device TX ID: 0x%x -> 0x%x %s\r\n", dct_tables->dct_app->audio_device_tx,
                                                           device, platform_audio_output_devices[device_index].device_name);
                        dct_tables->dct_app->audio_device_tx = device;
                        app_dct_dirty |= APP_DCT_AUDIO_DEVICE_TX_DIRTY;
                    }
                }
            }
            platform_audio_print_device_list(AUDIO_DEVICE_ID_NONE, WICED_FALSE,
                                             dct_tables->dct_app->audio_device_tx, (app_dct_dirty & APP_DCT_AUDIO_DEVICE_RX_DIRTY),
                                             1);
            break;
        }

        case CONFIG_CMD_INPUT_SAMPLE_RATE:
        {
            platform_audio_sample_rates_t sample_rate = dct_tables->dct_app->input_sample_rate;
            if (argc > 2)
            {
                sample_rate = apollo_sample_rate_type_get(atoi(argv[2]));
                if ( (sample_rate > 0) && (sample_rate != dct_tables->dct_app->input_sample_rate) )
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "Input sample rate: %s KHz-> %s KHz\r\n",
                                   platform_audio_device_get_sample_rates_string(dct_tables->dct_app->input_sample_rate),
                                   platform_audio_device_get_sample_rates_string(sample_rate));
                    dct_tables->dct_app->input_sample_rate = sample_rate;
                    app_dct_dirty |= APP_DCT_INPUT_SAMPLE_RATE_DIRTY;
                }
            }
            apollo_log_msg(APOLLO_LOG_NOTICE, "Input sample rate: %s KHz\r\n", platform_audio_device_get_sample_rates_string(dct_tables->dct_app->input_sample_rate));
            break;
        }

        case CONFIG_CMD_APOLLO_ROLE:
        {
            wiced_result_t result = WICED_ERROR;
            uint32_t apollo_role = dct_tables->dct_app->apollo_role;
            if (argc > 2)
            {
                result = apollo_role_type_get(argv[2], &apollo_role);
                if (result == WICED_SUCCESS && (apollo_role != dct_tables->dct_app->apollo_role))
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "Apollo role: %d -> %d\r\n", dct_tables->dct_app->apollo_role, apollo_role);
                    dct_tables->dct_app->apollo_role = apollo_role;
                    app_dct_dirty |= APP_DCT_APOLLO_ROLE_DIRTY;
                }
            }
            apollo_log_msg(APOLLO_LOG_NOTICE, "Apollo role type: %s\r\n", apollo_role_type_get_text(dct_tables->dct_app->apollo_role));
            break;
        }

        case CONFIG_CMD_CLIENTADDR:
        {
            wiced_ip_address_t address;

            if (argc > 2)
            {
                if (str_to_ip(argv[2], &address) == 0 && dct_tables->dct_app->clientaddr.ip.v4 != address.ip.v4)
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "Apollo client IP address: %d.%d.%d.%d -> %d.%d.%d.%d\r\n",
                            (dct_tables->dct_app->clientaddr.ip.v4 >> 24) & 0xFF, (dct_tables->dct_app->clientaddr.ip.v4 >> 16) & 0xFF,
                            (dct_tables->dct_app->clientaddr.ip.v4 >> 8) & 0xFF, dct_tables->dct_app->clientaddr.ip.v4 & 0xFF,
                            (address.ip.v4 > 24) & 0xFF, (address.ip.v4 > 16) & 0xFF, (address.ip.v4 > 8) & 0xFF, address.ip.v4 & 0xFF);
                    dct_tables->dct_app->clientaddr.ip.v4 = address.ip.v4;
                    app_dct_dirty |= APP_DCT_CLIENTADDR_DIRTY;
                }
            }
            apollo_log_msg(APOLLO_LOG_NOTICE, "Apollo client IP address: %d.%d.%d.%d\r\n",
                           (dct_tables->dct_app->clientaddr.ip.v4 >> 24) & 0xFF, (dct_tables->dct_app->clientaddr.ip.v4 >> 16) & 0xFF,
                           (dct_tables->dct_app->clientaddr.ip.v4 >> 8) & 0xFF, dct_tables->dct_app->clientaddr.ip.v4 & 0xFF);
            break;
        }

        case CONFIG_CMD_RTP_PORT:
            if (argc > 2)
            {
                port = atoi(argv[2]);
                /* validity check */
                if (port < 0)
                {
                    apollo_log_msg(APOLLO_LOG_ERR, "Error: port %d invalid\r\n", port);
                    break;
                }
                if (dct_tables->dct_app->rtp_port != port)
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "rtp_port: %d -> %d\r\n", dct_tables->dct_app->rtp_port, port);
                    dct_tables->dct_app->rtp_port = port;
                    app_dct_dirty |= APP_DCT_RTP_PORT_DIRTY;
                }
            }
            apollo_log_msg(APOLLO_LOG_NOTICE, "RTP port: %d\r\n", dct_tables->dct_app->rtp_port);
            break;

        case CONFIG_CMD_LOG_LEVEL:
            if (argc > 2)
            {
                level = atoi(argv[2]);
                /* validity check */
                if (level < 0)
                {
                    apollo_log_msg(APOLLO_LOG_ERR, "Error: log level %d invalid\r\n", level);
                    break;
                }
                if (dct_tables->dct_app->log_level != level)
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "log_level: %d -> %d\r\n", dct_tables->dct_app->log_level, level);
                    dct_tables->dct_app->log_level = level;
                    app_dct_dirty |= APP_DCT_LOG_LEVEL_DIRTY;
                }
            }
            apollo_log_msg(APOLLO_LOG_NOTICE, "Log level: %d\r\n", dct_tables->dct_app->log_level);
            break;

        case CONFIG_CMD_IS_CONFIGURED:
            is_configured = dct_tables->dct_app->is_configured;
            if (argc > 2)
            {
                if (strcasecmp(argv[2], "no") == 0)
                {
                    is_configured = 0;
                }
                else if (strcasecmp(argv[2], "yes") == 0)
                {
                    is_configured = 1;
                }
                else
                {
                    is_configured = atoi(argv[2]);
                }
                is_configured = (is_configured == 0) ? 0 : 1;
                if (dct_tables->dct_app->is_configured != is_configured)
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "is configured: %d -> %d\r\n", dct_tables->dct_app->is_configured, is_configured);
                    dct_tables->dct_app->is_configured = is_configured;
                    app_dct_dirty |= APP_DCT_IS_CONFIGURED_DIRTY;
                }
            }
            apollo_log_msg(APOLLO_LOG_NOTICE, "Is configured: %d\r\n", dct_tables->dct_app->is_configured);
            break;

#ifdef WICED_DCT_INCLUDE_BT_CONFIG
        case CONFIG_CMD_BT_DEVICE_NAME:
        {
            int namelen = sizeof(dct_tables->dct_bt->bluetooth_device_name);

            if (argc > 2)
            {
                if (strlen(argv[2]) >= namelen)
                {
                    apollo_log_msg(APOLLO_LOG_ERR, "Error: too long (max %d)\r\n", namelen);
                    break;
                }
                if (strcmp((const char *)dct_tables->dct_bt->bluetooth_device_name, argv[2]) != 0)
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "BT device name: %s -> %s\r\n", dct_tables->dct_bt->bluetooth_device_name, argv[2]);
                    memset(dct_tables->dct_bt->bluetooth_device_name, 0, namelen);
                    strcpy((char *)dct_tables->dct_bt->bluetooth_device_name, argv[2]);
                    bt_dct_dirty |= BT_DCT_DEV_NAME_DIRTY;
                }
            }
            apollo_log_msg(APOLLO_LOG_NOTICE, "BT device name: %s\r\n", dct_tables->dct_bt->bluetooth_device_name);
            break;
        }

        case CONFIG_CMD_BT_DEVICE_MAC_ADDR:
            if (argc > 2)
            {
                uint8_t new_addr[6];

                /* start with current DCT mac value to allow one-octet shortcut */
                memcpy(new_addr, dct_tables->dct_bt->bluetooth_device_address, sizeof(new_addr));
                if (apollo_mac_addr_parse(argv[2], new_addr) != WICED_TRUE)
                {
                    apollo_log_msg(APOLLO_LOG_ERR, "Error: %s not a valid mac.\r\n", argv[2]);
                    break;
                }

                if (memcmp(new_addr, dct_tables->dct_bt->bluetooth_device_address, sizeof(new_addr)) != 0)
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "bt_mac_addr: %02x:%02x:%02x:%02x:%02x:%02x -> %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                            dct_tables->dct_bt->bluetooth_device_address[0], dct_tables->dct_bt->bluetooth_device_address[1],
                            dct_tables->dct_bt->bluetooth_device_address[2], dct_tables->dct_bt->bluetooth_device_address[3],
                            dct_tables->dct_bt->bluetooth_device_address[4], dct_tables->dct_bt->bluetooth_device_address[5],
                            new_addr[0], new_addr[1], new_addr[2], new_addr[3], new_addr[4], new_addr[5]);
                    memcpy(dct_tables->dct_bt->bluetooth_device_address, new_addr, sizeof(new_addr));
                    bt_dct_dirty |= BT_DCT_MAC_ADDR_DIRTY;
                    apollo_log_msg(APOLLO_LOG_NOTICE, "After save, you MUST reboot for BT MAC address change to take effect\r\n");
                }
            }
            apollo_log_msg(APOLLO_LOG_NOTICE, "BT MAC address: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                    dct_tables->dct_bt->bluetooth_device_address[0], dct_tables->dct_bt->bluetooth_device_address[1],
                    dct_tables->dct_bt->bluetooth_device_address[2], dct_tables->dct_bt->bluetooth_device_address[3],
                    dct_tables->dct_bt->bluetooth_device_address[4], dct_tables->dct_bt->bluetooth_device_address[5]);
            break;

        case CONFIG_CMD_BT_DEVICE_CLASS:
            if (argc > 2)
            {
                uint8_t new_class[3];

                /* start with current DCT mac value to allow one-octet shortcut */
                memcpy(new_class, dct_tables->dct_bt->bluetooth_device_address, sizeof(new_class));
                if (apollo_bt_device_class_parse(argv[2], new_class) != WICED_TRUE)
                {
                    apollo_log_msg(APOLLO_LOG_ERR, "Error: %s not a valid class.\r\n", argv[2]);
                    break;
                }

                if (memcmp(new_class, dct_tables->dct_bt->bluetooth_device_address, sizeof(new_class)) != 0)
                {
                    apollo_log_msg(APOLLO_LOG_DEBUG0, "bt_mac_addr: %02x:%02x:%02x -> %02x:%02x:%02x\r\n",
                            dct_tables->dct_bt->bluetooth_device_class[0], dct_tables->dct_bt->bluetooth_device_class[1],
                            dct_tables->dct_bt->bluetooth_device_class[2], new_class[0], new_class[1], new_class[2]);
                    memcpy(dct_tables->dct_bt->bluetooth_device_address, new_class, sizeof(new_class));
                    bt_dct_dirty |= BT_DCT_DEV_CLASS_DIRTY;
                }
            }
            apollo_log_msg(APOLLO_LOG_NOTICE, "BT MAC address: %02x:%02x:%02x\r\n", dct_tables->dct_bt->bluetooth_device_class[0],
                    dct_tables->dct_bt->bluetooth_device_class[1], dct_tables->dct_bt->bluetooth_device_class[2]);
            break;
#endif

        case CONFIG_CMD_SAVE:
            if (app_dct_dirty != 0)
            {
                apollo_log_msg(APOLLO_LOG_NOTICE, "Saving App DCT:\r\n");
                apollo_save_app_dct(dct_tables);
                app_dct_dirty = 0;
                apollo_print_app_info(dct_tables);
            }
            if (net_dct_dirty != 0)
            {
                apollo_log_msg(APOLLO_LOG_NOTICE, "Saving Network DCT:\r\n");
                apollo_save_network_dct(dct_tables);
                net_dct_dirty = 0;
                apollo_print_network_info(dct_tables);
            }
            if (wifi_dct_dirty != 0)
            {
                apollo_log_msg(APOLLO_LOG_NOTICE, "Saving WiFi DCT:\r\n");
                apollo_save_wifi_dct(dct_tables);
                if ((wifi_dct_dirty & WIFI_DCT_MAC_ADDR_DIRTY) != 0)
                {
                    apollo_log_msg(APOLLO_LOG_NOTICE, "You MUST reboot for MAC address change to take effect\r\n");
                }
                wifi_dct_dirty = 0;
                apollo_print_wifi_info(dct_tables);
            }
#ifdef WICED_DCT_INCLUDE_BT_CONFIG
            if (bt_dct_dirty != 0)
            {
                apollo_log_msg(APOLLO_LOG_NOTICE, "Saving BT DCT:\r\n");
                apollo_save_bt_dct(dct_tables);
                if ((bt_dct_dirty & BT_DCT_MAC_ADDR_DIRTY) != 0)
                {
                    apollo_log_msg(APOLLO_LOG_NOTICE, "You MUST reboot for BT MAC address change to take effect\r\n");
                }
                bt_dct_dirty = 0;
                apollo_print_bt_info(dct_tables);
            }
#endif
            break;

        default:
            apollo_log_msg(APOLLO_LOG_ERR, "Unrecognized config command: %s\r\n", (argv[1][0] != 0) ? argv[1] : "");
            apollo_config_command_help();
            break;
    }

    /*
     * Restore the original log level.
     */

    apollo_log_set_level(log_save);
}

static void apollo_print_app_info(apollo_dct_collection_t* dct_tables)
{
    char buf[20];

    apollo_channel_name_get_text(dct_tables->dct_app->speaker_channel, buf, sizeof(buf));

    apollo_log_printf("  Apollo app DCT:\r\n");
    apollo_log_printf("     is configured: %s %c\r\n", dct_tables->dct_app->is_configured == 0 ? "no" : "yes", DIRTY_CHECK(app_dct_dirty, APP_DCT_IS_CONFIGURED_DIRTY));
    apollo_log_printf("       apollo role: %s %c\r\n", apollo_role_type_get_text(dct_tables->dct_app->apollo_role), DIRTY_CHECK(app_dct_dirty, APP_DCT_APOLLO_ROLE_DIRTY));
    apollo_log_printf("      speaker name: %s %c\r\n", dct_tables->dct_app->speaker_name, DIRTY_CHECK(app_dct_dirty, APP_DCT_SPEAKER_NAME_DIRTY));
    apollo_log_printf("           channel: (0x%08x) %s %c\r\n", dct_tables->dct_app->speaker_channel, buf, DIRTY_CHECK(app_dct_dirty, APP_DCT_SPEAKER_CHANNEL_DIRTY));
    apollo_log_printf("         buffering: %d ms %c\r\n", dct_tables->dct_app->buffering_ms, DIRTY_CHECK(app_dct_dirty, APP_DCT_BUFF_MS_DIRTY));
    apollo_log_printf("         threshold: %d ms %c\r\n", dct_tables->dct_app->threshold_ms, DIRTY_CHECK(app_dct_dirty, APP_DCT_THRESH_MS_DIRTY));
    apollo_log_printf("        auto_start: %s %c\r\n", dct_tables->dct_app->auto_start == 0 ? "off" : "on", DIRTY_CHECK(app_dct_dirty, APP_DCT_AUTO_START_DIRTY));
    apollo_log_printf("      clock enable: %s %c\r\n", dct_tables->dct_app->clock_enable == 0 ? "disable" : "enable", DIRTY_CHECK(app_dct_dirty, APP_DCT_CLOCK_ENABLE_DIRTY));
    apollo_log_printf(" PLL tuning enable: %s %c\r\n", dct_tables->dct_app->pll_tuning_enable == 0 ? "disable" : "enable", DIRTY_CHECK(app_dct_dirty, APP_DCT_PLL_TUNING_ENABLE_DIRTY));
    apollo_log_printf("            volume: %d %c\r\n", dct_tables->dct_app->volume, DIRTY_CHECK(app_dct_dirty, APP_DCT_VOLUME_DIRTY));
    apollo_log_printf("       source type: %s %c\r\n", apollo_source_type_get_text(dct_tables->dct_app->source_type), DIRTY_CHECK(app_dct_dirty, APP_DCT_SOURCE_TYPE_DIRTY));
    apollo_log_printf("      payload size: %d %c\r\n", dct_tables->dct_app->payload_size, DIRTY_CHECK(app_dct_dirty, APP_DCT_PAYLOAD_SIZE_DIRTY));
    apollo_log_printf("        FEC  order: %s %c\r\n", apollo_fec_order_get_text(dct_tables->dct_app->fec_order), DIRTY_CHECK(app_dct_dirty, APP_DCT_FEC_ORDER_DIRTY));
    apollo_log_printf("        FEC length: %d %c\r\n", dct_tables->dct_app->fec_length, DIRTY_CHECK(app_dct_dirty, APP_DCT_FEC_LENGTH_DIRTY));
    platform_audio_print_device_list(dct_tables->dct_app->audio_device_rx, (app_dct_dirty & APP_DCT_AUDIO_DEVICE_RX_DIRTY),
                                     dct_tables->dct_app->audio_device_tx, (app_dct_dirty & APP_DCT_AUDIO_DEVICE_TX_DIRTY), 0);
    apollo_log_printf(" Input sample rate: %s KHz %c\r\n", platform_audio_device_get_sample_rates_string(dct_tables->dct_app->input_sample_rate),
            DIRTY_CHECK(app_dct_dirty, APP_DCT_INPUT_SAMPLE_RATE_DIRTY));
    apollo_log_printf(" client IP address: %d.%d.%d.%d %c\r\n",
            (dct_tables->dct_app->clientaddr.ip.v4 >> 24) & 0xFF, (dct_tables->dct_app->clientaddr.ip.v4 >> 16) & 0xFF,
            (dct_tables->dct_app->clientaddr.ip.v4 >> 8) & 0xFF, dct_tables->dct_app->clientaddr.ip.v4 & 0xFF, DIRTY_CHECK(app_dct_dirty, APP_DCT_CLIENTADDR_DIRTY));
    apollo_log_printf("          RTP port: %d %c\r\n", dct_tables->dct_app->rtp_port, DIRTY_CHECK(app_dct_dirty, APP_DCT_RTP_PORT_DIRTY));
    apollo_log_printf("         log level: %d (currently %d) %c\r\n", dct_tables->dct_app->log_level, apollo_log_get_level(), DIRTY_CHECK(app_dct_dirty, APP_DCT_LOG_LEVEL_DIRTY));
}

static void apollo_print_network_info(apollo_dct_collection_t* dct_tables)
{
    wiced_ip_address_t ip_addr;
    int32_t rssi;

    apollo_log_printf("  Network DCT:\r\n");
    apollo_log_printf("         Interface: %s\r\n",
           (dct_tables->dct_network->interface == (wiced_interface_t)WWD_STA_INTERFACE)      ? "STA" :
           (dct_tables->dct_network->interface == (wiced_interface_t)WWD_AP_INTERFACE)       ? "AP" :
           (dct_tables->dct_network->interface == (wiced_interface_t)WWD_ETHERNET_INTERFACE) ? "Ethernet" :
           "Unknown");
    wiced_ip_get_ipv4_address(dct_tables->dct_network->interface, &ip_addr);
    apollo_log_printf("           IP addr: %d.%d.%d.%d\r\n",
           (int)((ip_addr.ip.v4 >> 24) & 0xFF), (int)((ip_addr.ip.v4 >> 16) & 0xFF),
           (int)((ip_addr.ip.v4 >> 8) & 0xFF),  (int)(ip_addr.ip.v4 & 0xFF));
    wwd_wifi_get_rssi(&rssi);
    apollo_log_printf("              RSSI: %d dBm\r\n", rssi);
}

static void apollo_print_wifi_info(apollo_dct_collection_t* dct_tables)
{
    wiced_security_t sec;
    uint32_t channel;
    int band;
    wiced_mac_t wiced_mac;

    wwd_wifi_get_mac_address(&wiced_mac, WICED_STA_INTERFACE);

    if (dct_tables->dct_network->interface == WICED_STA_INTERFACE)
    {
        sec = dct_tables->dct_wifi->stored_ap_list[0].details.security;
        apollo_log_printf("  WiFi DCT:\r\n");

        apollo_log_printf("   WICED MAC (STA): %02x:%02x:%02x:%02x:%02x:%02x\r\n", PRINT_MAC(wiced_mac));

        apollo_log_printf("               MAC: %02x:%02x:%02x:%02x:%02x:%02x %c\r\n", PRINT_MAC(dct_tables->dct_wifi->mac_address),
                                                        DIRTY_CHECK(wifi_dct_dirty, WIFI_DCT_MAC_ADDR_DIRTY));
        apollo_log_printf("              SSID: %.*s %c\r\n", dct_tables->dct_wifi->stored_ap_list[0].details.SSID.length,
                                                        dct_tables->dct_wifi->stored_ap_list[0].details.SSID.value,
                                                        DIRTY_CHECK(wifi_dct_dirty, WIFI_DCT_SSID_DIRTY));
        apollo_log_printf("          Security: %s %c\r\n", apollo_security_type_get_name(sec), DIRTY_CHECK(wifi_dct_dirty, WIFI_DCT_SECURITY_TYPE_DIRTY));

        if (dct_tables->dct_wifi->stored_ap_list[0].details.security != WICED_SECURITY_OPEN)
        {
            apollo_log_printf("        Passphrase: %.*s %c\r\n", dct_tables->dct_wifi->stored_ap_list[0].security_key_length,
                    dct_tables->dct_wifi->stored_ap_list[0].security_key, DIRTY_CHECK(wifi_dct_dirty, WIFI_DCT_SECURITY_KEY_DIRTY));
        }
        else
        {
            apollo_log_printf("      Passphrase: none\r\n");
        }

        channel = dct_tables->dct_wifi->stored_ap_list[0].details.channel;
        band    = dct_tables->dct_wifi->stored_ap_list[0].details.band;
        apollo_log_printf("           Channel: %d %c\r\n", (int)channel, DIRTY_CHECK(wifi_dct_dirty, WIFI_DCT_CHANNEL_DIRTY));
        apollo_log_printf("              Band: %s %c\r\n", (band == WICED_802_11_BAND_2_4GHZ) ? "2.4GHz" : "5GHz",
                                                             DIRTY_CHECK(wifi_dct_dirty, WIFI_DCT_CHANNEL_DIRTY));
    }
    else
    {
        /*
         * Nothing for AP interface yet.
         */
    }
}


#ifdef WICED_DCT_INCLUDE_BT_CONFIG
static void apollo_print_bt_info(apollo_dct_collection_t* dct_tables)
{
    apollo_log_printf("  BT DCT:\r\n");

    apollo_log_printf("    BT device name: %s %c\r\n", dct_tables->dct_bt->bluetooth_device_name,
                      DIRTY_CHECK(bt_dct_dirty, BT_DCT_DEV_NAME_DIRTY));
    apollo_log_printf("            BT MAC: %02x:%02x:%02x:%02x:%02x:%02x %c\r\n", dct_tables->dct_bt->bluetooth_device_address[0],
                      dct_tables->dct_bt->bluetooth_device_address[1], dct_tables->dct_bt->bluetooth_device_address[2],
                      dct_tables->dct_bt->bluetooth_device_address[3], dct_tables->dct_bt->bluetooth_device_address[4],
                      dct_tables->dct_bt->bluetooth_device_address[5], DIRTY_CHECK(bt_dct_dirty, BT_DCT_MAC_ADDR_DIRTY));
    apollo_log_printf("   BT device class: %02x:%02x:%02x %c\r\n", dct_tables->dct_bt->bluetooth_device_class[0],
                      dct_tables->dct_bt->bluetooth_device_class[1], dct_tables->dct_bt->bluetooth_device_class[2],
                      DIRTY_CHECK(bt_dct_dirty, BT_DCT_DEV_CLASS_DIRTY));
}
#endif


static void apollo_print_current_info(apollo_dct_collection_t* dct_tables)
{
    uint32_t channel;
    wiced_wifi_get_channel(&channel);
    apollo_log_printf(" Current:\r\n");
    apollo_log_printf("           Channel: %lu\r\n              Band: %s\r\n",
                      channel, channel <= 13 ? "2.4GHz" : "5GHz");
}

void apollo_config_print_info(apollo_dct_collection_t* dct_tables)
{
    apollo_log_printf("\r\nConfig Info: * = dirty\r\n");
    apollo_print_app_info(dct_tables);
    apollo_print_network_info(dct_tables);
    apollo_print_wifi_info(dct_tables);
#ifdef WICED_DCT_INCLUDE_BT_CONFIG
    apollo_print_bt_info(dct_tables);
#endif
    apollo_print_current_info(dct_tables);
    apollo_log_printf("\r\n");
}

wiced_result_t apollo_config_init(apollo_dct_collection_t* dct_tables)
{
    wiced_result_t result;

    /* Get network configuration */
    result = wiced_dct_read_lock((void **)&dct_tables->dct_network, WICED_TRUE, DCT_NETWORK_CONFIG_SECTION, 0, sizeof(platform_dct_network_config_t));
    if (result != WICED_SUCCESS || (dct_tables->dct_network->interface != WICED_STA_INTERFACE && dct_tables->dct_network->interface != WICED_AP_INTERFACE))
    {
        apollo_log_msg(APOLLO_LOG_ERR, "Can't get network configuration!\r\n");
    }

    /* Get WiFi configuration */
    result |= wiced_dct_read_lock((void **)&dct_tables->dct_wifi, WICED_TRUE, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t));
    if (result != WICED_SUCCESS || dct_tables->dct_wifi->device_configured != WICED_TRUE)
    {
        apollo_log_msg(APOLLO_LOG_ERR, "Can't get WiFi configuration!\r\n");
    }

#ifdef WICED_DCT_INCLUDE_BT_CONFIG
    /* Get BT configuration */
    result |= wiced_dct_read_lock((void **)&dct_tables->dct_bt, WICED_TRUE, DCT_BT_CONFIG_SECTION, 0, sizeof(platform_dct_bt_config_t));
    if (result != WICED_SUCCESS)
    {
        apollo_log_msg(APOLLO_LOG_ERR, "Can't get BT configuration!\r\n");
    }
#endif

    /* Get app specific data from Non Volatile DCT */
    result |= wiced_dct_read_lock((void **)&dct_tables->dct_app, WICED_TRUE, DCT_APP_SECTION, 0, sizeof(apollo_dct_t));
    if (result != WICED_SUCCESS)
    {
        apollo_log_msg(APOLLO_LOG_ERR, "Can't get app configuration!\r\n");
    }

    return result;
}

wiced_result_t apollo_config_deinit(apollo_dct_collection_t* dct_tables)
{
    wiced_result_t result = WICED_SUCCESS;

    if (dct_tables->dct_network != NULL)
    {
        result = wiced_dct_read_unlock(dct_tables->dct_network, WICED_TRUE);
        if (result == WICED_SUCCESS)
        {
            dct_tables->dct_network = NULL;
        }
        else
        {
            apollo_log_msg(APOLLO_LOG_ERR, "Can't free/release network configuration !\r\n");
        }
    }

    if (dct_tables->dct_wifi != NULL)
    {
        result |= wiced_dct_read_unlock(dct_tables->dct_wifi, WICED_TRUE);
        if (result == WICED_SUCCESS)
        {
            dct_tables->dct_wifi = NULL;
        }
        else
        {
            apollo_log_msg(APOLLO_LOG_ERR, "Can't free/release WiFi configuration !\r\n");
        }
    }

#ifdef WICED_DCT_INCLUDE_BT_CONFIG
    if (dct_tables->dct_bt != NULL)
    {
        result |= wiced_dct_read_unlock(dct_tables->dct_bt, WICED_TRUE);
        if (result == WICED_SUCCESS)
        {
            dct_tables->dct_bt = NULL;
        }
        else
        {
            apollo_log_msg(APOLLO_LOG_ERR, "Can't free/release BT configuration !\r\n");
        }
    }

#endif

    if (dct_tables->dct_app != NULL)
    {
        result |= wiced_dct_read_unlock(dct_tables->dct_app, WICED_TRUE);
        if (result == WICED_SUCCESS)
        {
            dct_tables->dct_app = NULL;
        }
        else
        {
            apollo_log_msg(APOLLO_LOG_ERR, "Can't free/release app configuration !\r\n");
        }
    }

    return result;
}

wiced_result_t apollo_config_save(apollo_dct_collection_t* dct_tables)
{
    wiced_result_t result;

    result  = apollo_save_app_dct( dct_tables );
    if ( result != WICED_SUCCESS )
    {
        apollo_log_msg(APOLLO_LOG_ERR, "apollo_save_app_dct() failed !\r\n");
    }
    result |= apollo_save_network_dct( dct_tables );
    if ( result != WICED_SUCCESS )
    {
        apollo_log_msg(APOLLO_LOG_ERR, "apollo_save_network_dct() failed !\r\n");
    }

    result |= apollo_save_wifi_dct( dct_tables );
    if ( result != WICED_SUCCESS )
    {
        apollo_log_msg(APOLLO_LOG_ERR, "apollo_save_wifi_dct() failed !\r\n");
    }
#ifdef WICED_DCT_INCLUDE_BT_CONFIG
    result |= apollo_save_bt_dct( dct_tables );
    if ( result != WICED_SUCCESS )
    {
        apollo_log_msg(APOLLO_LOG_ERR, "apollo_save_bt_dct() failed !\r\n");
    }
#endif

    return result;
}
