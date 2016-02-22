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
 * Bluetooth Classic RFCOMM and BLE GATT Dual mode sample application
 *
 * Features demonstrated
 *  - WICED BT RFCOMM server APIs
 *
 * On startup this demo:
 *  - Initializes the wiced_bt subsystem
 *  - Starts the GATT and RFCOMM server
 *  - Waits for clients to connect
 *
 * Application Instructions
 *   Connect a PC terminal to the serial port of the WICED Eval board,
 *   then build and download the application as described in the WICED
 *   Quick Start Guide
 *
 */
#include "wiced_bt_cfg.h"
#include <string.h>
#include <stdio.h>
#include "wiced.h"
#include "wiced_bt_dev.h"
#include "wiced_bt_ble.h"
#include "wiced_bt_gatt.h"
#include "wiced_bt_cfg.h"
#include "wiced_bt_gatt_db.h"
#include "bt_rfcomm_server.h"
#include "wiced_bt_stack.h"

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

/******************************************************
 *               Function Declarations
 ******************************************************/
static wiced_bt_dev_status_t bt_dualmode_server_management_cback( wiced_bt_management_evt_t event, wiced_bt_management_evt_data_t *p_event_data );
static void bt_dualmode_server_init( void );
extern void ble_proximity_reporter_init( void );
extern void bt_rfcomm_server_init( void );

/******************************************************
 *               Variable Definitions
 ******************************************************/
uint8_t bluetooth_device_name[249] = WICED_BLUETOOTH_DEVICE_NAME;

/******************************************************
 *               Function Definitions
 ******************************************************/
void application_start( )
{
    /* Initialize Bluetooth controller and host stack */
#ifdef WICED_DCT_INCLUDE_BT_CONFIG
    {
        /* Configure the Device Name and Class of Device from the DCT */
         platform_dct_bt_config_t* dct_bt_config;
        wiced_dct_read_lock( (void**) &dct_bt_config, WICED_TRUE, DCT_BT_CONFIG_SECTION, 0, sizeof(platform_dct_bt_config_t) );
        WPRINT_APP_INFO( ("WICED DCT BT NAME: %s \r\n", dct_bt_config->bluetooth_device_name) );
        strlcpy((char*)bluetooth_device_name, (char*)dct_bt_config->bluetooth_device_name, sizeof(bluetooth_device_name));
        wiced_bt_cfg_settings.device_name = bluetooth_device_name;
        WPRINT_APP_INFO( ("WICED DCT BT DEVICE CLASS : %02x %02x %02x\r\n", dct_bt_config->bluetooth_device_class[0],
                dct_bt_config->bluetooth_device_class[1],dct_bt_config->bluetooth_device_class[2]) );
        memcpy(wiced_bt_cfg_settings.device_class, dct_bt_config->bluetooth_device_class, sizeof(dct_bt_config->bluetooth_device_class));
        wiced_dct_read_unlock( (void*) dct_bt_config, WICED_TRUE );
    }
#endif

    wiced_bt_stack_init( bt_dualmode_server_management_cback, &wiced_bt_cfg_settings, wiced_bt_cfg_buf_pools );
}

void bt_dualmode_server_init( void )
{
    ble_proximity_reporter_init();
    bt_rfcomm_server_init();
}

/* Bluetooth management event handler */
static wiced_bt_dev_status_t bt_dualmode_server_management_cback( wiced_bt_management_evt_t event, wiced_bt_management_evt_data_t *p_event_data )
{
    wiced_bt_device_address_t bda;

    WPRINT_BT_APP_INFO( ( "Bluetooth Management Event: 0x%x\n", event ) );

    wiced_bt_dev_status_t status = WICED_BT_SUCCESS;
    switch ( event )
    {
    case BTM_ENABLED_EVT:
        /* Bluetooth controller and host stack enabled */
        WPRINT_BT_APP_INFO( ( "Bluetooth enabled (%s)\n", ( (p_event_data->enabled.status == WICED_BT_SUCCESS) ? "success":"failure" ) ) );

        if ( p_event_data->enabled.status == WICED_BT_SUCCESS )
        {
            wiced_bt_dev_read_local_addr( bda );
            WPRINT_BT_APP_INFO( ( "Local Bluetooth Address: [%02X:%02X:%02X:%02X:%02X:%02X]\n", bda[0], bda[1], bda[2], bda[3], bda[4], bda[5] ) );

#ifdef WICED_DCT_INCLUDE_BT_CONFIG
    {
        /* Configure the Device Address from the DCT */
         platform_dct_bt_config_t* dct_bt_config;
        wiced_dct_read_lock( (void**) &dct_bt_config, WICED_TRUE, DCT_BT_CONFIG_SECTION, 0, sizeof(platform_dct_bt_config_t) );
        WPRINT_APP_INFO( ("WICED DCT BT ADDR 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x \r\n",
                dct_bt_config->bluetooth_device_address[0], dct_bt_config->bluetooth_device_address[1],
                dct_bt_config->bluetooth_device_address[2], dct_bt_config->bluetooth_device_address[3],
                dct_bt_config->bluetooth_device_address[4], dct_bt_config->bluetooth_device_address[5]) );
        wiced_bt_set_local_bdaddr ( dct_bt_config->bluetooth_device_address );
        wiced_dct_read_unlock( (void*) dct_bt_config, WICED_TRUE );
    }
#endif
    /* Enable proximity reporter and rfcomm server */
            bt_dualmode_server_init();
        }
        break;

    case BTM_PAIRED_DEVICE_LINK_KEYS_REQUEST_EVT:
        /* Request for stored link keys for remote device (if any) */
        /* (sample app does not store link keys to NVRAM) */
        status = WICED_BT_UNKNOWN_ADDR;
        break;

    case BTM_PAIRED_DEVICE_LINK_KEYS_UPDATE_EVT:
        /* Request store newly generated pairing link keys to NVRAM */
        /* (sample app does not store link keys to NVRAM) */
        break;


    case BTM_LOCAL_IDENTITY_KEYS_REQUEST_EVT:
        /* Request to restore local identity keys from NVRAM (requested during Bluetooth start up) */
        /* (sample app does not store keys to NVRAM. New local identity keys will be generated).   */
        status = WICED_BT_NO_RESOURCES;
        break;

    case BTM_LOCAL_IDENTITY_KEYS_UPDATE_EVT:
        /* Request to store newly generated local identity keys to NVRAM */
        /* (sample app does not store keys to NVRAM) */
        break;
#if 0
    case BTM_PAIRING_IO_CAPABILITIES_REQUEST_EVT:
        /* Request for local IO capabilities (sample app does not have i/o capabilities) */
        p_event_data->pairing_io_capabilities_request.local_io_cap = BTM_IO_CAPABILIES_NONE;
        break;
#endif
    case BTM_USER_CONFIRMATION_REQUEST_EVT:
        /* User confirmation request for pairing (sample app always accepts) */
        wiced_bt_dev_confirm_req_reply ( WICED_BT_SUCCESS, p_event_data->user_confirmation_request.bd_addr );
        break;

    case BTM_PAIRING_COMPLETE_EVT:
        /* Pairing complete */
        break;


    case BTM_BLE_ADVERT_STATE_CHANGED_EVT:
        /* adv state change callback */
        WPRINT_BT_APP_INFO( ( "---->>> New ADV state: %d\n", p_event_data->ble_advert_state_changed ) );
        break;

    default:
        WPRINT_BT_APP_INFO( ( "Unhandled Bluetooth Management Event: 0x%x\n", event ) );
        break;
    }

    return ( status );
}
