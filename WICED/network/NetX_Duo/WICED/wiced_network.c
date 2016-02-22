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
 *  Wiced NetX_Duo networking layer
 */

#include "wiced.h"
#include "wiced_network.h"
#include "wiced_wifi.h"
#include "wiced_utilities.h"
#include "wiced_deep_sleep.h"
#include "wwd_debug.h"
#include "wwd_assert.h"
#include "nx_api.h"
#include "nx_user.h"
#include "nx_icmp.h"
#include "wwd_management.h"
#include "wwd_network.h"
#include "dhcp_server.h"
#include "dns.h"
#include "platform_dct.h"
#include "platform_ethernet.h"
#include "internal/wiced_internal_api.h"
#include "wwd_network_constants.h"
#include "wiced_framework.h"
#include "wwd_buffer_interface.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define DRIVER_FOR_IF( interface )                ( wiced_ip_driver_entries[ ( interface ) & 3 ] )
#define STACK_FOR_IF( interface )                 ( wiced_ip_stack[          ( interface ) & 3 ] )
#define ARP_FOR_IF( interface )                   ( wiced_arp_cache[         ( interface ) & 3 ] )
#define DHCP_CLIENT_IS_INITIALISED( interface )   ( DHCP_HANDLE(interface).nx_dhcp_id == NX_DHCP_ID )
#define DHCP_HANDLE( interface )                  ( *wiced_dhcp_handle[   ( interface != WICED_ETHERNET_INTERFACE ? 0 : 1 ) ])
#define DHCP_CLIENT_HOSTNAME( interface )         ( dhcp_client_hostname[ ( interface != WICED_ETHERNET_INTERFACE ? 0 : 1 ) ])
#define AUTO_IP_INITIALISED( auto_ip_handle )     ( auto_ip_handle.nx_auto_ip_id == NX_AUTO_IP_ID )

/******************************************************
 *                    Constants
 ******************************************************/

#ifdef WICED_USE_ETHERNET_INTERFACE
    #ifdef WICED_ETHERNET_RX_PACKET_POOL_SIZE
        #ifdef RX_PACKET_POOL_SIZE
            #undef RX_PACKET_POOL_SIZE
        #endif /* RX_PACKET_POOL_SIZE */
        #define RX_PACKET_POOL_SIZE           WICED_ETHERNET_RX_PACKET_POOL_SIZE
    #elif !defined(RX_PACKET_POOL_SIZE)
        #define RX_PACKET_POOL_SIZE           (32 /* WWD default RX DMA ring size */ + 4 /* Ethernet default RX DMA ring size */ )
    #endif /* WICED_ETHERNET_RX_PACKET_POOL_SIZE  */
#endif /* WICED_USE_ETHERNET_INTERFACE */

#ifndef TX_PACKET_POOL_SIZE
#define TX_PACKET_POOL_SIZE         (7)
#endif

#ifndef RX_PACKET_POOL_SIZE
#define RX_PACKET_POOL_SIZE         (7)
#endif

#define NUM_BUFFERS_POOL_SIZE(x)    ((WICED_LINK_MTU_ALIGNED + sizeof(NX_PACKET)+1)*(x))

#define APP_TX_BUFFER_POOL_SIZE     NUM_BUFFERS_POOL_SIZE(TX_PACKET_POOL_SIZE)
#define APP_RX_BUFFER_POOL_SIZE     NUM_BUFFERS_POOL_SIZE(RX_PACKET_POOL_SIZE)

#define MAXIMUM_IP_ADDRESS_CHANGE_CALLBACKS           (2)

#define DHCP_DEFAULT_CLIENT_OBJECT_NAME            ((char*)"WICED DHCP Client")

#ifdef AUTO_IP_ENABLED
#define AUTO_IP_STACK_SIZE         (4096)
#define AUTO_IP_PRIORITY           (3)
#endif /* AUTO_IP_ENABLED */

#define MAX_LINK_LOCAL_IPV6_READY_ATTEMPS (10)
#define LINK_LOCAL_IPV6_ATTEMPT_INTERVAL   (500)

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

typedef struct
{
    wiced_ip_address_change_callback_t callback;
    void*                              arg;
} ip_address_change_callback_t;

/******************************************************
 *                 Static Variables
 ******************************************************/

/* consider instead of allocating more and then fixup pointer instruct compiler to align array */
static uint8_t tx_buffer_pool_memory [APP_TX_BUFFER_POOL_SIZE + PLATFORM_L1_CACHE_BYTES];
static uint8_t rx_buffer_pool_memory [APP_RX_BUFFER_POOL_SIZE + PLATFORM_L1_CACHE_BYTES];

#ifdef WICED_USE_WIFI_STA_INTERFACE
    static NX_IP            wifi_sta_ip_handle;
    static char             wifi_sta_ip_stack[IP_STACK_SIZE];
    static char             wifi_sta_arp_cache[ARP_CACHE_SIZE];
    static NX_DHCP          wifi_sta_dhcp_handle;
    static wiced_hostname_t wifi_sta_dhcp_client_hostname;
    #define WIFI_STA_IP_HANDLE             &wifi_sta_ip_handle
    #define WIFI_STA_IP_STACK              wifi_sta_ip_stack
    #define WIFI_STA_ARP_CACHE             wifi_sta_arp_cache
    #define WIFI_STA_DHCP_HANDLE           &wifi_sta_dhcp_handle
    #define WIFI_STA_DHCP_CLIENT_HOSTNAME  &wifi_sta_dhcp_client_hostname
#else
    #define WIFI_STA_IP_HANDLE            (NULL)
    #define WIFI_STA_IP_STACK             (NULL)
    #define WIFI_STA_ARP_CACHE            (NULL)
    #define WIFI_STA_DHCP_HANDLE          (NULL)
    #define WIFI_STA_DHCP_CLIENT_HOSTNAME (NULL)
#endif

#ifdef WICED_USE_WIFI_AP_INTERFACE
    static NX_IP wifi_ap_ip_handle;
    static char  wifi_ap_ip_stack[IP_STACK_SIZE];
    static char  wifi_ap_arp_cache[ARP_CACHE_SIZE];
    #define WIFI_AP_IP_HANDLE    &wifi_ap_ip_handle
    #define WIFI_AP_IP_STACK     wifi_ap_ip_stack
    #define WIFI_AP_ARP_CACHE    wifi_ap_arp_cache
#else
    #define WIFI_AP_IP_HANDLE    (NULL)
    #define WIFI_AP_IP_STACK     (NULL)
    #define WIFI_AP_ARP_CACHE    (NULL)
#endif

#ifdef WICED_USE_WIFI_P2P_INTERFACE
    static NX_IP wifi_p2p_ip_handle;
    static char  wifi_p2p_ip_stack[IP_STACK_SIZE];
    static char  wifi_p2p_arp_cache[ARP_CACHE_SIZE];
    #define WIFI_P2P_IP_HANDLE    &wifi_p2p_ip_handle
    #define WIFI_P2P_IP_STACK     wifi_p2p_ip_stack
    #define WIFI_P2P_ARP_CACHE    wifi_p2p_arp_cache
