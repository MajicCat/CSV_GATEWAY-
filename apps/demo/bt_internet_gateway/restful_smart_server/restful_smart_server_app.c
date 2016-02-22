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
 * BLE RESTful API Demo Application
 *
 */

#include "string.h"
#include "wiced.h"
#include "bt_internet_gateway.h"
#include "restful_smart_server.h"

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               External Function Declarations
 ******************************************************/

static wiced_result_t local_keys_callback        ( big_security_event_t event, wiced_bt_local_identity_keys_t* keys );
static wiced_result_t paired_device_keys_callback( big_security_event_t event, wiced_bt_device_link_keys_t* keys );

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

void application_start( void )
{
    wiced_init( );

    /* Bring-up the network interface */
    wiced_network_up( WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL );

    bt_internet_gateway_init( local_keys_callback );

    restful_smart_server_start( paired_device_keys_callback );
}

static wiced_result_t local_keys_callback( big_security_event_t event, wiced_bt_local_identity_keys_t* keys )
{
    wiced_result_t result = WICED_BT_SUCCESS;

    switch ( event )
    {
        case BIG_UPDATE_SECURITY_KEYS_EVENT:
        {
            WPRINT_APP_INFO( ( "Store local keys in non-volatile memory\n" ) );

            /* Return WICED_BT_SUCCESS to indicate to the stack that keys are successfully stored */
            result = WICED_BT_SUCCESS;
            break;
        }
        case BIG_REQUEST_SECURITY_KEYS_EVENT:
        {
            WPRINT_APP_INFO( ( "Stack requests for local security keys\n" ) );

            /* Return WICED_BT_ERROR to indicate to the stack that keys aren't available. The stack needs to generate them internally */
            result = WICED_BT_ERROR;
            break;
        }
    }

    return result;
}

static wiced_result_t paired_device_keys_callback( big_security_event_t event, wiced_bt_device_link_keys_t* keys )
{
    wiced_result_t result = WICED_BT_SUCCESS;

    switch ( event )
    {
        case BIG_UPDATE_SECURITY_KEYS_EVENT:
        {
            WPRINT_APP_INFO( ( "Store paired device keys in non-volatile memory\n" ) );

            /* Return WICED_BT_SUCCESS to indicate to the stack that keys are successfully stored */
            result = WICED_BT_SUCCESS;
            break;
        }
        case BIG_REQUEST_SECURITY_KEYS_EVENT:
        {
            WPRINT_APP_INFO( ( "Stack requests for paired device security keys\n" ) );

            /* Return WICED_BT_ERROR to indicate to the stack that keys aren't available. The stack needs to generate them internally */
            result = WICED_BT_ERROR;
            break;
        }
    }

    return result;
}
