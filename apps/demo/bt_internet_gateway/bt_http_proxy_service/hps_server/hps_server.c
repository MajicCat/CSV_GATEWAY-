/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

#include "bt_internet_gateway.h"
#include "bt_http_proxy_server.h"
#include "wiced_rtos.h"
#include "wiced_network.h"
#include "wiced_tcpip.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define WICED_HPS_INTERFACE (WICED_STA_INTERFACE)
#define MAX_HPS_CONNECTIONS (3)

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
 *               Static Function Declarations
 ******************************************************/

static wiced_result_t local_keys_callback        ( big_security_event_t event, wiced_bt_local_identity_keys_t* keys );
static wiced_result_t paired_device_keys_callback( big_security_event_t event, wiced_bt_device_link_keys_t* keys );

/******************************************************
 *               Variable Definitions
 ******************************************************/

hps_connection_t hps_connection_array[ MAX_HPS_CONNECTIONS ];

/******************************************************
 *               Function Definitions
 ******************************************************/

void application_start( void )
{
    wiced_init();

    bt_internet_gateway_init( local_keys_callback );

    hps_server_start( &hps_connection_array[ 0 ], MAX_HPS_CONNECTIONS, WICED_HPS_INTERFACE, paired_device_keys_callback );
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
