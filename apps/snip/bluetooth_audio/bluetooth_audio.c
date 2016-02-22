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
 * Bluetooth Audio AVDT Sink Application
 *
 * The application demonstrates the following features:
 *  - Bluetooth intialization
 *  - A2DP sink
 *  - SBC Decoding
 *  - Audio playback
 *
 * Usage:
 *    On startup device will be discoverable and connectable,
 *    allowing a BT audio source to connect and stream audio.
 *
 * Notes: Currently supports 44.1kHz and 48kHz audio
 */

#include <stdlib.h>
#include "wiced.h"
#include "wiced_rtos.h"
#include "wiced_result.h"
#include "platform_audio.h"
#include "wiced_bt_stack.h"
#include "wiced_bt_dev.h"
#include "bluetooth_audio.h"
#include "bluetooth_audio_nv.h"

#ifdef USE_MEM_POOL
#include "mem_pool.h"
#endif

/******************************************************
 *                    Constants
 ******************************************************/
        // SDP Record for Device ID
#define HDLR_DEVICE_ID                  0x10001
        // SDP Record for AVDT Sink
#define HDLR_AVDT_SINK                  0x10002

#define BLUETOOTH_DEVICE_NAME           "WICED BLUETOOTH AUDIO"

#define EIR_DATA_LENGTH                 240
#define EIR_COMPLETE_LOCAL_NAME_TYPE    0x09
#define EIR_COMPLETE_16BITS_UUID_TYPE   0x03

#define BT_AUDIO_TASK_PRIORITY_BASE     ( 0 )
#define BT_AUDIO_PLAYER_TASK_PRIORITY   ( BT_AUDIO_TASK_PRIORITY_BASE )
#define BT_AUDIO_DECODER_TASK_PRIORITY    ( BT_AUDIO_TASK_PRIORITY_BASE + 1 )

#define PLAYER_STACK_SIZE               0x1000
#define DECODER_STACK_SIZE                0x1000

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/
static wiced_bt_dev_status_t bt_audio_management_callback( wiced_bt_management_evt_t event, wiced_bt_management_evt_data_t *p_event_data );
void bt_audio_sink_data_cb( wiced_bt_a2dp_sink_codec_t codec_type, wiced_bt_a2dp_sink_audio_data_t* p_audio_data );
static void bt_audio_sink_control_cb( wiced_bt_a2dp_sink_event_t event, wiced_bt_a2dp_sink_event_data_t* p_data);

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/
wiced_thread_t         decoder_thread;
wiced_thread_t         player_thread;

uint8_t decoder_stack[DECODER_STACK_SIZE]__attribute__((section (".ccm")));
uint8_t player_stack[PLAYER_STACK_SIZE]__attribute__((section (".ccm")));

DEFINE_APP_DCT(bt_dct_t)
{
    .bt_hash_table              = {0},
    .bt_paired_device_info[0]   = {0},
    .bt_local_id_keys           = {{0}},
};

static uint8_t bluetooth_device_name[249] = BLUETOOTH_DEVICE_NAME;

uint8_t uuid_list[] =
{
    0x0B, 0x11, /* Audio Sink */
};