#else
    #define WIFI_P2P_IP_HANDLE    (NULL)
    #define WIFI_P2P_IP_STACK     (NULL)
    #define WIFI_P2P_ARP_CACHE    (NULL)
#endif

#ifdef WICED_USE_ETHERNET_INTERFACE
    static NX_IP             ethernet_ip_handle;
    static char              ethernet_ip_stack[IP_STACK_SIZE];
    static char              ethernet_arp_cache[ARP_CACHE_SIZE];
    static NX_DHCP           ethernet_dhcp_handle;
    static wiced_hostname_t  ethernet_dhcp_client_hostname;
    #define ETHERNET_IP_HANDLE             &ethernet_ip_handle
    #define ETHERNET_IP_STACK              ethernet_ip_stack
    #define ETHERNET_ARP_CACHE             ethernet_arp_cache
    #define ETHERNET_DHCP_HANDLE           &ethernet_dhcp_handle
    #define ETHERNET_DHCP_CLIENT_HOSTNAME  &ethernet_dhcp_client_hostname
#else
    #define ETHERNET_IP_HANDLE            (NULL)
    #define ETHERNET_IP_STACK             (NULL)
    #define ETHERNET_ARP_CACHE            (NULL)
    #define ETHERNET_DHCP_HANDLE          (NULL)
    #define ETHERNET_DHCP_CLIENT_HOSTNAME (NULL)
#endif

/* Network objects */
static char* wiced_ip_stack[ 4 ] =
{
    [WICED_STA_INTERFACE]      = WIFI_STA_IP_STACK,
    [WICED_AP_INTERFACE]       = WIFI_AP_IP_STACK,
    [WICED_P2P_INTERFACE]      = WIFI_P2P_IP_STACK,
    [WICED_ETHERNET_INTERFACE] = ETHERNET_IP_STACK,
};

static char* wiced_arp_cache[ 4 ] =
{
    [WICED_STA_INTERFACE]      = WIFI_STA_ARP_CACHE,
    [WICED_AP_INTERFACE]       = WIFI_AP_ARP_CACHE,
    [WICED_P2P_INTERFACE]      = WIFI_P2P_ARP_CACHE,
    [WICED_ETHERNET_INTERFACE] = ETHERNET_ARP_CACHE,
};

/*
 * Note: The DHCP related macros determine the mapping between interface and array entry
 */
static NX_DHCP* wiced_dhcp_handle[ 2 ] =
{
    [ 0 ] = WIFI_STA_DHCP_HANDLE,
    [ 1 ] = ETHERNET_DHCP_HANDLE,
};

/*
 * Note: The DHCP related macros determine the mapping between interface and array entry
 */
static wiced_hostname_t* dhcp_client_hostname[ 2 ] =
{
    [ 0 ] = WIFI_STA_DHCP_CLIENT_HOSTNAME,
    [ 1 ] = ETHERNET_DHCP_CLIENT_HOSTNAME,
};

/******************************************************
 *                 Global Variables
 ******************************************************/

NX_IP* wiced_ip_handle[ 4 ] =
{
    [WICED_STA_INTERFACE]      = WIFI_STA_IP_HANDLE,
    [WICED_AP_INTERFACE]       = WIFI_AP_IP_HANDLE,
    [WICED_P2P_INTERFACE]      = WIFI_P2P_IP_HANDLE,
    [WICED_ETHERNET_INTERFACE] = ETHERNET_IP_HANDLE,
};

NX_PACKET_POOL wiced_packet_pools[ 2 ]; /* 0=TX, 1=RX */

NX_PACKET_POOL wiced_application_tx_packet_pool;
NX_PACKET_POOL wiced_application_rx_packet_pool;

#ifdef AUTO_IP_ENABLED
static NX_AUTO_IP auto_ip_handle;
static uint8_t    auto_ip_stack[ AUTO_IP_STACK_SIZE ];
ULONG             original_auto_ip_address = 0;
#endif

/* One DHCP client handle for STA interface and one for Ethernet interface */

static wiced_dhcp_server_t internal_dhcp_server;

static void (* const wiced_ip_driver_entries[ 4 ])(struct NX_IP_DRIVER_STRUCT *) =
{
    [WICED_STA_INTERFACE] = wiced_sta_netx_duo_driver_entry,
    [WICED_AP_INTERFACE]  = wiced_ap_netx_duo_driver_entry,
    [WICED_P2P_INTERFACE] = wiced_p2p_netx_duo_driver_entry,

#ifdef WICED_USE_ETHERNET_INTERFACE
    [WICED_ETHERNET_INTERFACE] = wiced_ethernet_netx_driver_entry,
#endif
};

const wiced_ip_address_t INITIALISER_IPV4_ADDRESS( wiced_ip_broadcast, NX_IP_LIMITED_BROADCAST );

/* IP status callback variables */
static ip_address_change_callback_t wiced_ip_address_change_callbacks[MAXIMUM_IP_ADDRESS_CHANGE_CALLBACKS];

/* Network suspension variables */
static uint32_t     network_suspend_start_time  [ 4 ];
static uint32_t     network_suspend_end_time    [ 4 ];
static wiced_bool_t network_is_suspended = WICED_FALSE;

/* Wi-Fi power save state */
static uint8_t wifi_powersave_mode         = 0;
static uint16_t wifi_return_to_sleep_delay = 0;

/******************************************************
 *               Static Function Declarations
 ******************************************************/

static void           ip_address_changed_handler( NX_IP* ip_handle, VOID* additional_info );
static wiced_result_t dhcp_client_init( wiced_interface_t interface, NX_PACKET_POOL* packet_pool );
static wiced_result_t dhcp_client_deinit( wiced_interface_t interface );

static wiced_bool_t   tcp_sockets_are_closed( wiced_interface_t interface );
static wiced_result_t wiced_network_suspend_layers( wiced_interface_t interface );
static wiced_result_t wiced_network_resume_layers( wiced_interface_t interface );

static void           wiced_call_link_up_callbacks( wiced_interface_t interface );
static void           wiced_call_link_down_callbacks( wiced_interface_t interface );

static ULONG wiced_network_init_packet_pool( NX_PACKET_POOL* pool, const char* pool_name, uint8_t* memory_pointer, uint32_t memory_size );

static wiced_result_t wiced_ip_driver_notify( wiced_interface_t interface, wiced_bool_t up );

/******************************************************
 *               Function Definitions
 ******************************************************/

