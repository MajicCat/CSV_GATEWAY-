/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

/******************************************************************************
 This driver was constructed using the NetX User Guide as a reference.
 It is available at http://www.rtos.com/PDFs/NetX_User_Guide.pdf
 *****************************************************************************/

#include "nx_api.h"
#include "wwd_wifi.h"
#include "network/wwd_network_interface.h"
#include "wwd_network.h"
#include "network/wwd_network_constants.h"
#include "wwd_assert.h"
#include "wiced_crypto.h"
#include "wiced_deep_sleep.h"
#include "platform_ethernet.h"
#include "internal/wwd_internal.h"

#if ( NX_PHYSICAL_HEADER != WICED_PHYSICAL_HEADER )
#error ERROR PHYSICAL HEADER SIZE CHANGED - PREBUILT NETX-DUO LIBRARY WILL NOT WORK
#endif

#if ( NX_PHYSICAL_TRAILER != WICED_PHYSICAL_TRAILER )
#error ERROR PHYSICAL TRAILER SIZE CHANGED - PREBUILT NETX-DUO LIBRARY WILL NOT WORK
#endif

/******************************************************
 *                      Macros
 ******************************************************/

#define IF_TO_IP( interface )   ( ip_ptr[ (interface)&3 ] )     /* STA = 0,  AP = 1, P2P = 2 */

/******************************************************
 *                    Constants
 ******************************************************/

#ifndef WICED_TCP_RX_DEPTH_QUEUE
#define WICED_TCP_RX_DEPTH_QUEUE    WICED_DEFAULT_TCP_RX_DEPTH_QUEUE
#endif

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

static void nx_wiced_add_ethernet_header( NX_IP* ip_ptr_in, NX_PACKET* packet_ptr, ULONG destination_mac_msw, ULONG destination_mac_lsw, USHORT ethertype );
static VOID wiced_netx_driver_entry( NX_IP_DRIVER* driver, wwd_interface_t interface );

/******************************************************
 *               Variable Definitions
 ******************************************************/

/* Saves pointers to the IP instances so that the receive function knows where to send data */
static NX_IP* ip_ptr[WWD_INTERFACE_MAX];

/* WICED specific NetX_Duo variable used to control the TCP RX queue depth when NetX_Duo is released as a library */
const ULONG nx_tcp_max_out_of_order_packets = WICED_TCP_RX_DEPTH_QUEUE;

/******************************************************
 *               Function Definitions
 ******************************************************/

VOID wiced_sta_netx_duo_driver_entry( NX_IP_DRIVER* driver )
{
    wiced_netx_driver_entry( driver, WWD_STA_INTERFACE );
}

VOID wiced_ap_netx_duo_driver_entry( NX_IP_DRIVER* driver )
{
    wiced_netx_driver_entry( driver, WWD_AP_INTERFACE );
}

VOID wiced_p2p_netx_duo_driver_entry( NX_IP_DRIVER* driver )
{
    wiced_netx_driver_entry( driver, WWD_P2P_INTERFACE );
}

