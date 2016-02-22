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
 * BLE MESH RESTful API Demo Application
 * The Application enables BIG as a Mesh Node and Mesh Proxy
 * When the application comes up BIG will be ready to be provisioned.
 * 1> Provision the BIG and other mesh tags using the Mesh Android Test App
 * 2> Inside Home : The App can be used to control mesh tags via Proxy
 *    (BIG can be one of the Mesh Proxy for inside home usecase)
 * 3> Outside Home : The App controls the mesh Tags via BIG using the Mesh REST interfaces
 *
 */

#include "string.h"
#include "wiced.h"
#include "bt_internet_gateway.h"
#include "blemesh.h"

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
static wiced_result_t blemesh_read_dct_callback  ( flood_mesh_dct_t* read_dct_buffer );
static wiced_result_t blemesh_write_dct_callback ( flood_mesh_dct_t* dct_to_write );

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

    blemesh_init( paired_device_keys_callback, blemesh_read_dct_callback, blemesh_write_dct_callback );
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

static wiced_result_t blemesh_read_dct_callback( flood_mesh_dct_t* read_dct_buffer )
{
    flood_mesh_dct_t *dct;
    uint32_t i;

    wiced_dct_read_lock( (void**) &dct, TRUE, DCT_APP_SECTION, 0, sizeof( *dct ) );
    read_dct_buffer->node_restore = dct->node_restore;
    for ( i = 0; i < MESH_NODE_INFO_LENGTH; i++ )
    {
        read_dct_buffer->node_info[ i ] = dct->node_info[ i ];

    }
    wiced_dct_read_unlock( dct, WICED_FALSE );
    return WICED_SUCCESS;
}

static wiced_result_t blemesh_write_dct_callback( flood_mesh_dct_t* dct_to_write )
{
    return wiced_dct_write( dct_to_write, DCT_APP_SECTION, 0, sizeof( *dct_to_write ) );
}