wiced_result_t wiced_network_init( void )
{
    /* Initialize the NetX system.  */
    WPRINT_NETWORK_INFO(("Initialising NetX_Duo " NetX_Duo_VERSION "\n"));
    nx_system_initialize( );

    memset( ip_networking_inited, 0, WICED_INTERFACE_MAX * sizeof(wiced_bool_t) );

    /* Create packet pools for transmit and receive */
    WPRINT_NETWORK_INFO(("Creating Packet pools\n"));
    if ( wiced_network_init_packet_pool( &wiced_packet_pools[0], "", tx_buffer_pool_memory, sizeof(tx_buffer_pool_memory) ) != NX_SUCCESS )
    {
        WPRINT_NETWORK_ERROR(("Couldn't create TX packet pool\n"));
        return WICED_ERROR;
    }
    if ( wiced_network_init_packet_pool( &wiced_packet_pools[1], "", rx_buffer_pool_memory, sizeof(rx_buffer_pool_memory) ) != NX_SUCCESS )
    {
        WPRINT_NETWORK_ERROR(("Couldn't create RX packet pool\n"));
        return WICED_ERROR;
    }
    if ( wwd_buffer_init( wiced_packet_pools ) != WWD_SUCCESS )
    {
        WPRINT_NETWORK_ERROR(("Could not initialize buffer interface\n"));
        return WICED_ERROR;
    }

    memset( &internal_dhcp_server, 0, sizeof( internal_dhcp_server ) );
    memset( wiced_ip_address_change_callbacks, 0, MAXIMUM_IP_ADDRESS_CHANGE_CALLBACKS * sizeof(wiced_ip_address_change_callback_t) );

    wiced_rtos_init_mutex( &link_subscribe_mutex );

    memset( link_up_callbacks_wireless,   0, sizeof( link_up_callbacks_wireless ) );
    memset( link_down_callbacks_wireless, 0, sizeof( link_down_callbacks_wireless ) );

#ifdef WICED_USE_ETHERNET_INTERFACE
    memset( link_up_callbacks_ethernet,   0, sizeof(link_up_callbacks_ethernet) );
    memset( link_down_callbacks_ethernet, 0, sizeof(link_down_callbacks_ethernet) );
#endif

    return WICED_SUCCESS;
}

wiced_result_t wiced_network_deinit( void )
{
#ifdef FEATURE_NX_IPV6
    tx_mutex_delete(&nx_nd_cache_protection);
#endif
    nx_packet_pool_delete(&wiced_packet_pools[0]);
    nx_packet_pool_delete(&wiced_packet_pools[1]);
    wiced_rtos_deinit_mutex( &link_subscribe_mutex );
    return WICED_SUCCESS;
}

wiced_result_t wiced_network_create_packet_pool( uint8_t* memory_pointer, uint32_t memory_size, wiced_network_packet_dir_t direction )
{
    wiced_result_t result = WICED_ERROR;

    if ( direction == WICED_NETWORK_PACKET_TX )
    {
        if ( wiced_network_init_packet_pool( &wiced_application_tx_packet_pool, "application_tx", memory_pointer, memory_size ) != NX_SUCCESS )
        {
            WPRINT_NETWORK_ERROR( ("Couldn't create TX packet pool\n") );
        }
        else
        {
            host_buffer_add_application_defined_pool( &wiced_application_tx_packet_pool, WWD_NETWORK_TX );
            result = WICED_SUCCESS;
        }
    }
    else if ( direction == WICED_NETWORK_PACKET_RX )
    {
        if ( wiced_network_init_packet_pool( &wiced_application_rx_packet_pool, "application_rx", memory_pointer, memory_size ) != NX_SUCCESS )
        {
            WPRINT_NETWORK_ERROR( ("Couldn't create RX packet pool\n") );
        }
        else
        {
            host_buffer_add_application_defined_pool( &wiced_application_rx_packet_pool, WWD_NETWORK_RX );
            result = WICED_SUCCESS;
        }
    }

    return result;
}