const uint8_t sdp_database[] = // Define SDP database
{
    SDP_ATTR_SEQUENCE_1(159),

    // SDP Record for Device ID
    SDP_ATTR_SEQUENCE_1(77),
        SDP_ATTR_RECORD_HANDLE(HDLR_DEVICE_ID),
        SDP_ATTR_CLASS_ID(UUID_SERVCLASS_PNP_INFORMATION),
        SDP_ATTR_PROTOCOL_DESC_LIST(1),
        SDP_ATTR_BROWSE_LIST,
        SDP_ATTR_UINT2(ATTR_ID_SPECIFICATION_ID, 0x103),
        SDP_ATTR_UINT2(ATTR_ID_VENDOR_ID, 0x000F),
        SDP_ATTR_UINT2(ATTR_ID_PRODUCT_ID, 0x0000),
        SDP_ATTR_UINT2(ATTR_ID_PRODUCT_VERSION, 0x0001),
        SDP_ATTR_BOOLEAN(ATTR_ID_PRIMARY_RECORD, 0x01),
        SDP_ATTR_UINT2(ATTR_ID_VENDOR_ID_SOURCE, DI_VENDOR_ID_SOURCE_BTSIG),

    // SDP Record for AVDT Sink
    SDP_ATTR_SEQUENCE_1(78),
        SDP_ATTR_RECORD_HANDLE(HDLR_AVDT_SINK),
        SDP_ATTR_CLASS_ID(UUID_SERVCLASS_AUDIO_SINK),
        SDP_ATTR_ID(ATTR_ID_PROTOCOL_DESC_LIST), SDP_ATTR_SEQUENCE_1(16),
            SDP_ATTR_SEQUENCE_1(6),
                SDP_ATTR_UUID16(UUID_PROTOCOL_L2CAP),
                SDP_ATTR_VALUE_UINT2(BT_PSM_AVDTP),
            SDP_ATTR_SEQUENCE_1(6),
                SDP_ATTR_UUID16(UUID_PROTOCOL_AVDTP),
                SDP_ATTR_VALUE_UINT2(0x100),
        SDP_ATTR_BROWSE_LIST,
        SDP_ATTR_ID(ATTR_ID_BT_PROFILE_DESC_LIST), SDP_ATTR_SEQUENCE_1(8),
            SDP_ATTR_SEQUENCE_1(6),
                SDP_ATTR_UUID16(UUID_SERVCLASS_ADV_AUDIO_DISTRIBUTION),
                SDP_ATTR_VALUE_UINT2(0x100),
        SDP_ATTR_SERVICE_NAME(9),
            'A', '2', 'D', 'P', ' ', 'S', 'i', 'n', 'k',
        SDP_ATTR_UINT2(ATTR_ID_SUPPORTED_FEATURES, 0x0003),
};

/* Name, address and class of device shall be over-written using DCT entries if present.
* Please use the DCT entries to configure these parameters */
wiced_bt_cfg_settings_t wiced_bt_audio_cfg_settings =
{
    .device_name               = (uint8_t*)BLUETOOTH_DEVICE_NAME,
    .device_class              = {0x24, 0x04, 0x18},
    .security_requirement_mask = BTM_SEC_NONE,
    .max_simultaneous_links    = 3,/*TBD*/

    /*BR/EDR scan settings */
    .br_edr_scan_cfg =
    {
        .inquiry_scan_type     = BTM_SCAN_TYPE_INTERLACED,
        .inquiry_scan_interval = WICED_BT_CFG_DEFAULT_INQUIRY_SCAN_INTERVAL,
        .inquiry_scan_window   = WICED_BT_CFG_DEFAULT_INQUIRY_SCAN_WINDOW,

        .page_scan_type        = BTM_SCAN_TYPE_INTERLACED,
        .page_scan_interval    = WICED_BT_CFG_DEFAULT_PAGE_SCAN_INTERVAL,
        .page_scan_window      = WICED_BT_CFG_DEFAULT_PAGE_SCAN_WINDOW,
    },

    /*BLE scan settings */
    .ble_scan_cfg =
        {
            .scan_mode = BTM_BLE_SCAN_MODE_NONE,
        },

    .l2cap_application =
     {
         .max_links       = 2,
         .max_psm         = 0, /*TBD*/
         .max_channels    = 6,
     },

     .avdt_cfg =
     {
         .max_links = 1,
     },

     .addr_resolution_db_size = 3,
};

const wiced_bt_cfg_buf_pool_t wiced_bt_audio_cfg_buf_pools[WICED_BT_CFG_NUM_BUF_POOLS] =
{
/*  { buf_size, buf_count } */
    { 64,       10   },      /* Small Buffer Pool */
    { 360,      10   },      /* Medium Buffer Pool (used for HCI & RFCOMM control messages, min recommended size is 360) */
    { 660,      20  },      /* Large Buffer Pool  (used for HCI ACL messages) */
    { 0,         0   },      /* Extra Large Buffer Pool - Used for avdt media packets and miscellaneous (if not needed, set buf_count to 0) */
};

