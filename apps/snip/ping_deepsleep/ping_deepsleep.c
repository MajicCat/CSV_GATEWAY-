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
 * Ping DeepSleep Application
 *
 */

#include "wiced.h"
#include "wiced_deep_sleep.h"
#include "wwd_buffer_interface.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define WIFI_SLEEP_TIME                      (10000 * MILLISECONDS)
#define POWERSAVE_RETURN_TO_SLEEP_DELAY      10
#define USE_POWERSAVE_POLL
#define NETWORK_INTERFACE                    WICED_STA_INTERFACE
#define PING_NUMBER                          3
#define PING_CONDITION                       ( ( wakeup_count % 3 ) == 0 )
#define DEADLINE_NETWORKING_TO_COMPLETE      1000

#ifdef USE_STATIC_IP_SETTINGS
static const wiced_ip_setting_t device_static_ip_settings =
{
    INITIALISER_IPV4_ADDRESS( .ip_address,   MAKE_IPV4_ADDRESS(192, 168,   1,  2) ),
    INITIALISER_IPV4_ADDRESS( .gateway,      MAKE_IPV4_ADDRESS(192, 168,   1,  1) ),
    INITIALISER_IPV4_ADDRESS( .netmask,      MAKE_IPV4_ADDRESS(255, 255, 255,  0) ),
};
#define NETWORK_INTERFACE_ADDRESS_ASSIGNING  WICED_USE_STATIC_IP
#define NETWORK_INTERFACE_ADDRESS            &device_static_ip_settings
#else
#define NETWORK_INTERFACE_ADDRESS_ASSIGNING  WICED_USE_EXTERNAL_DHCP_SERVER
#define NETWORK_INTERFACE_ADDRESS            NULL
#endif

#ifdef USE_POWERSAVE_POLL
#define NETWORK_INTERFACE_ENABLE_POWERSAVE() wiced_wifi_enable_powersave()
#else
#define NETWORK_INTERFACE_ENABLE_POWERSAVE() wiced_wifi_enable_powersave_with_throughput( POWERSAVE_RETURN_TO_SLEEP_DELAY );
#endif

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

static wiced_result_t send_ping ( void );
static void           deep_sleep( void );
static wiced_bool_t   get_ping_destination( wiced_ip_address_t* ip_address );
static void           print_wlan_log( void );

/******************************************************
 *               Variables Definitions
 ******************************************************/

static wiced_bool_t network_suspended = WICED_FALSE;

static uint32_t     WICED_DEEP_SLEEP_SAVED_VAR( wakeup_count )        = 0;
static uint32_t     WICED_DEEP_SLEEP_SAVED_VAR( cold_boot_fast_time ) = 0;
static uint32_t     WICED_DEEP_SLEEP_SAVED_VAR( cold_boot_slow_time ) = 0;
static uint32_t     WICED_DEEP_SLEEP_SAVED_VAR( max_up_time )         = 0;

/******************************************************
 *               Function Definitions
 ******************************************************/