wiced_result_t wiced_ip_up( wiced_interface_t interface, wiced_network_config_t config, const wiced_ip_setting_t* ip_settings )
{
    UINT        status;
    UINT        ipv6_address_index;
    UINT        ipv6_interface_index;
    ULONG       ipv6_prefix;
    NXD_ADDRESS ipv6_address;
    uint8_t     ipv6_address_attempt = 0;

    if ( IP_NETWORK_IS_INITED(interface) )
    {
        return WICED_SUCCESS;
    }

    /* Enable the network interface  */
    if ( ( config == WICED_USE_STATIC_IP || config == WICED_USE_INTERNAL_DHCP_SERVER ) && ip_settings != NULL )
    {
        status = nx_ip_create( &IP_HANDLE(interface), (char*)"NetX IP", GET_IPV4_ADDRESS(ip_settings->ip_address), GET_IPV4_ADDRESS(ip_settings->netmask), &wiced_packet_pools[0], DRIVER_FOR_IF( interface ), STACK_FOR_IF( interface ), IP_STACK_SIZE, 2 );
        nx_ip_gateway_address_set( &IP_HANDLE(interface), GET_IPV4_ADDRESS(ip_settings->gateway) );
    }
    else
    {
        status = nx_ip_create( &IP_HANDLE(interface), (char*)"NetX IP", IP_ADDRESS(0, 0, 0, 0), 0xFFFFF000UL, &wiced_packet_pools[0], DRIVER_FOR_IF( interface ), STACK_FOR_IF( interface ), IP_STACK_SIZE, 2 );
    }

    if ( status != NX_SUCCESS )
    {
        WPRINT_NETWORK_ERROR( ( "Failed to create IP\n" ) );
        return WICED_ERROR;
    }

    if ( wiced_ip_driver_notify( interface, WICED_TRUE ) != WICED_SUCCESS )
    {
        WPRINT_NETWORK_ERROR( ( "Failed to notify driver\n" ) );
        goto driver_not_notified_leave_wifi_and_delete_ip;
    }

    /* Enable IPv6 */
    status = nxd_ipv6_enable( &IP_HANDLE(interface));

    /* Enable ARP */
    if ( nx_arp_enable( &IP_HANDLE(interface), (void *) ARP_FOR_IF( interface ), ARP_CACHE_SIZE ) != NX_SUCCESS )
    {
        WPRINT_NETWORK_ERROR( ( "Failed to enable ARP\n" ) );
        goto leave_wifi_and_delete_ip;
    }

    if ( nx_tcp_enable( &IP_HANDLE(interface) ) != NX_SUCCESS )
    {
        WPRINT_NETWORK_ERROR( ( "Failed to enable TCP\n" ) );
        goto leave_wifi_and_delete_ip;

    }

    if ( nx_udp_enable( &IP_HANDLE(interface) ) != NX_SUCCESS )
    {
        WPRINT_NETWORK_ERROR( ( "Failed to enable UDP\n" ) );
        goto leave_wifi_and_delete_ip;

    }

    if ( nxd_icmp_enable( &IP_HANDLE(interface) ) != NX_SUCCESS )
    {
        WPRINT_NETWORK_ERROR( ( "Failed to enable ICMP\n" ) );
        goto leave_wifi_and_delete_ip;

    }

    if ( nx_igmp_enable( &IP_HANDLE(interface) ) != NX_SUCCESS )
    {
        WPRINT_NETWORK_ERROR( ( "Failed to enable IGMP\n" ) );
        goto leave_wifi_and_delete_ip;

    }

    if ( nx_ip_fragment_enable( &IP_HANDLE(interface) ) != NX_SUCCESS )
    {
        WPRINT_NETWORK_ERROR(("Failed to enable IP fragmentation\n"));
        goto leave_wifi_and_delete_ip;
    }

    /*
     * Notifies application that networking is ready and push all saved during initialization packets up to stack.
     * We need to do it before DHCP client start to discover own address, because we need to push up DHCP packets and not preserve them.
     * For DHCP need to implement state restoring after deep-sleep, and below initialization need to be done after that.
     */
    if ( WICED_DEEP_SLEEP_IS_ENABLED() )
    {
        wiced_deep_sleep_set_networking_ready();
    }

    /* Obtain an IP address via DHCP if required */
    if ( config == WICED_USE_EXTERNAL_DHCP_SERVER )
    {
        WPRINT_NETWORK_INFO( ("Obtaining IPv4 address via DHCP\n") );

        if ( dhcp_client_init( interface, &wiced_packet_pools[0] ) != WICED_SUCCESS )
        {
            WPRINT_NETWORK_ERROR( ( "Failed to initialise DHCP client\n" ) );
            goto leave_wifi_and_delete_ip;
        }
    }
    else if ( config == WICED_USE_INTERNAL_DHCP_SERVER )
    {
        /* Create the DHCP Server.  */
        while ( IP_HANDLE(interface).nx_ip_driver_link_up == NX_FALSE ) // This case happens after p2p moves from group negotiation to starting the group owner
        {
            host_rtos_delay_milliseconds(10);
        }

        if ( wiced_start_dhcp_server( &internal_dhcp_server, interface ) != WICED_SUCCESS )
        {
            WPRINT_NETWORK_ERROR( ( "Failed to initialise DHCP server\r\n" ) );
            goto leave_wifi_and_delete_ip;
        }
    }


    /* Check for address resolution and wait for our addresses to be ready */
    status = nx_ip_status_check( &IP_HANDLE(interface), NX_IP_ADDRESS_RESOLVED, (ULONG *) &status, WICED_DHCP_IP_ADDRESS_RESOLUTION_TIMEOUT );

    if ( status == NX_SUCCESS )
    {
        ULONG ip_address, network_mask;
        nx_ip_address_get( &IP_HANDLE(interface), &ip_address, &network_mask );
        WPRINT_NETWORK_INFO( ( "IPv4 network ready IP: %u.%u.%u.%u\n", (unsigned char) ( ( ip_address >> 24 ) & 0xff ), (unsigned char) ( ( ip_address >> 16 ) & 0xff ), (unsigned char) ( ( ip_address >> 8 ) & 0xff ), (unsigned char) ( ( ip_address >> 0 ) & 0xff ) ) );

        /* Register a handler for any address changes */
        status = nx_ip_address_change_notify( &IP_HANDLE(interface), ip_address_changed_handler, NX_NULL );
        if ( status != NX_SUCCESS )
        {
            WPRINT_NETWORK_ERROR( ( "Unable to register for IPv4 address change callback\n" ) );
            goto leave_wifi_and_delete_ip;
        }
    }
    else
    {
#ifdef AUTO_IP_ENABLED
        ULONG ip_address, network_mask;
        int   tries = 0;
#endif
        if ( DHCP_CLIENT_IS_INITIALISED( interface) )
        {
            dhcp_client_deinit( interface );
        }
#ifdef AUTO_IP_ENABLED
        WPRINT_NETWORK_INFO(("Unable to obtain IP address via DHCP. Perform AUTO_IP\r\n"));

        /* Try to get link-local address, in case dhcp is failing */
        status =  nx_auto_ip_create( &auto_ip_handle, "AutoIP 0", &IP_HANDLE(interface), auto_ip_stack, 4096, AUTO_IP_PRIORITY );
        if ( status != NX_SUCCESS )
        {
            nx_auto_ip_delete( &auto_ip_handle );
            goto leave_wifi_and_delete_ip;
        }
        status =  nx_auto_ip_start( &auto_ip_handle, original_auto_ip_address );
        if ( status != NX_SUCCESS )
        {
            nx_auto_ip_delete( &auto_ip_handle );
            goto leave_wifi_and_delete_ip;
        }

        /* Wait for AUTO_IP address to be resolved.   */
        do
        {
            /* Call IP status check routine.   */
            status =  nx_ip_status_check( &IP_HANDLE(interface), NX_IP_ADDRESS_RESOLVED, (ULONG *) &status, WICED_AUTO_IP_ADDRESS_RESOLUTION_TIMEOUT );
        } while ( ( status != NX_SUCCESS ) && ( tries++ < 5 ) );

        if ( status != NX_SUCCESS )
        {
            nx_auto_ip_stop( &auto_ip_handle );
            nx_auto_ip_delete( &auto_ip_handle );
            goto leave_wifi_and_delete_ip;
        }

        nx_ip_address_get( &IP_HANDLE(interface), &ip_address, &network_mask );
        WPRINT_NETWORK_INFO( ( "IPv4  AUTO_IP network ready IP: %u.%u.%u.%u\n", (unsigned char) ( ( ip_address >> 24 ) & 0xff ), (unsigned char) ( ( ip_address >> 16 ) & 0xff ), (unsigned char) ( ( ip_address >> 8 ) & 0xff ), (unsigned char) ( ( ip_address >> 0 ) & 0xff ) ) );

        /* Register a handler for any address changes */
        status = nx_ip_address_change_notify( &IP_HANDLE(interface), ip_address_changed_handler, &auto_ip_handle );
        if ( status != NX_SUCCESS )
        {
            WPRINT_NETWORK_ERROR( ( "Unable to register for IPv4 address change callback\n" ) );
            goto leave_wifi_and_delete_ip;
        }

#else

        WPRINT_NETWORK_INFO(("Unable to obtain IP address via DHCP\r\n"));
        goto leave_wifi_and_delete_ip;
#endif
    }

    if ( config == WICED_USE_EXTERNAL_DHCP_SERVER )
    {
        UCHAR              dns_ip_string[NX_DHCP_NUM_DNS_SERVERS*4]; /* NX_DHCP_NUM_DNS_SERVERS is the maximum number of DNS servers that will be read from the DHCP packet */
        UINT               size;
        wiced_ip_address_t address;
        UINT               i = 0;
        UINT               dns_address_count;
        ULONG              *dns_address_ptr;

        /* Obtain the IP addresses of the DNS servers. */
        size = sizeof( dns_ip_string );
        if ( nx_dhcp_user_option_retrieve( &DHCP_HANDLE(interface), NX_DHCP_OPTION_DNS_SVR, dns_ip_string, &size ) == NX_SUCCESS )
        {
            /* Calculate the DNS Server address count.  */
            dns_address_count = size / sizeof(ULONG);

            /* Output the DNS Server addresses.  */
            for(i = 0; i < dns_address_count; i++)
            {
                /* Set the DNS address pointer.  */
                dns_address_ptr = (ULONG *)(dns_ip_string + i * sizeof(ULONG));

                /* Add gateway DNS server */
                SET_IPV4_ADDRESS( address, *dns_address_ptr ); /* The DHCP client code now reverses the byte order when it reads the options from the packet so no conversion is required. */
                dns_client_add_server_address( address );
            }
        }

        /* Add Google DNS server (8.8.8.8) */
        memset( dns_ip_string, 8, 4 );
        SET_IPV4_ADDRESS( address, nx_dhcp_user_option_convert( dns_ip_string ) );
        dns_client_add_server_address( address );
    }

    /* Set the IPv6 linklocal address using our MAC */
    WPRINT_NETWORK_INFO( ("Setting IPv6 link-local address\n") );

    nxd_ipv6_address_set( &IP_HANDLE(interface), 0, NX_NULL, 10, &ipv6_address_index );

    /* Wait until the link-local address is properly advertised using network solicitation frame
     * and nobody on the local network complains.
     * If the address is not valid after MAX_LINK_LOCAL_IPV6_READY_ATTEMPS disable ipv6 */
    do
    {
        if ( IP_HANDLE(interface).nx_ipv6_address[ ipv6_address_index ].nxd_ipv6_address_state == NX_IPV6_ADDR_STATE_VALID )
        {
            uint16_t* ipv6 = (uint16_t*)ipv6_address.nxd_ip_address.v6;
            nxd_ipv6_address_get( &IP_HANDLE(interface), ipv6_address_index, &ipv6_address, &ipv6_prefix, &ipv6_interface_index );
            WPRINT_NETWORK_INFO( ( "IPv6 network ready IP: %.4X:%.4X:%.4X:%.4X:%.4X:%.4X:%.4X:%.4X\n",
                                   (unsigned int) ( ( ipv6[ 1 ] ) ),
                                   (unsigned int) ( ( ipv6[ 0 ] ) ),
                                   (unsigned int) ( ( ipv6[ 3 ] ) ),
                                   (unsigned int) ( ( ipv6[ 2 ] ) ),
                                   (unsigned int) ( ( ipv6[ 5 ] ) ),
                                   (unsigned int) ( ( ipv6[ 4 ] ) ),
                                   (unsigned int) ( ( ipv6[ 7 ] ) ),
                                   (unsigned int) ( ( ipv6[ 6 ] ) ) ) );
            break;
        }
        host_rtos_delay_milliseconds( LINK_LOCAL_IPV6_ATTEMPT_INTERVAL );
        ipv6_address_attempt++;
    }
    while ( ipv6_address_attempt < MAX_LINK_LOCAL_IPV6_READY_ATTEMPS );

    if ( IP_HANDLE(interface).nx_ipv6_address[ ipv6_address_index ].nxd_ipv6_address_state != NX_IPV6_ADDR_STATE_VALID )
    {
        WPRINT_NETWORK_INFO(("IPv6 network is not ready\r\n"));
        nxd_ipv6_disable( &IP_HANDLE(interface) );
    }



    SET_IP_NETWORK_INITED(interface, WICED_TRUE);

    return WICED_SUCCESS;

leave_wifi_and_delete_ip:
    wiced_ip_driver_notify( interface, WICED_FALSE );
driver_not_notified_leave_wifi_and_delete_ip:
    if ( interface == WICED_STA_INTERFACE )
    {
        wiced_leave_ap( interface );
    }
    nx_ip_delete( &IP_HANDLE(interface));
    return WICED_ERROR;
}