static VOID wiced_netx_driver_entry( NX_IP_DRIVER* driver, wwd_interface_t interface )
{
    NX_PACKET*  packet_ptr;
    wiced_mac_t mac;
    NX_IP*      ip;

    wiced_assert( "Bad args", driver != NULL );

    packet_ptr = driver->nx_ip_driver_packet;

    driver->nx_ip_driver_status = NX_NOT_SUCCESSFUL;

    if ( ( interface != WWD_STA_INTERFACE ) &&
         ( interface != WWD_AP_INTERFACE ) &&
         ( interface != WWD_P2P_INTERFACE ) )
    {
        wiced_assert( "Bad interface", 0 != 0 );
        return;
    }

    /* Save the IP instance pointer so that the receive function will know where to send data */
    IF_TO_IP(interface) = driver->nx_ip_driver_ptr;
    ip = IF_TO_IP(interface);

    /* Process commands which are valid independent of the link state */
    switch ( driver->nx_ip_driver_command )
    {
        case NX_LINK_INITIALIZE:
            ip->nx_ip_driver_mtu            = (ULONG) WICED_PAYLOAD_MTU;
            ip->nx_ip_driver_mapping_needed = (UINT) NX_TRUE;
            driver->nx_ip_driver_status     = (UINT) NX_SUCCESS;
            break;

        case NX_LINK_UNINITIALIZE:
            IF_TO_IP(interface) = NULL;
            break;

        case NX_LINK_ENABLE:
            if ( wwd_wifi_get_mac_address( &mac, interface ) != WWD_SUCCESS )
            {
                ip->nx_ip_driver_link_up = NX_FALSE;
                break;
            }

            ip->nx_ip_arp_physical_address_msw = (ULONG) ( ( mac.octet[0] << 8 ) + mac.octet[1] );
            ip->nx_ip_arp_physical_address_lsw = (ULONG) ( ( mac.octet[2] << 24 ) + ( mac.octet[3] << 16 ) + ( mac.octet[4] << 8 ) + mac.octet[5] );

            ip->nx_ip_driver_link_up = (UINT) NX_TRUE;
            driver->nx_ip_driver_status = (UINT) NX_SUCCESS;
            break;

        case NX_LINK_DISABLE:
            ip->nx_ip_driver_link_up = NX_FALSE;
            driver->nx_ip_driver_status = (UINT) NX_SUCCESS;
            break;

        case NX_LINK_MULTICAST_JOIN:
            mac.octet[0] = (uint8_t) ( ( driver->nx_ip_driver_physical_address_msw & 0x0000ff00 ) >> 8 );
            mac.octet[1] = (uint8_t) ( ( driver->nx_ip_driver_physical_address_msw & 0x000000ff ) >> 0 );
            mac.octet[2] = (uint8_t) ( ( driver->nx_ip_driver_physical_address_lsw & 0xff000000 ) >> 24 );
            mac.octet[3] = (uint8_t) ( ( driver->nx_ip_driver_physical_address_lsw & 0x00ff0000 ) >> 16 );
            mac.octet[4] = (uint8_t) ( ( driver->nx_ip_driver_physical_address_lsw & 0x0000ff00 ) >> 8 );
            mac.octet[5] = (uint8_t) ( ( driver->nx_ip_driver_physical_address_lsw & 0x000000ff ) >> 0 );

            if ( wwd_wifi_register_multicast_address( &mac ) != WWD_SUCCESS )
            {
                driver->nx_ip_driver_status = (UINT) NX_NOT_SUCCESSFUL;
            }
            driver->nx_ip_driver_status = (UINT) NX_SUCCESS;
            break;

        case NX_LINK_MULTICAST_LEAVE:
            mac.octet[0] = 0;
            mac.octet[1] = 0;
            mac.octet[2] = 0;
            mac.octet[3] = 0;
            mac.octet[4] = 0;
            mac.octet[5] = 0;

            if ( wwd_wifi_unregister_multicast_address( &mac ) != WWD_SUCCESS )
            {
                driver->nx_ip_driver_status = (UINT) NX_NOT_SUCCESSFUL;
            }
            driver->nx_ip_driver_status = (UINT) NX_SUCCESS;
            break;

        case NX_LINK_GET_STATUS:
            /* Signal status through return pointer */
            *(driver -> nx_ip_driver_return_ptr) = (ULONG) ip->nx_ip_driver_link_up;
            driver->nx_ip_driver_status = (UINT) NX_SUCCESS;
            break;

        case NX_LINK_PACKET_SEND:
        case NX_LINK_ARP_RESPONSE_SEND:
        case NX_LINK_ARP_SEND:
        case NX_LINK_RARP_SEND:
        case NX_LINK_PACKET_BROADCAST:
        case NX_LINK_PTP_SEND:
            /* These cases require the link to be up, and will be processed below if it is up. */
            break;

        case NX_LINK_DEFERRED_PROCESSING:
        default:
            /* Invalid driver request */
            driver->nx_ip_driver_status = (UINT) NX_UNHANDLED_COMMAND;
            break;
    }

    /* Check if the link is up */
    if ( ( ip->nx_ip_driver_link_up == NX_TRUE ) && ( wwd_wifi_is_ready_to_transceive( interface ) == WWD_SUCCESS ) )
    {
        switch ( driver->nx_ip_driver_command )
        {
            case NX_LINK_PACKET_SEND:
                if (packet_ptr->nx_packet_ip_version == NX_IP_VERSION_V4)
                {
                    nx_wiced_add_ethernet_header( ip, packet_ptr, driver->nx_ip_driver_physical_address_msw, driver->nx_ip_driver_physical_address_lsw, (USHORT) WICED_ETHERTYPE_IPv4 );
                }
                else if (packet_ptr->nx_packet_ip_version == NX_IP_VERSION_V6)
                {
                    nx_wiced_add_ethernet_header( ip, packet_ptr, driver->nx_ip_driver_physical_address_msw, driver->nx_ip_driver_physical_address_lsw, (USHORT) WICED_ETHERTYPE_IPv6 );
                }
                else
                {
                    wiced_assert("Bad packet IP version", 0 != 0);
                    nx_packet_release(packet_ptr);
                    break;
                }
                wwd_network_send_ethernet_data( (wiced_buffer_t) packet_ptr, interface );
                driver->nx_ip_driver_status = (UINT) NX_SUCCESS;
                break;

            case NX_LINK_ARP_RESPONSE_SEND:
                nx_wiced_add_ethernet_header( ip, packet_ptr, driver->nx_ip_driver_physical_address_msw, driver->nx_ip_driver_physical_address_lsw, (USHORT) WICED_ETHERTYPE_ARP );
                wwd_network_send_ethernet_data( (wiced_buffer_t) packet_ptr, interface );
                driver->nx_ip_driver_status = (UINT) NX_SUCCESS;
                break;

            case NX_LINK_ARP_SEND:
                nx_wiced_add_ethernet_header( ip, packet_ptr, (ULONG) 0xFFFF, (ULONG) 0xFFFFFFFF, (USHORT) WICED_ETHERTYPE_ARP );
                wwd_network_send_ethernet_data( (wiced_buffer_t) packet_ptr, interface );
                driver->nx_ip_driver_status = (UINT) NX_SUCCESS;
                break;

            case NX_LINK_RARP_SEND:
                nx_wiced_add_ethernet_header( ip, packet_ptr, (ULONG) 0xFFFF, (ULONG) 0xFFFFFFFF, (USHORT) WICED_ETHERTYPE_RARP );
                wwd_network_send_ethernet_data( (wiced_buffer_t) packet_ptr, interface );
                driver->nx_ip_driver_status = (UINT) NX_SUCCESS;
                break;

            case NX_LINK_PACKET_BROADCAST:
                if (packet_ptr->nx_packet_ip_version == NX_IP_VERSION_V4)
                {
                    nx_wiced_add_ethernet_header( ip, packet_ptr, (ULONG) 0xFFFF, (ULONG) 0xFFFFFFFF, (USHORT) WICED_ETHERTYPE_IPv4 );
                }
                else if (packet_ptr->nx_packet_ip_version == NX_IP_VERSION_V6)
                {
                    nx_wiced_add_ethernet_header( ip, packet_ptr, (ULONG) 0xFFFF, (ULONG) 0xFFFFFFFF, (USHORT) WICED_ETHERTYPE_IPv6 );
                }
                else
                {
                    wiced_assert("Bad packet IP version", 0 != 0);
                    nx_packet_release(packet_ptr);
                    break;
                }
                wwd_network_send_ethernet_data( (wiced_buffer_t) packet_ptr, interface );
                driver->nx_ip_driver_status = (UINT) NX_SUCCESS;
                break;

            case NX_LINK_PTP_SEND:
                 if (packet_ptr->nx_packet_ip_version == NX_IP_VERSION_V4)
                 {
                     nx_wiced_add_ethernet_header( ip, packet_ptr, driver->nx_ip_driver_physical_address_msw, driver->nx_ip_driver_physical_address_lsw, (USHORT) WICED_ETHERTYPE_DOT1AS );
                 }
                 wwd_network_send_ethernet_data( (wiced_buffer_t) packet_ptr, interface );
                 driver->nx_ip_driver_status = (UINT) NX_SUCCESS;
                 break;

            case NX_LINK_INITIALIZE:
            case NX_LINK_ENABLE:
            case NX_LINK_DISABLE:
            case NX_LINK_MULTICAST_JOIN:
            case NX_LINK_MULTICAST_LEAVE:
            case NX_LINK_GET_STATUS:
            case NX_LINK_DEFERRED_PROCESSING:
            default:
                /* Handled in above case statement */
                break;
        }
    }
    else
    {
        /* Link is down, free any packet provided by the command */
        if ( packet_ptr != NULL )
        {
            switch (driver->nx_ip_driver_command)
            {
                case NX_LINK_PACKET_BROADCAST:
                case NX_LINK_RARP_SEND:
                case NX_LINK_ARP_SEND:
                case NX_LINK_ARP_RESPONSE_SEND:
                case NX_LINK_PACKET_SEND:
                    nx_packet_release(packet_ptr);
                    break;

                default:
                    break;
            }
        }
    }
}