wiced_bt_a2dp_codec_info_t bt_audio_codec_capabilities =
{
    .codec_id = WICED_BT_A2DP_SINK_CODEC_SBC,
    .cie =
        {
            .sbc =
            {
                (A2D_SBC_IE_SAMP_FREQ_44 | A2D_SBC_IE_SAMP_FREQ_48),    /* samp_freq */
                (A2D_SBC_IE_CH_MD_MONO | A2D_SBC_IE_CH_MD_STEREO |
                 A2D_SBC_IE_CH_MD_JOINT | A2D_SBC_IE_CH_MD_DUAL),       /* ch_mode */
                (A2D_SBC_IE_BLOCKS_16 | A2D_SBC_IE_BLOCKS_12 |
                 A2D_SBC_IE_BLOCKS_8 | A2D_SBC_IE_BLOCKS_4),            /* block_len */
                (A2D_SBC_IE_SUBBAND_4 | A2D_SBC_IE_SUBBAND_8),          /* num_subbands */
                (A2D_SBC_IE_ALLOC_MD_L | A2D_SBC_IE_ALLOC_MD_S),        /* alloc_mthd */
                A2D_SBC_IE_MAX_BITPOOL,                                 /* max_bitpool */
                A2D_SBC_IE_MIN_BITPOOL                                  /* min_bitpool */
            }
        }
};

wiced_bt_a2dp_config_data_t bt_audio_codec_config =
{
    .feature_mask = 0,
    .codec_capabilities =
    {
        .count = 1,
        .info = &bt_audio_codec_capabilities,
    }
};

/*BD address of the connected peer device*/
wiced_bt_device_address_t remote_address = {0};
wiced_bt_device_address_t local_address = {0x81,0x34,0x09,0xAD,0xCD,0xAC};
wiced_bt_a2dp_codec_info_t  codec_config;
#ifdef USE_MEM_POOL
bt_buffer_pool_handle_t  mem_pool;
#endif

/******************************************************
 *               Function Definitions
 ******************************************************/

wiced_result_t bt_audio_context_init( void )
{
    wiced_result_t result;
#ifdef USE_MEM_POOL
    result = bt_buffer_pool_init(&mem_pool, 30, 660);
    if(result != WICED_SUCCESS)
    {
        return result;
    }
#endif
    result = bt_audio_decoder_context_init();
    if(result != WICED_SUCCESS)
    {
        return result;
    }

    result = bt_audio_init_player();

    return result;
}