wiced_result_t wiced_ip_down( wiced_interface_t interface )
{
    if ( IP_NETWORK_IS_INITED(interface) )
    {
        /* Cleanup DHCP & DNS */
        if ( ( interface == WICED_AP_INTERFACE ) || ( interface == WICED_CONFIG_INTERFACE )
                                                 || ( ( interface == WICED_P2P_INTERFACE ) && ( wwd_wifi_p2p_go_is_up == WICED_TRUE ) ) )
        {
            wiced_stop_dhcp_server( &internal_dhcp_server );
        }
        else /* STA or Ethernet interface */
        {
            if ( DHCP_CLIENT_IS_INITIALISED( interface) )
            {
                dhcp_client_deinit( interface );
            }
            dns_client_remove_all_server_addresses( );
        }

#ifdef AUTO_IP_ENABLED
        if ( AUTO_IP_INITIALISED(auto_ip_handle) )
        {
            nx_auto_ip_get_address( &auto_ip_handle, &original_auto_ip_address );
            nx_auto_ip_stop( &auto_ip_handle );
            nx_auto_ip_delete( &auto_ip_handle );
        }
#endif /* WICED_AUTO_IP_ENABLE */

        /* Tell driver interface went down */
        wiced_ip_driver_notify( interface, WICED_FALSE );

        /* Delete the network interface */
        if ( nx_ip_delete( &IP_HANDLE(interface) ) != NX_SUCCESS)
        {
            WPRINT_NETWORK_ERROR( ( "Could not delete IP instance\n" ) );
        }
        memset( &IP_HANDLE(interface), 0, sizeof(NX_IP));

        SET_IP_NETWORK_INITED(interface, WICED_FALSE);
    }

    return WICED_SUCCESS;
}


wiced_result_t wiced_network_suspend( void )
{
    wiced_assert( "Network is already suspended", network_is_suspended == WICED_FALSE );

    if ( network_is_suspended == WICED_TRUE )
    {
        return WICED_SUCCESS;
    }

#ifdef WICED_USE_ETHERNET_INTERFACE
    if( wiced_network_is_up( WICED_ETHERNET_INTERFACE ) )
    {
        if( wiced_network_suspend_layers( WICED_ETHERNET_INTERFACE ) != WICED_SUCCESS )
        {
            return WICED_ERROR;
        }
    }
#endif

    if( wiced_network_is_up( WICED_STA_INTERFACE ) )
    {
        if( wiced_network_suspend_layers( WICED_STA_INTERFACE ) != WICED_SUCCESS )
        {
            return WICED_ERROR;
        }
    }

    network_is_suspended = WICED_TRUE;

    return WICED_SUCCESS;
}

wiced_result_t wiced_network_resume( void )
{
    wiced_assert( "Network was not suspended previously", network_is_suspended == WICED_TRUE );

    /* Ensure network was previously suspended */
    if ( network_is_suspended != WICED_TRUE )
    {
        return WICED_SUCCESS;
    }

#ifdef WICED_USE_ETHERNET_INTERFACE
    if( wiced_network_is_up( WICED_ETHERNET_INTERFACE ) )
    {
        if( wiced_network_resume_layers( WICED_ETHERNET_INTERFACE ) == WICED_ERROR )
        {
            return WICED_ERROR;
        }
    }
#endif

    if( wiced_network_is_up( WICED_STA_INTERFACE ) )
    {
        if( wiced_network_resume_layers( WICED_STA_INTERFACE ) == WICED_ERROR )
        {
            return WICED_ERROR;
        }
    }

    network_is_suspended = WICED_FALSE;

    return WICED_SUCCESS;
}

void wiced_network_notify_link_up( wiced_interface_t interface )
{
    IP_HANDLE(interface).nx_ip_driver_link_up = NX_TRUE;
}

