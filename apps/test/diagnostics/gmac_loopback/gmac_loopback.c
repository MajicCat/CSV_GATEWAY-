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
 * Ethernet (GMAC) test Application
 *
 * This program tests the board's GMAC functionality.
 *
 * It uses GMAC DMA and PHY loopbacks to transmit/receive/compare packets.
 *
 */

#include "wiced.h"
#include "platform_ethernet.h"

#define GMAC_LOOPBACK_PACKET_NUM             1024

#define GMAC_LOOPBACK_IP                     MAKE_IPV4_ADDRESS(192, 168,   0, 1)
#define GMAC_LOOPBACK_NETMASK                MAKE_IPV4_ADDRESS(255, 255, 255, 0)

#define GMAC_LOOPBACK_UDP_TARGET_PORT        50007
#define GMAC_LOOPBACK_UDP_TARGET_IP          MAKE_IPV4_ADDRESS(255, 255, 255, 255)

#define GMAC_LOOPBACK_RX_TIMEOUT             5000

#define GMAC_LOOPBACK_UDP_PAYLOAD_SIZE       1472

static platform_ethernet_loopback_mode_t modes_to_test[] =
{
    /*
     * DMA should be first as it checks GMAC only, and if fine then next tests involve PHY.
     * Tests sequence has all permutations of previous:current test.
     */
    PLATFORM_ETHERNET_LOOPBACK_DMA,
    PLATFORM_ETHERNET_LOOPBACK_DMA,
    PLATFORM_ETHERNET_LOOPBACK_PHY,
    PLATFORM_ETHERNET_LOOPBACK_PHY,
    PLATFORM_ETHERNET_LOOPBACK_DMA
};

static const wiced_ip_setting_t device_static_ip_settings =
{
    INITIALISER_IPV4_ADDRESS( .ip_address, GMAC_LOOPBACK_IP ),
    INITIALISER_IPV4_ADDRESS( .gateway,    GMAC_LOOPBACK_IP ),
    INITIALISER_IPV4_ADDRESS( .netmask,    GMAC_LOOPBACK_NETMASK ),
};

static uint8_t gmac_loopback_tx_counter = 0;

static int gmac_loopback_tx_udp_packet( wiced_udp_socket_t* udp_socket )
{
    int             result = 0;
    wiced_packet_t* packet;
    uint8_t*        udp_data;
    uint16_t        available_data_length;
    wiced_result_t  wiced_result;

    wiced_result = wiced_packet_create_udp( udp_socket, GMAC_LOOPBACK_UDP_PAYLOAD_SIZE, &packet, (uint8_t**)&udp_data, &available_data_length );
    if ( wiced_result != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ( "UDP packet allocating failure: %d\n", (int)wiced_result ) );
        result = -1;
    }
    else
    {
        if ( available_data_length < GMAC_LOOPBACK_UDP_PAYLOAD_SIZE )
        {
            WPRINT_APP_INFO( ( "Packet available data length is too small: %d\n", (int)available_data_length ) );
            result = -2;
        }
        else
        {
            const wiced_ip_address_t INITIALISER_IPV4_ADDRESS( target_ip_addr, GMAC_LOOPBACK_UDP_TARGET_IP );
            uint16_t                 i;

            gmac_loopback_tx_counter++;

            for ( i = 0; i < GMAC_LOOPBACK_UDP_PAYLOAD_SIZE; i++ )
            {
                udp_data[ i ] = (uint8_t)( gmac_loopback_tx_counter + i );
            }

            wiced_packet_set_data_end( packet, udp_data + GMAC_LOOPBACK_UDP_PAYLOAD_SIZE );

            wiced_result = wiced_udp_send( udp_socket, &target_ip_addr, GMAC_LOOPBACK_UDP_TARGET_PORT, packet );
            if ( wiced_result != WICED_SUCCESS )
            {
                WPRINT_APP_INFO( ( "UDP packet sending failure: %d\n", (int)wiced_result ) );
                result = -3;
            }
        }

        if ( result != 0 )
        {
            wiced_packet_delete( packet );
        }
    }

    return result;
}