static UINT nx_send_ethernet_packet( NX_IP_DRIVER* driver, USHORT ethertype )
{
    NX_IP*     ip = driver->nx_ip_driver_ptr;
    NX_PACKET* packet_ptr = driver->nx_ip_driver_packet;

    if ( packet_ptr == NULL )
    {
        return (UINT) NX_NOT_SUCCESSFUL;
    }

    if ( ip->nx_ip_driver_link_up != NX_TRUE )
    {
        nx_packet_release(packet_ptr);
        return (UINT) NX_NOT_SUCCESSFUL;
    }

    if ( platform_ethernet_is_ready_to_transceive() != WICED_TRUE )
    {
        nx_packet_release(packet_ptr);
        return (UINT) NX_NOT_SUCCESSFUL;
    }

    if ( (ethertype == WICED_ETHERTYPE_IPv4) && (packet_ptr->nx_packet_ip_version == NX_IP_VERSION_V6))
    {
        ethertype = (USHORT) WICED_ETHERTYPE_IPv6;
    }

    nx_wiced_add_ethernet_header( ip, packet_ptr, driver->nx_ip_driver_physical_address_msw, driver->nx_ip_driver_physical_address_lsw, ethertype );
    platform_ethernet_send_data( (wiced_buffer_t)packet_ptr );

    return (UINT) NX_SUCCESS;
}