void wiced_network_notify_link_down( wiced_interface_t interface )
{
    IP_HANDLE(interface).nx_ip_driver_link_up = NX_FALSE;
}

static void wiced_call_link_down_callbacks( wiced_interface_t interface )
{
    int i;

    for ( i = 0; i < WICED_MAXIMUM_LINK_CALLBACK_SUBSCRIPTIONS; i++ )
    {
        if ( (LINK_DOWN_CALLBACKS_LIST( interface ))[i] != NULL )
        {
            (LINK_DOWN_CALLBACKS_LIST( interface ))[i]( );
        }
    }
}

static void wiced_call_link_up_callbacks( wiced_interface_t interface )
{
    int i;

    for ( i = 0; i < WICED_MAXIMUM_LINK_CALLBACK_SUBSCRIPTIONS; i++ )
    {
        if ( (LINK_UP_CALLBACKS_LIST( interface ))[i] != NULL )
        {
            (LINK_UP_CALLBACKS_LIST( interface ))[i]();
        }
    }
}

wiced_result_t wiced_wireless_link_down_handler( void* arg )
{
    const wiced_interface_t interface = WICED_STA_INTERFACE;
    wiced_result_t          result    = WICED_SUCCESS;

    UNUSED_PARAMETER( arg );

    WPRINT_NETWORK_DEBUG( ("Wireless link DOWN!\n\r") );

    if ( DHCP_CLIENT_IS_INITIALISED( interface ) )
    {
        UINT res = nx_dhcp_stop( &DHCP_HANDLE( interface ) );

        if ( ( res != NX_SUCCESS ) && ( res != NX_DHCP_NOT_STARTED ) )
        {
            WPRINT_NETWORK_ERROR( ("Stopping DHCP failed!\n\r") );
            result = WICED_ERROR;
        }
    }

    if ( nx_arp_dynamic_entries_invalidate( &IP_HANDLE(  interface  ) ) != NX_SUCCESS )
    {
        WPRINT_NETWORK_ERROR( ("Clearing ARP cache failed!\n\r") );
        result = WICED_ERROR;
    }

    /* Inform all subscribers about an event */
    wiced_call_link_down_callbacks( interface );

    /* Kick the radio chip if it's in power save mode in case the link down event is due to missing beacons. Setting the chip to the same power save mode is sufficient. */
    wifi_powersave_mode = wiced_wifi_get_powersave_mode();
    if ( wifi_powersave_mode == PM1_POWERSAVE_MODE )
    {
        wiced_wifi_enable_powersave();
    }
    else if ( wifi_powersave_mode == PM2_POWERSAVE_MODE )
    {
        wifi_return_to_sleep_delay = wiced_wifi_get_return_to_sleep_delay();
        wiced_wifi_enable_powersave_with_throughput( wifi_return_to_sleep_delay );
    }

    return result;
}

#ifdef WICED_USE_ETHERNET_INTERFACE
wiced_result_t wiced_ethernet_link_down_handler( void )
{
    const wiced_interface_t interface = WICED_ETHERNET_INTERFACE;
    wiced_result_t          result    = WICED_SUCCESS;

    WPRINT_NETWORK_DEBUG( ("Ethernet link DOWN!\n\r") );

    if ( DHCP_CLIENT_IS_INITIALISED( interface ) )
    {
        UINT res = nx_dhcp_stop( &DHCP_HANDLE( interface ) );

        if ( ( res != NX_SUCCESS ) && ( res != NX_DHCP_NOT_STARTED ) )
        {
            WPRINT_NETWORK_ERROR( ("Stopping DHCP failed!\n\r") );
            result = WICED_ERROR;
        }
    }

    if ( nx_arp_dynamic_entries_invalidate( &IP_HANDLE(  interface  ) ) != NX_SUCCESS )
    {
        WPRINT_NETWORK_ERROR( ("Clearing ARP cache failed!\n\r") );
        result = WICED_ERROR;
    }

    /* Inform all subscribers about an event */
    wiced_call_link_down_callbacks( interface );

    return result;
}
#endif

wiced_result_t wiced_wireless_link_up_handler( void* arg )
{
    const wiced_interface_t interface = WICED_STA_INTERFACE;
    wiced_result_t          result    = WICED_SUCCESS;

    UNUSED_PARAMETER( arg );

    WPRINT_NETWORK_DEBUG( ("Wireless link UP!\n") );

    if ( DHCP_CLIENT_IS_INITIALISED( interface ) )
    {
        UINT res;

        /* Save the current power save state */
        wifi_powersave_mode = wiced_wifi_get_powersave_mode();
        wifi_return_to_sleep_delay = wiced_wifi_get_return_to_sleep_delay();

        /* Disable power save for the DHCP exchange */
        if ( wifi_powersave_mode != NO_POWERSAVE_MODE )
        {
            wiced_wifi_disable_powersave( );
        }

        res = nx_dhcp_start( &DHCP_HANDLE( interface ) );
        if ( res == NX_DHCP_ALREADY_STARTED )
        {
            nx_dhcp_force_renew( &DHCP_HANDLE( interface ) );
        }
        else if ( res != NX_SUCCESS )
        {
            WPRINT_NETWORK_ERROR( ( "Failed to initiate DHCP transaction\n" ) );
            result = WICED_ERROR;
        }

        /* Wait a little to allow DHCP a chance to complete, but we can't block here */
        host_rtos_delay_milliseconds(10);

        if ( wifi_powersave_mode == PM1_POWERSAVE_MODE )
        {
            wiced_wifi_enable_powersave();
        }
        else if ( wifi_powersave_mode == PM2_POWERSAVE_MODE )
        {
            wiced_wifi_enable_powersave_with_throughput( wifi_return_to_sleep_delay );
        }
    }

    /* Inform all subscribers about an event */
    wiced_call_link_up_callbacks( interface );

    return result;
}

#ifdef WICED_USE_ETHERNET_INTERFACE
wiced_result_t wiced_ethernet_link_up_handler( void )
{
    const wiced_interface_t interface = WICED_ETHERNET_INTERFACE;
    wiced_result_t          result    = WICED_SUCCESS;

    WPRINT_NETWORK_DEBUG( ("Ethernet link UP!\n\r") );

    if ( DHCP_CLIENT_IS_INITIALISED( interface ) )
    {
        UINT res;

        res = nx_dhcp_start( &DHCP_HANDLE( interface ) );
        if ( res == NX_DHCP_ALREADY_STARTED )
        {
            nx_dhcp_force_renew( &DHCP_HANDLE( interface ) );
        }
        else if ( res != NX_SUCCESS )
        {
            WPRINT_NETWORK_ERROR( ( "Failed to initiate DHCP transaction\n" ) );
            result = WICED_ERROR;
        }
    }

    /* Inform all subscribers about an event */
    wiced_call_link_up_callbacks( interface );

    return result;
}
#endif