static int gmac_loopback_rx_udp_packet( wiced_udp_socket_t* udp_socket )
{
    int                result = 0;
    wiced_packet_t*    packet;
    wiced_result_t     wiced_result;

    wiced_result = wiced_udp_receive( udp_socket, &packet, GMAC_LOOPBACK_RX_TIMEOUT );
    if ( wiced_result != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ( "UDP receive failure: %d\n", (int)wiced_result ) );
        result = -4;
    }
    else
    {
        uint8_t* udp_data;
        uint16_t data_length;
        uint16_t available_data_length;
        uint16_t i;

        wiced_packet_get_data( packet, 0, (uint8_t**)&udp_data, &data_length, &available_data_length );

        if ( ( data_length != available_data_length ) || ( data_length != GMAC_LOOPBACK_UDP_PAYLOAD_SIZE ) )
        {
            WPRINT_APP_INFO( ( "Expected size is %u but data_length=%u available_data_length=%u\n",
                               (unsigned)GMAC_LOOPBACK_UDP_PAYLOAD_SIZE, (unsigned)data_length, (unsigned)available_data_length ) );
            result = -5;
        }
        else
        {
            for ( i = 0; i < GMAC_LOOPBACK_UDP_PAYLOAD_SIZE; i++ )
            {
                if ( udp_data[ i ] != (uint8_t)( gmac_loopback_tx_counter + i ) )
                {
                    WPRINT_APP_INFO( ( "RX data of 0x%x at offset %u while 0x%x expected\n",
                                       (unsigned)udp_data[ i ], (unsigned)i, (unsigned)gmac_loopback_tx_counter ) );
                    result = -6;
                    break;
                }
            }
        }

        wiced_packet_delete( packet );
    }

    return result;
}

static int gmac_loopback_txrx_udp_packets( wiced_udp_socket_t* udp_socket )
{
    wiced_time_t rtos_time1, rtos_time2;
    int i;

    wiced_time_get_time( &rtos_time1 );

    for ( i = 0; i < GMAC_LOOPBACK_PACKET_NUM; i++ )
    {
        int result;

        result = gmac_loopback_tx_udp_packet( udp_socket );
        if ( result != 0 )
        {
             return result;
        }

        result = gmac_loopback_rx_udp_packet( udp_socket );
        if ( result != 0 )
        {
            return result;
        }
    }

    wiced_time_get_time( &rtos_time2 );

    WPRINT_APP_INFO( ( "\n%d packets during %d RTOS ticks\n", (int)GMAC_LOOPBACK_PACKET_NUM, (int)( rtos_time2 - rtos_time1 ) ) );

    return 0;
}

int gmac_loopback_single_test( void )
{
    int            result = 0;
    wiced_result_t wiced_result;

    wiced_result = wiced_network_init( );
    if ( wiced_result != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ( "Network initialization failure: %d\n", (int)wiced_result ) );
        result = -7;
    }
    else
    {
        wiced_result = wiced_network_up( WICED_ETHERNET_INTERFACE, WICED_USE_STATIC_IP, &device_static_ip_settings );
        if ( wiced_result != WICED_SUCCESS )
        {
            WPRINT_APP_INFO( ( "Network bringing up failure: %d\n", (int)wiced_result ) );
            result = -8;
        }
        else
        {
            wiced_udp_socket_t udp_socket;

            wiced_result = wiced_udp_create_socket( &udp_socket, GMAC_LOOPBACK_UDP_TARGET_PORT, WICED_ETHERNET_INTERFACE );
            if ( wiced_result != WICED_SUCCESS )
            {
                WPRINT_APP_INFO( ( "Create socket failure: %d\n", (int)wiced_result ) );
                result = - 9;
            }
            else
            {
                result = gmac_loopback_txrx_udp_packets( &udp_socket );

                wiced_udp_delete_socket( &udp_socket );
            }

            wiced_network_down( WICED_ETHERNET_INTERFACE );
        }

        wiced_network_deinit( );
    }

    return result;
}

int gmac_loopback_test( void )
{
    int            result = 0;
    wiced_result_t wiced_result;
    unsigned       i;

    if ( !PLATFORM_FEATURE_ENAB( GMAC ) )
    {
        WPRINT_APP_INFO( ( "\nGMAC loopback test NOT supported\n" ) );
        return 0;
    }

    for ( i = 0; i < ARRAYSIZE (modes_to_test ); i++ )
    {
        WPRINT_APP_INFO( ( "\nStart test %u from %u using %d mode\n", i + 1, ARRAYSIZE (modes_to_test ), modes_to_test[ i ] ) );

        wiced_result = platform_ethernet_set_loopback_mode( modes_to_test[ i ] );
        if ( wiced_result != WICED_SUCCESS )
        {
            WPRINT_APP_INFO( ( "Ethernet loopback mode setting failure: %d\n", (int)wiced_result ) );
            result = -10;
        }
        else
        {
            result = gmac_loopback_single_test( );
        }

        WPRINT_APP_INFO( ( "\nFinish test %u from %u : %s\n+++++++++\n", i + 1, ARRAYSIZE (modes_to_test ), ( result == 0 ) ? "SUCCEED" : "FAILED" ) );

        if ( result != 0 )
        {
            break;
        }
    }

    platform_ethernet_set_loopback_mode( PLATFORM_ETHERNET_LOOPBACK_DISABLE );

    if ( result == 0 )
    {
        WPRINT_APP_INFO( ( "\nGMAC loopback test SUCCEED\n" ) );
    }
    else
    {
        WPRINT_APP_INFO( ( "\nGMAC loopback test FAILED with %d return code during %u iteration\n", (int)result, i ) );
    }

    return result;
}