VOID wiced_ethernet_netx_driver_entry( NX_IP_DRIVER* driver )
{
    platform_ethernet_config_t* eth_config;
    NX_IP*      ip = driver->nx_ip_driver_ptr;

    driver->nx_ip_driver_status = (UINT) NX_SUCCESS;

    switch ( driver->nx_ip_driver_command )
    {
        case NX_LINK_INITIALIZE:
            if ( platform_ethernet_get_config( &eth_config ) != PLATFORM_SUCCESS )
            {
                driver->nx_ip_driver_status = (UINT) NX_NOT_SUCCESSFUL;
                break;
            }

            IF_TO_IP(WWD_ETHERNET_INTERFACE) = ip;

            ip->nx_ip_arp_physical_address_msw = (ULONG) ( ( eth_config->mac_addr.octet[0] << 8 )  + eth_config->mac_addr.octet[1] );
            ip->nx_ip_arp_physical_address_lsw = (ULONG) ( ( eth_config->mac_addr.octet[2] << 24 ) + ( eth_config->mac_addr.octet[3] << 16 ) +
                                                           ( eth_config->mac_addr.octet[4] << 8 )  + eth_config->mac_addr.octet[5] );

            ip->nx_ip_driver_mtu            = (ULONG) WICED_PAYLOAD_MTU;
            ip->nx_ip_driver_mapping_needed = (UINT) NX_TRUE; /* initiate ARP requests */
            break;

        case NX_LINK_UNINITIALIZE:
            IF_TO_IP(WWD_ETHERNET_INTERFACE) = NULL;
            break;

        case NX_LINK_ENABLE:
            /* ip->nx_ip_driver_link_up would be set when driver detects link is up */
            break;

        case NX_LINK_DISABLE:
            ip->nx_ip_driver_link_up = NX_FALSE;
            break;

        case NX_LINK_GET_STATUS:
            /* Signal status through return pointer */
            *(driver -> nx_ip_driver_return_ptr) = (ULONG) ip->nx_ip_driver_link_up;
            break;

        case NX_LINK_MULTICAST_JOIN:
        case NX_LINK_MULTICAST_LEAVE:
            /* TODO: send IGMP packet? instruct Ethernet driver? */
            break;

        case NX_LINK_PACKET_BROADCAST:
        case NX_LINK_PACKET_SEND:
            driver->nx_ip_driver_status = nx_send_ethernet_packet( driver, (USHORT) WICED_ETHERTYPE_IPv4 );
            break;

        case NX_LINK_ARP_RESPONSE_SEND:
            driver->nx_ip_driver_status = nx_send_ethernet_packet( driver, (USHORT) WICED_ETHERTYPE_ARP );
            break;

        case NX_LINK_ARP_SEND:
            driver->nx_ip_driver_status = nx_send_ethernet_packet( driver, (USHORT) WICED_ETHERTYPE_ARP );
            if ( driver->nx_ip_driver_status != NX_SUCCESS )
            {
                /*
                 * ARP packet sending usually triggered by packet sending.
                 * Packet is waiting in ARP entry pending list till ARP request resolved.
                 * But we have just dropped ARP packet. As result data packet will be hanging for quite long
                 * in the list till ARP entry be found not resolved and deleted (tens of seconds).
                 * We can delete ARP entry, which frees all pending packets. Or we can just clear whole cache.
                 * Cache clearing is much simpler, but have other penalties.
                 */
                nx_arp_dynamic_entries_invalidate( ip );
            }
            break;

        case NX_LINK_RARP_SEND:
            driver->nx_ip_driver_status = nx_send_ethernet_packet( driver, (USHORT) WICED_ETHERTYPE_RARP );
            break;

        default:
            /* Invalid driver request */
            driver->nx_ip_driver_status = (UINT) NX_UNHANDLED_COMMAND;
            break;
    }
}