wiced_result_t wiced_wireless_link_renew_handler( void* arg )
{
    wiced_result_t result = WICED_SUCCESS;
    wiced_ip_address_t ipv4_address;

    UNUSED_PARAMETER( arg );

    wiced_ip_get_gateway_address( WICED_STA_INTERFACE, &ipv4_address );

    if ( nx_arp_dynamic_entry_set( &IP_HANDLE( WICED_STA_INTERFACE ), IP_ADDRESS((unsigned int)((GET_IPV4_ADDRESS(ipv4_address) >> 24) & 0xFF),
                        (unsigned int)((GET_IPV4_ADDRESS(ipv4_address) >> 16) & 0xFF),
                        (unsigned int)((GET_IPV4_ADDRESS(ipv4_address) >> 8) & 0xFF),
                        (unsigned int)((GET_IPV4_ADDRESS(ipv4_address) >> 0) & 0xFF)), 0x0, 0x0 ) != NX_SUCCESS )
    {
        WPRINT_NETWORK_ERROR( ("Dynamically changing the ARP entry failed!\n\r") );
        result = WICED_ERROR;

    }
    /* Try do a DHCP renew. This may not be successful if we've had a link down event previously */
    if ( DHCP_CLIENT_IS_INITIALISED( WICED_STA_INTERFACE ) )
    {
        nx_dhcp_force_renew( &DHCP_HANDLE( WICED_STA_INTERFACE ) );
    }

    return result;
}

wiced_result_t wiced_ip_register_address_change_callback( wiced_ip_address_change_callback_t callback, void* arg )
{
    uint8_t i;
    for ( i = 0; i < MAXIMUM_IP_ADDRESS_CHANGE_CALLBACKS; i++ )
    {
        if ( wiced_ip_address_change_callbacks[i].callback == NULL )
        {
            wiced_ip_address_change_callbacks[i].callback = callback;
            wiced_ip_address_change_callbacks[i].arg = arg;
            return WICED_SUCCESS;
        }
    }

    WPRINT_NETWORK_ERROR( ( "Out of callback storage space\n" ) );

    return WICED_ERROR;
}

wiced_result_t wiced_ip_deregister_address_change_callback( wiced_ip_address_change_callback_t callback )
{
    uint8_t i;
    for ( i = 0; i < MAXIMUM_IP_ADDRESS_CHANGE_CALLBACKS; i++ )
    {
        if ( wiced_ip_address_change_callbacks[i].callback == callback )
        {
            memset( &wiced_ip_address_change_callbacks[i], 0, sizeof( wiced_ip_address_change_callback_t ) );
            return WICED_SUCCESS;
        }
    }

    WPRINT_NETWORK_ERROR( ( "Unable to find callback to deregister\n" ) );

    return WICED_ERROR;
}

wiced_bool_t wiced_ip_is_any_pending_packets( wiced_interface_t interface )
{
    NX_IP* ip_handle;
    ULONG  i;

    if ( !IP_NETWORK_IS_INITED(interface) )
    {
        return WICED_FALSE;
    }

    ip_handle = &IP_HANDLE(interface);

    for ( i = 0; i < ip_handle->nx_ip_arp_total_entries; i++ )
    {
        if ( ip_handle->nx_ip_arp_cache_memory[i].nx_arp_packets_waiting )
        {
            return WICED_TRUE;
        }
    }

#ifndef NX_DISABLE_FRAGMENTATION
    if ( ip_handle->nx_ip_received_fragment_head != NULL )
    {
        return WICED_TRUE;
    }
#endif

    return WICED_FALSE;
}

/******************************************************
 *            Static Function Definitions
 ******************************************************/

static wiced_result_t dhcp_client_init( wiced_interface_t interface, NX_PACKET_POOL* packet_pool )
{
    wiced_result_t    result;
    wiced_hostname_t* dhcp_hostname = DHCP_CLIENT_HOSTNAME(interface);
    NX_IP*            ip_handle     = &IP_HANDLE(interface);
    NX_DHCP*          dhcp_handle   = &DHCP_HANDLE(interface);

    /* get hostname */
    result = wiced_network_get_hostname( dhcp_hostname );
    if ( result != WICED_SUCCESS )
    {
        return result;
    }
    WPRINT_NETWORK_INFO( ("DHCP CLIENT hostname %s\n", dhcp_hostname->value) );

    /* clear DHCP info to start */
    memset( dhcp_handle, 0, sizeof( *dhcp_handle ) );

    /* Create the DHCP instance. */
    if ( nx_dhcp_create( dhcp_handle, ip_handle, dhcp_hostname->value ) != NX_SUCCESS )
    {
        WPRINT_NETWORK_ERROR( ( "Failed to Create DHCP thread\n" ) );
        return WICED_ERROR;
    }

    nx_dhcp_packet_pool_set( dhcp_handle, packet_pool );

    nx_dhcp_request_client_ip(dhcp_handle, ip_handle->nx_ip_address, NX_TRUE);

    /* Start DHCP. */
    if ( nx_dhcp_start( dhcp_handle ) != NX_SUCCESS )
    {
        WPRINT_NETWORK_ERROR( ( "Failed to initiate DHCP transaction\n" ) );
        return WICED_ERROR;
    }

    return WICED_SUCCESS;
}

static wiced_result_t dhcp_client_deinit( wiced_interface_t interface )
{
    NX_DHCP* dhcp_handle = &DHCP_HANDLE(interface);
    UINT     res;

    res = nx_dhcp_stop( dhcp_handle );
    if ( ( res != NX_SUCCESS ) && ( res != NX_DHCP_NOT_STARTED ) )
    {
        WPRINT_NETWORK_ERROR( ( "Failed to stop DHCP client\n" ) );
    }

#ifdef NETX_DHCP_CLIENT_DOESNT_DELETE_PACKET_POOL
    nx_dhcp_delete();
#else
    /* Check for AutoIP address.  */
    if ( ( dhcp_handle->nx_dhcp_ip_address & NX_AUTO_IP_ADDRESS_MASK ) != NX_AUTO_IP_ADDRESS )
    {
        /* Clear the IP address and the subnet mask.   */
        //_nx_dhcp_ip_address_set( dhcp_handle, 0, 0 );

        /* Clear the Gateway/Router IP address.  */
        nx_ip_gateway_address_set( dhcp_handle->nx_dhcp_ip_ptr, 0 );
    }

    /* Terminate the DHCP processing thread.  */
    tx_thread_terminate( &( dhcp_handle->nx_dhcp_thread ) );

    /* Delete the DHCP processing thread.  */
    tx_thread_delete( &( dhcp_handle->nx_dhcp_thread ) );

    /* Delete the DHCP mutex.  */
    tx_mutex_delete( &( dhcp_handle->nx_dhcp_mutex ) );

    /* Delete the UDP socket.  */
    nx_udp_socket_delete( &( dhcp_handle->nx_dhcp_socket ) );

    /* Clear the dhcp structure ID. */
    dhcp_handle->nx_dhcp_id = 0;
#endif

    /* Clear the dhcp handle structure and name array */
    memset( dhcp_handle->nx_dhcp_name, 0x00, ( HOSTNAME_SIZE + 1 ) );
    memset( dhcp_handle, 0, sizeof( *dhcp_handle ) );

    return WICED_SUCCESS;
}