void application_start(void)
{
    wiced_result_t result;
    wiced_time_t   cur_rtos_time;
    uint32_t       cur_slow_time;
    uint32_t       cur_fast_time;
    uint32_t       warm_boot_fast_time;
    uint32_t       warm_boot_slow_time;
    uint32_t       rtos_time_since_deep_sleep_enter;
    wiced_bool_t   network_inited;

    wakeup_count++;

    wiced_time_get_time( &cur_rtos_time );
    rtos_time_since_deep_sleep_enter = wiced_deep_sleep_ticks_since_enter( );
    cur_slow_time                    = platform_tick_get_time( PLATFORM_TICK_GET_SLOW_TIME_STAMP );
    cur_fast_time                    = platform_tick_get_time( PLATFORM_TICK_GET_FAST_TIME_STAMP );

    WPRINT_APP_INFO(( "Application started: cur_rtos_time=%u cur_slow_time=%lu rtos_time_since_deep_sleep_enter=%lu wakeup_count=%lu\n",
        (unsigned)cur_rtos_time, cur_slow_time, rtos_time_since_deep_sleep_enter, wakeup_count ));

    if ( WICED_DEEP_SLEEP_IS_WARMBOOT_HANDLE( ) )
    {
        WPRINT_APP_INFO(( "Warm boot.\n" ));

        warm_boot_fast_time = cur_fast_time;
        warm_boot_slow_time = cur_slow_time;

        result = wiced_resume_after_deep_sleep( );
    }
    else
    {
        WPRINT_APP_INFO(( "Cold boot.\n" ));

        warm_boot_fast_time = 0;
        warm_boot_slow_time = 0;
        cold_boot_fast_time = cur_fast_time;
        cold_boot_slow_time = cur_slow_time;

        result = wiced_init( );

        if ( result == WICED_SUCCESS )
        {
            result = NETWORK_INTERFACE_ENABLE_POWERSAVE( );
        }
    }
    if ( result != WICED_SUCCESS )
    {
        while ( 1 )
        {
            WPRINT_APP_INFO(( "*** Init failed %d, just spinning here ***\n", (int)result ));

            print_wlan_log( );

            wiced_rtos_delay_milliseconds( WIFI_SLEEP_TIME );
        }
    }

    WPRINT_APP_INFO(( "WICED inited: rtos_time_since_deep_sleep_enter=%lu\n", wiced_deep_sleep_ticks_since_enter( ) ));

    if ( !WICED_DEEP_SLEEP_IS_WARMBOOT_HANDLE( ) )
    {
        /* Bring up the network interface and connect to the Wi-Fi network */
        WPRINT_APP_INFO(( "Networking is about to bring up\n" ));
        result = wiced_network_up( NETWORK_INTERFACE, NETWORK_INTERFACE_ADDRESS_ASSIGNING, NETWORK_INTERFACE_ADDRESS );
    }
    else
    {
        /* Resume network interface */
        WPRINT_APP_INFO(( "Networking is about to resume\n" ));
        result = wiced_network_resume_after_deep_sleep( NETWORK_INTERFACE, NETWORK_INTERFACE_ADDRESS_ASSIGNING, NETWORK_INTERFACE_ADDRESS );
    }
    if ( result == WICED_SUCCESS )
    {
        WPRINT_APP_INFO(( "Networking inited\n" ));
        network_inited = WICED_TRUE;
    }
    else
    {
        WPRINT_APP_INFO(( "Networking NOT inited: result %d\n", result ));
        network_inited = WICED_FALSE;
    }

    while (1)
    {
        /* Send an ICMP ping to the gateway */
        if ( PING_CONDITION && network_inited )
        {
            int i;

            for ( i = 0; i < PING_NUMBER; ++i )
            {
                print_wlan_log( );
                send_ping( );
            }
        }
        print_wlan_log( );

        /* Print statistic */
        WPRINT_APP_INFO(( "stats: up_num=%lu up_time=%lu max_up_time=%lu wait_up_time=%lu cpu_timer(cold=%lu warm=%lu) pmu_timer(cold=%lu warm=%lu)\n",
            PLATFORM_WLAN_POWERSAVE_GET_STATS( PLATFORM_WLAN_POWERSAVE_STATS_CALL_NUM ),
            PLATFORM_WLAN_POWERSAVE_GET_STATS( PLATFORM_WLAN_POWERSAVE_STATS_UP_TIME ),
            max_up_time,
            PLATFORM_WLAN_POWERSAVE_GET_STATS( PLATFORM_WLAN_POWERSAVE_STATS_WAIT_UP_TIME ),
            cold_boot_fast_time,
            warm_boot_fast_time,
            cold_boot_slow_time,
            warm_boot_slow_time ));

        if ( WICED_DEEP_SLEEP_IS_WARMBOOT_HANDLE( ) )
        {
            uint32_t up_time = PLATFORM_WLAN_POWERSAVE_GET_STATS( PLATFORM_WLAN_POWERSAVE_STATS_UP_TIME );
            max_up_time = MAX( max_up_time, up_time );
        }

        /* Delay to make sure UART messages are went out */
        wiced_rtos_delay_milliseconds( 100 );

        /* Enter deep-sleep */
        deep_sleep( );
    }
}