void application_start( )
{
    wiced_result_t result;

    wiced_core_init( );

    platform_init_audio( );

    WPRINT_APP_INFO ( ("Starting Bluetooth...\n") );

    result = bt_audio_context_init();
    if(result != WICED_SUCCESS)
    {
        WPRINT_APP_ERROR( ( "Failed to initialize Bluetooth App Context variables... aborting.\n" ) );
        return;
    }

    wiced_rtos_create_thread_with_stack( &player_thread, BT_AUDIO_PLAYER_TASK_PRIORITY, "PLAYER TASK", bt_audio_player_task, (UINT16 *)player_stack, PLAYER_STACK_SIZE, NULL );
    wiced_rtos_create_thread_with_stack( &decoder_thread, BT_AUDIO_DECODER_TASK_PRIORITY, "DECODER TASK", bt_audio_decoder_task, (UINT16 *)decoder_stack, DECODER_STACK_SIZE, NULL );

    /* Initialize BT NVRAM with APP_OFFSET for Bluetooth */
    result = bt_audio_nv_init( );
    if ( result != WICED_SUCCESS )
    {
        WPRINT_APP_ERROR( ("bt_audio_nv_init() failed !!\r\n" ) );
        return;
    }

#ifdef WICED_DCT_INCLUDE_BT_CONFIG
    {
        /* Configure the Device Name and Class of Device from the DCT */
         platform_dct_bt_config_t* dct_bt_config;
         // Read config
        wiced_dct_read_lock( (void**) &dct_bt_config, WICED_TRUE, DCT_BT_CONFIG_SECTION, 0, sizeof(platform_dct_bt_config_t) );
        WPRINT_APP_INFO( ("WICED DCT BT NAME: %s \r\n", dct_bt_config->bluetooth_device_name) );
        strlcpy((char*)bluetooth_device_name, (char*)dct_bt_config->bluetooth_device_name, sizeof(bluetooth_device_name));
        wiced_bt_audio_cfg_settings.device_name = bluetooth_device_name;
        WPRINT_APP_INFO( ("WICED DCT BT DEVICE CLASS : %02x %02x %02x\r\n", dct_bt_config->bluetooth_device_class[0],
                            dct_bt_config->bluetooth_device_class[1],dct_bt_config->bluetooth_device_class[2]) );
        memcpy( wiced_bt_audio_cfg_settings.device_class, dct_bt_config->bluetooth_device_class, sizeof(dct_bt_config->bluetooth_device_class));
        wiced_dct_read_unlock( (void*) dct_bt_config, WICED_TRUE );
    }
#endif
        WPRINT_APP_INFO( ("WICED DCT name %s\r\n", wiced_bt_audio_cfg_settings.device_name) );

        /* Initialize BT stack */
    result = wiced_bt_stack_init( bt_audio_management_callback, &wiced_bt_audio_cfg_settings, wiced_bt_audio_cfg_buf_pools );
    WPRINT_APP_INFO( ("wiced_add_bluetooth_dm result: 0x%x, \n", result) );
    if ( result != WICED_SUCCESS )
    {
        WPRINT_APP_ERROR( ( "Failed to initialize Bluetooth\n" ) );
        return;
    }
}


/*Utility Functions */
wiced_result_t bt_audio_get_config_from_cie(wiced_bt_a2dp_codec_info_t* p_codec_config, bt_audio_config_t* p_audio_config)
{
    wiced_result_t result = WICED_BADARG;

    if(p_codec_config == NULL || p_audio_config == NULL)
        return result;

    if(p_codec_config->codec_id == WICED_BT_A2DP_SINK_CODEC_SBC)
    {
        if(p_codec_config->cie.sbc.samp_freq == A2D_SBC_IE_SAMP_FREQ_44)
            p_audio_config->sample_rate = 44100;
        else if(p_codec_config->cie.sbc.samp_freq == A2D_SBC_IE_SAMP_FREQ_48)
            p_audio_config->sample_rate = 48000;
        else if(p_codec_config->cie.sbc.samp_freq == A2D_SBC_IE_SAMP_FREQ_32)
            p_audio_config->sample_rate = 32000;
        else if(p_codec_config->cie.sbc.samp_freq == A2D_SBC_IE_SAMP_FREQ_16)
            p_audio_config->sample_rate = 16000;
        else
            return WICED_ERROR;

        if(p_codec_config->cie.sbc.ch_mode == A2D_SBC_IE_CH_MD_MONO)
            p_audio_config->channels = 1;
        else
            p_audio_config->channels = 2;

        p_audio_config->bits_per_sample = 16;
        result = WICED_SUCCESS;
    }
    return result;
}


/* DM Functions */
wiced_result_t bt_audio_write_eir( uint8_t *device_name )
{
    uint8_t eir_cfg[EIR_DATA_LENGTH] = {0};
    uint8_t* p = eir_cfg;
    uint8_t name_len = strlen((char*)device_name);

    *p++ = (uint8_t)(name_len+1);                 /* Length */
    *p++ = (uint8_t)EIR_COMPLETE_LOCAL_NAME_TYPE; /* EIR Data Type */
    memcpy(p, device_name, name_len);   /* Name string */
    p += name_len;

    *p++ = sizeof(uuid_list)+1;
    *p++ = (uint8_t) EIR_COMPLETE_16BITS_UUID_TYPE;
    memcpy(p, uuid_list, sizeof(uuid_list));

    return wiced_bt_dev_write_eir(eir_cfg, EIR_DATA_LENGTH);
}