void host_network_process_ethernet_data( wiced_buffer_t buffer, wwd_interface_t interface )
{
    USHORT         ethertype;
    NX_PACKET*     packet_ptr = (NX_PACKET*) buffer;
    unsigned char* buff       = packet_ptr->nx_packet_prepend_ptr;

    if ( buff == NULL )
    {
        return;
    }

    /* Check if interface is valid, if not, drop frame */
    if ( interface > WWD_ETHERNET_INTERFACE )
    {
        wiced_assert( "invalid interface", 0 != 0 );
        nx_packet_release(packet_ptr);
        return;
    }

    ethertype = (USHORT)(buff[12] << 8 | buff[13]);

    /* Check if this is an 802.1Q VLAN tagged packet */
    if ( ethertype == WICED_ETHERTYPE_8021Q )
    {
        /* Need to remove the 4 octet VLAN Tag, by moving src and dest addresses 4 octets to the right,
         * and then read the actual ethertype. The VLAN ID and priority fields are currently ignored. */
        uint8_t temp_buffer[12];
        memcpy( temp_buffer, packet_ptr->nx_packet_prepend_ptr, 12 );
        memcpy( packet_ptr->nx_packet_prepend_ptr + 4, temp_buffer, 12 );

        packet_ptr->nx_packet_prepend_ptr = packet_ptr->nx_packet_prepend_ptr + 4;
        packet_ptr->nx_packet_length      = packet_ptr->nx_packet_length - 4;

        buff      = packet_ptr->nx_packet_prepend_ptr;
        ethertype = (USHORT) ( buff[12] << 8 | buff[13] );
    }

#ifdef ADD_NETX_EAPOL_SUPPORT
    if ( ethertype == WICED_ETHERTYPE_EAPOL )
    {
         /* pass it to the EAP layer, but do not release the packet */
        wwd_eapol_receive_eapol_packet( buffer, interface );
    }
    else
#endif
    {
        NX_IP* ip;

        if ( WICED_DEEP_SLEEP_IS_ENABLED() && ( WICED_DEEP_SLEEP_SAVE_PACKETS_NUM != 0 ) )
        {
            if ( wiced_deep_sleep_save_packet( packet_ptr, interface ) )
            {
                return;
            }
        }

        ip = IF_TO_IP(interface);

        /* Check if interface is not attached to IP instance, if so drop frame */
        if ( ip == NULL )
        {
            nx_packet_release(packet_ptr);
            return;
        }

        /* Remove the ethernet header, so packet is ready for reading by NetX */
        packet_ptr->nx_packet_prepend_ptr = packet_ptr->nx_packet_prepend_ptr + WICED_ETHERNET_SIZE;
        packet_ptr->nx_packet_length      = packet_ptr->nx_packet_length - WICED_ETHERNET_SIZE;

        if ( ( ethertype == WICED_ETHERTYPE_IPv4 ) || ethertype == WICED_ETHERTYPE_IPv6 )
        {
#ifdef WICED_FIX_UNICAST_DHCP_OFFERS
            NX_IP_HEADER*  ip_header  = (NX_IP_HEADER*)packet_ptr->nx_packet_prepend_ptr;
            NX_UDP_HEADER* udp_header = (NX_UDP_HEADER*)(packet_ptr -> nx_packet_prepend_ptr + sizeof(NX_IP_HEADER));
            UINT sender_port          = (UINT)(udp_header->nx_udp_header_word_0 >> 16);
            ULONG destination_ip      = ip_header->nx_ip_header_destination_ip;

            /* Check if a DHCP server has sent us a unicast packet instead of broadcast */
            if ( ( sender_port == 67 ) && ( destination_ip != 0xFFFFFFFF ) && ( destination_ip != ip_ptr->nx_ip_address ) )
            {
                ip_header->nx_ip_header_destination_ip = 0xFFFFFFFF;
            }
#endif

#ifdef NX_DIRECT_ISR_CALL
            _nx_ip_packet_receive(ip, packet_ptr);
#else
            _nx_ip_packet_deferred_receive( ip, packet_ptr );
#endif
        }
        else if ( ethertype == WICED_ETHERTYPE_ARP )
        {
            _nx_arp_packet_deferred_receive( ip, packet_ptr );
        }
        else if ( ethertype == WICED_ETHERTYPE_RARP )
        {
            _nx_rarp_packet_deferred_receive( ip, packet_ptr );
        }
        else
        {
            /* Unknown ethertype - just release the packet */
            nx_packet_release(packet_ptr);
        }
    }
}