static void deep_sleep( void )
{
    int i;

    /* Wait till packets be sent */
    for ( i = 0; i < DEADLINE_NETWORKING_TO_COMPLETE; ++i )
    {
        if ( wiced_deep_sleep_is_networking_idle( NETWORK_INTERFACE ) )
        {
            break;
        }

        wiced_rtos_delay_milliseconds( 1 );
    }

    /* Suspend network timers */
    if ( !network_suspended )
    {
        if ( wiced_network_suspend( ) == WICED_SUCCESS )
        {
            network_suspended = WICED_TRUE;
        }
    }

    /*
     * Wakeup system monitor thread so it can:
     *     - kick watchdog
     *     - go to sleep for maximum period so deep-sleep be longer
     */
    wiced_wakeup_system_monitor_thread( );

    /* Enable power save */
    wiced_platform_mcu_enable_powersave( );

    /* Deep-sleep for a while */
    wiced_rtos_delay_milliseconds( WIFI_SLEEP_TIME );

    /* Disable power save */
    wiced_platform_mcu_disable_powersave( );

    /* Resume network timers */
    if ( network_suspended )
    {
        if ( wiced_network_resume( ) == WICED_SUCCESS )
        {
            network_suspended = WICED_FALSE;
        }
    }
}

static wiced_result_t send_ping( void )
{
    const uint32_t     ping_timeout = 1000;
    uint32_t           elapsed_ms;
    wiced_result_t     status;
    wiced_ip_address_t ping_target_ip;

    if ( !get_ping_destination( &ping_target_ip ) )
    {
        WPRINT_APP_INFO(("Unknown ping destination.\n"));
        status = WICED_ERROR;
    }
    else
    {
        WPRINT_APP_INFO(("Pinging %u.%u.%u.%u.\n",
            (unsigned int)((GET_IPV4_ADDRESS(ping_target_ip) >> 24) & 0xFF),
            (unsigned int)((GET_IPV4_ADDRESS(ping_target_ip) >> 16) & 0xFF),
            (unsigned int)((GET_IPV4_ADDRESS(ping_target_ip) >>  8) & 0xFF),
            (unsigned int)((GET_IPV4_ADDRESS(ping_target_ip) >>  0) & 0xFF) ));

        status = wiced_ping( NETWORK_INTERFACE, &ping_target_ip, ping_timeout, &elapsed_ms );
    }

    if ( status == WICED_SUCCESS )
    {
        WPRINT_APP_INFO(( "Ping Reply %lums since deep sleep enter %lums\n", elapsed_ms, wiced_deep_sleep_ticks_since_enter( ) ));
    }
    else if ( status == WICED_TIMEOUT )
    {
        WPRINT_APP_INFO(( "Ping timeout\n" ));
    }
    else
    {
        WPRINT_APP_INFO(( "Ping error: %d\n", (int)status ));
    }

    return WICED_SUCCESS;
}

static wiced_bool_t get_ping_destination( wiced_ip_address_t* ip_address )
{
    return ( wiced_ip_get_gateway_address( NETWORK_INTERFACE, ip_address ) == WICED_SUCCESS ) ? WICED_TRUE : WICED_FALSE;
}

static void print_wlan_log( void )
{
    static char buffer[200];
    (void)wwd_wifi_read_wlan_log( buffer, sizeof( buffer ) );
}

void wiced_deep_sleep_application_init_on_networking_ready_handler( void )
{
    /*
     * If application expects some packets sent to it while it was in deep-sleep,
     * application should create socket(s) here.
     * Network stack is trying to preserve packets during recovering from deep-sleep,
     * and after this point these preserved packets be pushed up to the stack.
     * If no sockets ready to receive packets, they would be lost.
     */
}