static wiced_bt_dev_status_t bt_audio_management_callback( wiced_bt_management_evt_t event, wiced_bt_management_evt_data_t *p_event_data )
{
    wiced_bt_dev_status_t status = WICED_BT_SUCCESS;
    wiced_bt_device_address_t bda;
    wiced_result_t result = WICED_ERROR;

    switch ( event )
    {
        case BTM_ENABLED_EVT:
        {
            /* Bluetooth controller and host stack enabled */
            WPRINT_APP_INFO( ("bt_audio_management_callback: Bluetooth enabled (%s)\n", ((p_event_data->enabled.status == WICED_BT_SUCCESS) ? "success":"failure")) );

            if ( p_event_data->enabled.status == WICED_BT_SUCCESS )
            {
#ifdef WICED_DCT_INCLUDE_BT_CONFIG
                /* Configure the Device Name and Class of Device from the DCT */
                platform_dct_bt_config_t* dct_bt_config;
                 // Read config
                wiced_dct_read_lock( (void**) &dct_bt_config, WICED_TRUE, DCT_BT_CONFIG_SECTION, 0, sizeof(platform_dct_bt_config_t) );
                WPRINT_APP_INFO( ("WICED DCT BT ADDR 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x \r\n",
                                  dct_bt_config->bluetooth_device_address[0], dct_bt_config->bluetooth_device_address[1],
                                  dct_bt_config->bluetooth_device_address[2], dct_bt_config->bluetooth_device_address[3],
                                  dct_bt_config->bluetooth_device_address[4], dct_bt_config->bluetooth_device_address[5]) );
                wiced_bt_set_local_bdaddr( dct_bt_config->bluetooth_device_address );
                result = bt_audio_write_eir(dct_bt_config->bluetooth_device_name);
                wiced_dct_read_unlock( (void*) dct_bt_config, WICED_TRUE );
#else
                result = bt_audio_write_eir(BLUETOOTH_DEVICE_NAME);
#endif
                WPRINT_APP_INFO( ("bt_audio_management_callback: wiced_bt_dev_write_eir result = 0x%x\n", (unsigned int)result) );

                wiced_bt_dev_read_local_addr( bda );
                WPRINT_APP_INFO( ("bt_audio_management_callback:Local Bluetooth Address: [%02X:%02X:%02X:%02X:%02X:%02X]\n", bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]) );
            }

            result = wiced_bt_sdp_db_init( ( uint8_t * )sdp_database, sizeof( sdp_database ) );
            WPRINT_APP_INFO( ("bt_audio_management_callback: wiced_bt_sdp_db_init result (bool) = 0x%x\n", (unsigned int)result) );

            result = wiced_bt_dev_set_discoverability(BTM_GENERAL_DISCOVERABLE, wiced_bt_audio_cfg_settings.br_edr_scan_cfg.inquiry_scan_window, wiced_bt_audio_cfg_settings.br_edr_scan_cfg.inquiry_scan_interval);
            WPRINT_APP_INFO( ("bt_audio_management_callback: discoverability result = 0x%x\n", (unsigned int)result) );

            result = wiced_bt_dev_set_connectability (BTM_CONNECTABLE, wiced_bt_audio_cfg_settings.br_edr_scan_cfg.page_scan_window, wiced_bt_audio_cfg_settings.br_edr_scan_cfg.page_scan_interval);
            WPRINT_APP_INFO( ("bt_audio_management_callback:connectability result = 0x%x\n", (unsigned int)result) );

            //a2dp sink init : TODO: get capability from the codec fwk.
            result = wiced_bt_a2dp_sink_init( &bt_audio_codec_config, bt_audio_sink_control_cb, bt_audio_sink_data_cb );
            WPRINT_APP_INFO( ("bt_audio_management_callback:wiced_bt_a2dp_sink_init result = 0x%x\n", (unsigned int)result) );
        }
        break;

        case BTM_PAIRING_IO_CAPABILITIES_BR_EDR_REQUEST_EVT:
            WPRINT_APP_INFO( ("bt_audio_management_callback: IO capabilities BR/EDR request: \n") );
            p_event_data->pairing_io_capabilities_br_edr_request.local_io_cap = BTM_IO_CAPABILITIES_NONE;
            p_event_data->pairing_io_capabilities_br_edr_request.auth_req = BTM_AUTH_SINGLE_PROFILE_GENERAL_BONDING_NO;
            break;

        case BTM_USER_CONFIRMATION_REQUEST_EVT:
            WPRINT_APP_INFO( ("bt_audio_management_callback: User confirmation request: \n") );
            wiced_bt_dev_confirm_req_reply(WICED_BT_SUCCESS, p_event_data->user_confirmation_request.bd_addr);
            break;

        case BTM_PAIRED_DEVICE_LINK_KEYS_UPDATE_EVT:
            WPRINT_APP_INFO( ("bt_audio_management_callback: Link key update evt\n") );
            bt_audio_nv_update_device_link_key( &p_event_data->paired_device_link_keys_update );
            break;

        case BTM_PAIRED_DEVICE_LINK_KEYS_REQUEST_EVT:
            {
                bt_audio_paired_device_info_t out_device;
                WPRINT_APP_INFO( ("bt_audio_management_callback: Link key request evt\n") );
                if(WICED_SUCCESS == bt_audio_nv_get_device_info_by_addr( &p_event_data->paired_device_link_keys_request.bd_addr, &out_device ) )
                {
                    memcpy(&p_event_data->paired_device_link_keys_request.key_data, &out_device.device_link.key_data, sizeof( out_device.device_link.key_data ) );
                }
                else
                {
                    status = WICED_BT_ERROR;
                }
            }
            break;

        case BTM_LOCAL_IDENTITY_KEYS_UPDATE_EVT:
            WPRINT_APP_INFO( ("bt_audio_management_callback: Local ID key update evt\n") );
            bt_audio_nv_update_local_id_keys( &p_event_data->local_identity_keys_update );
            break;

        case BTM_LOCAL_IDENTITY_KEYS_REQUEST_EVT:
            if(WICED_SUCCESS != bt_audio_nv_get_local_id_keys( &p_event_data->local_identity_keys_request ))
                status = WICED_BT_ERROR;
            break;

        case BTM_SECURITY_REQUEST_EVT:
            WPRINT_APP_INFO( ("bt_audio_management_callback: Security reqeust\n") );
            break;

        case BTM_PAIRING_COMPLETE_EVT:
            WPRINT_APP_INFO( ("bt_audio_management_callback: Pairing complete %i.\n", p_event_data->pairing_complete.pairing_complete_info.ble.status) );
            break;

        default:
            WPRINT_APP_INFO( ("bt_audio_management_callback: Unhandled Bluetooth Management Event: 0x%x\n", event) );
            status = WICED_BT_ERROR;
            break;
    }

    return ( status );
}