UINT nx_rand16( void )
{
    uint16_t output;
    wiced_crypto_get_random( &output, 2 );
    return output;
}


/******************************************************************************
 Static functions
 *****************************************************************************/

static void nx_wiced_add_ethernet_header( NX_IP* ip_ptr_in, NX_PACKET* packet_ptr, ULONG destination_mac_msw, ULONG destination_mac_lsw, USHORT ethertype )
{
    ULONG* ethernet_header;

    /* Make space at the front of the packet buffer for the ethernet header */
    packet_ptr->nx_packet_prepend_ptr = packet_ptr->nx_packet_prepend_ptr - WICED_ETHERNET_SIZE;
    packet_ptr->nx_packet_length = packet_ptr->nx_packet_length + WICED_ETHERNET_SIZE;

    /* Ensure ethernet header writing starts with 32 bit alignment */
    ethernet_header = (ULONG *) ( packet_ptr->nx_packet_prepend_ptr - 2 );

    *ethernet_header = destination_mac_msw;
    *( ethernet_header + 1 ) = destination_mac_lsw;
    *( ethernet_header + 2 ) = ( ip_ptr_in->nx_ip_arp_physical_address_msw << 16 ) | ( ip_ptr_in->nx_ip_arp_physical_address_lsw >> 16 );
    *( ethernet_header + 3 ) = ( ip_ptr_in->nx_ip_arp_physical_address_lsw << 16 ) | ethertype;

    NX_CHANGE_ULONG_ENDIAN(*(ethernet_header));
    NX_CHANGE_ULONG_ENDIAN(*(ethernet_header+1));
    NX_CHANGE_ULONG_ENDIAN(*(ethernet_header+2));
    NX_CHANGE_ULONG_ENDIAN(*(ethernet_header+3));
}