static void ip_address_changed_handler( NX_IP *ip_ptr, VOID *additional_info )
{
    uint8_t i;

    UNUSED_PARAMETER( ip_ptr );
    UNUSED_PARAMETER( additional_info );

#ifdef AUTO_IP_ENABLED
    {
        ULONG ip_address;
        ULONG network_mask;
        NX_AUTO_IP* handle =  (NX_AUTO_IP*) additional_info;

        nx_ip_address_get(ip_ptr, &ip_address, &network_mask);
        if ( ip_address == 0 && ( ( original_auto_ip_address & 0xFFFF0000UL ) == IP_ADDRESS(169, 254, 0, 0) ) )
        {
            nx_auto_ip_get_address(handle, &ip_address);
            nx_auto_ip_start(handle, ip_address);
        }
    }
#endif /* AUTO_IP_ENABLED */

    for ( i = 0; i < MAXIMUM_IP_ADDRESS_CHANGE_CALLBACKS; i++ )
    {
        if ( wiced_ip_address_change_callbacks[i].callback != NULL )
        {
            ( *wiced_ip_address_change_callbacks[i].callback )( wiced_ip_address_change_callbacks[i].arg );
        }
    }
}

static wiced_bool_t tcp_sockets_are_closed( wiced_interface_t interface )
{
    ULONG tcp_connections = 0;
    UINT  result;

    result = nx_tcp_info_get( &IP_HANDLE(interface), NX_NULL, NX_NULL, NX_NULL, NX_NULL, NX_NULL, NX_NULL, NX_NULL, &tcp_connections, NX_NULL, NX_NULL, NX_NULL);
    if ( result == NX_SUCCESS && tcp_connections == 0)
    {
        return WICED_TRUE;
    }
    return WICED_FALSE;
}

static ULONG wiced_network_init_packet_pool( NX_PACKET_POOL* pool, const char* pool_name, uint8_t* memory_pointer, uint32_t memory_size )
{
    void* memory_pointer_aligned = PLATFORM_L1_CACHE_PTR_ROUND_UP( memory_pointer );
    uint32_t memory_size_aligned = PLATFORM_L1_CACHE_ROUND_DOWN  ( memory_size - ( (uint32_t)memory_pointer_aligned - (uint32_t)memory_pointer ) );

    wiced_static_assert(packet_header_not_cache_aligned, sizeof(NX_PACKET) == PLATFORM_L1_CACHE_ROUND_UP(sizeof(NX_PACKET)));

    return nx_packet_pool_create( pool, pool_name, WICED_LINK_MTU_ALIGNED, memory_pointer_aligned, memory_size_aligned );
}

static wiced_result_t wiced_network_suspend_layers( wiced_interface_t interface )
{
    /* Ensure all current TCP sockets are closed */
    if ( tcp_sockets_are_closed( interface ) != WICED_TRUE )
    {
        return WICED_ERROR;
    }

    if ( DHCP_CLIENT_IS_INITIALISED( interface) )
    {
        if ( DHCP_HANDLE(interface).nx_dhcp_state != NX_DHCP_STATE_BOUND )
        {
            return WICED_ERROR;
        }
    }

    /* Suspend IP layer. This will deactivate IP layer periodic timers */
    if ( nx_ip_suspend( &IP_HANDLE(interface) ) != TX_SUCCESS )
    {
        return WICED_ERROR;
    }

    /* Suspend DHCP client */
    if ( DHCP_CLIENT_IS_INITIALISED( interface) )
    {
        if ( nx_dhcp_suspend( &DHCP_HANDLE(interface) ) != TX_SUCCESS )
        {
            return WICED_ERROR;
        }
    }

    /* TODO: Suspend IGMP */

    /* Suspend TCP. This will deactivate tcp fast periodic timer processing */
    if ( nx_tcp_suspend( &IP_HANDLE(interface) ) != NX_SUCCESS )
    {
        return WICED_ERROR;
    }

    /* Remember when the network was suspended, it will be used to update the DHCP lease time */
    if ( wiced_time_get_time( &network_suspend_start_time[WICED_TO_WWD_INTERFACE(interface)] ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }

    return WICED_SUCCESS;
}

static wiced_result_t wiced_network_resume_layers( wiced_interface_t interface )
{
    uint32_t number_of_ticks_network_was_suspended;

    /* Resume DHCP */
    if ( DHCP_CLIENT_IS_INITIALISED( interface) )
    {
        if ( nx_dhcp_resume( &DHCP_HANDLE(interface) ) != TX_SUCCESS )
        {
            wiced_assert("WICED can't resume DHCP client", 0 != 0 );
            return WICED_ERROR;
        }
    }

    /* Resume IP timers */
    if ( nx_ip_resume( &IP_HANDLE(interface)) != TX_SUCCESS )
    {
        wiced_assert("WICED can't resume IP timers", 0 != 0 );
        return WICED_ERROR;
    }

    /* TODO: Resume IGMP */

    /* Resume TCP */
    if ( nx_tcp_resume( &IP_HANDLE(interface)) != NX_SUCCESS )
    {
        wiced_assert("WICED can't resume TCP timers", 0 != 0 );
        return WICED_ERROR;
    }

    /* Calculate the length of time we were suspended */
    if ( wiced_time_get_time( &network_suspend_end_time[WICED_TO_WWD_INTERFACE(interface)] ) != WICED_SUCCESS )
    {
        wiced_assert("Error getting system time", 0 != 0 );
        return WICED_ERROR;
    }
    number_of_ticks_network_was_suspended = network_suspend_end_time[WICED_TO_WWD_INTERFACE(interface)] - network_suspend_start_time[WICED_TO_WWD_INTERFACE(interface)];

    /* Update DHCP time related variables */
    if ( DHCP_CLIENT_IS_INITIALISED( interface) )
    {
        if ( nx_dhcp_client_update_time_remaining( &DHCP_HANDLE(interface), number_of_ticks_network_was_suspended ) != NX_SUCCESS )
        {
            wiced_assert( "Error updating DHCP client time", 0 != 0 );
            return WICED_ERROR;
        }
    }

    return WICED_SUCCESS;
}

static wiced_result_t wiced_ip_driver_notify( wiced_interface_t interface, wiced_bool_t up )
{
    wiced_result_t result = WICED_SUCCESS;

#ifdef WICED_USE_ETHERNET_INTERFACE
    if ( interface == WICED_ETHERNET_INTERFACE )
    {
        if ( up )
        {
            result = ( platform_ethernet_start( ) == PLATFORM_SUCCESS ) ? WICED_SUCCESS : WICED_ERROR;
        }
        else
        {
            result = ( platform_ethernet_stop( ) == PLATFORM_SUCCESS ) ? WICED_SUCCESS : WICED_ERROR;
        }
    }
#else
    UNUSED_PARAMETER( interface );
    UNUSED_PARAMETER( up );
#endif

    return result;
}