/*Audio Sink Functions*/
static void bt_audio_sink_control_cb( wiced_bt_a2dp_sink_event_t event, wiced_bt_a2dp_sink_event_data_t* p_data)
{
    switch(event)
    {
        case WICED_BT_A2DP_SINK_CONNECT_EVT:
            WPRINT_APP_INFO( ("bt_audio_sink_control_cb:CONNECT EVENT \nstatus = %d\n", p_data->connect.result) );

            if(p_data->connect.result == WICED_SUCCESS)
            {
                memcpy( remote_address, p_data->connect.bd_addr, sizeof(wiced_bt_device_address_t) );
                WPRINT_APP_INFO( ("bt_audio_sink_control_cb: Remote Bluetooth Address: [%02X:%02X:%02X:%02X:%02X:%02X]\n",
                                       remote_address[0], remote_address[1], remote_address[2], remote_address[3], remote_address[4], remote_address[5]) );
            }
        break;

        case WICED_BT_A2DP_SINK_DISCONNECT_EVT:
        {
            wiced_bt_device_address_t bda;

            bt_audio_stop_player(); //stop if player was running, needed here when src goes out of range or BT is turned off at src and it doesn't send suspend before disconnecting
            memcpy( bda, p_data->disconnect.bd_addr, sizeof(wiced_bt_device_address_t) );
            WPRINT_APP_INFO( ("bt_audio_sink_control_cb:DISCONNECTED EVENT \nreason = %d \nRemote Bluetooth Address: [%02X:%02X:%02X:%02X:%02X:%02X]\n",
                                            p_data->disconnect.result, bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]) );
            memset(remote_address, 0, sizeof(wiced_bt_device_address_t) );
#ifdef USE_MEM_POOL
            bt_buffer_pool_print_debug_info( mem_pool );
#endif
        }
        break;

        case WICED_BT_A2DP_SINK_START_EVT:
        {
            bt_audio_config_t audio_config;
            //WPRINT_APP_INFO( ("bt_audio_sink_control_cb:STREAM START EVENT \nstatus = %d \n", p_data->start.result) );
            /*Configure codec and player(DAC) with the configuration received thru Codec Config event from the peer*/
            bt_audio_configure_decoder(&codec_config);
            bt_audio_get_config_from_cie(&codec_config, &audio_config);
            bt_audio_configure_player( &audio_config );
        }
        break;

        case WICED_BT_A2DP_SINK_SUSPEND_EVT:
            WPRINT_APP_INFO( ("bt_audio_sink_control_cb:STREAM SUSPEND EVENT \nstatus = %d \n", p_data->start.result) );
            /*Stop the player task and reset the decoder configuration*/
            bt_audio_stop_player();
            bt_audio_reset_decoder_config();
#ifdef USE_MEM_POOL
            bt_buffer_pool_print_debug_info( mem_pool );
#endif
         break;

        case WICED_BT_A2DP_SINK_CODEC_CONFIG_EVT:
        {
            WPRINT_APP_INFO( ("bt_audio_sink_control_cb:CODEC CONFIG \nCODEC ID:0x%02X  FS:0x%02X  CH_MODE:0x%02X  BLOCK_LEN:0x%02X  NUM_SUBBANDS:0x%02X  ALLOC_METHOD:0x%02X  MAX_BITPOOL:0x%02X  MIN_BITPOOL:0x%02X\n",
                    p_data->codec_config.codec_id, p_data->codec_config.cie.sbc.samp_freq, p_data->codec_config.cie.sbc.ch_mode, p_data->codec_config.cie.sbc.block_len,
                    p_data->codec_config.cie.sbc.num_subbands, p_data->codec_config.cie.sbc.alloc_mthd, p_data->codec_config.cie.sbc.max_bitpool, p_data->codec_config.cie.sbc.min_bitpool) );

            /*Save the codec configuration received from the peer, this is used to configure decoder and player at the time of starting the audio stream*/
            memcpy(&codec_config, &p_data->codec_config, sizeof(wiced_bt_a2dp_codec_info_t));
        }
        break;
    }
}

void bt_audio_sink_data_cb( wiced_bt_a2dp_sink_codec_t codec_type, wiced_bt_a2dp_sink_audio_data_t* p_audio_data )
{
    bt_audio_codec_data_t* audio = NULL;
    uint8_t* in_audio;
    uint16_t in_length;

    p_audio_data->p_pkt->len--;
    p_audio_data->p_pkt->offset++;
    in_length = p_audio_data->p_pkt->len;

#ifdef USE_MEM_POOL
    audio = bt_buffer_pool_allocate_buffer(mem_pool);
#else
    audio = malloc( sizeof(bt_audio_codec_data_t)+in_length );
#endif
    if(audio == NULL)
    {
        WPRINT_APP_INFO( ("bt_audio_sink_data_cb: mem alloc failed\n") );
        return;
    }
    in_audio = ((uint8_t*)(p_audio_data->p_pkt+1))+p_audio_data->p_pkt->offset;
    memcpy(audio->data, in_audio, in_length);
    audio->length = in_length;
    audio->offset = 0;
    bt_audio_write_to_decoder_queue(audio);
}

