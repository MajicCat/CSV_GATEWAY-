/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

/* DTLS client Snippet application
 *
 * This application is used to test DTLS library.
 *
 * Setup or run Scandium (DTLS Server) On your PC :
 *
 * Prerequisites :
 * 1. Java Development Kit (make sure that your Path environment variable points to the JDK and not to a JRE).
 * 2. Eclipse integrated development environment with the EGit and M2Eclipse plugins
 *    (Eclipse Luna for Java Developers is highly recommended, because it contains the listed plugins by default). You can download from the link
 *    http://projects.eclipse.org/releases/luna
 * 3. Scandium source code. Use git to clone to a local directory.
 *    git clone https://github.com/eclipse/californium.scandium.git
 * 4. Now Open Eclipse and Chose File menu > Import... and select Maven > Existing Maven Projects. select the directory where you cloned the scandium
 *    repository and click finish.
 * 5. connect laptop with AP.
 * 6. go to : scandium_examples/src/main/java/ExampleDTLSClient or Server and run.
 *
 *
 * Setup or Run DTLS client on WICED :
 *  Please change DTLS_SERVER_IP below in dtls_client.c [ Give laptop ip address after connecting to AP ]
 *  change in wifi_config.h to connect to same AP where laptop is connected.
 *  Download and run dtls_client.c
 *
 *  Handshake will be finished and Application data will be sent to server.
 *
 *
 * */

#include "wiced_dtls.h"
#include "wiced.h"

#define DTLS_TARGET_PORT   5684
#define SECONDS           (1000)

#define DTLS_SERVER_IP MAKE_IPV4_ADDRESS(192,168,1,3)

wiced_udp_socket_t udp_socket;
wiced_dtls_identity_t dtls_identity;
char Client_Key[ 256 ] = "secretPSK";
uint8_t Client_Identifier[ 256 ] = "Client_identity";

void application_start( void )
{
    wiced_result_t result;
    wiced_packet_t* packet;
    uint8_t* data;
    uint16_t available_space;
    char* rx_data;
    static uint16_t rx_data_length;
    uint16_t available_data_length;
    static wiced_ip_address_t udp_src_ip_addr;
    static uint16_t udp_src_port;
    const wiced_ip_address_t INITIALISER_IPV4_ADDRESS( target_ip_addr, DTLS_SERVER_IP );

    wiced_init( );

    /* Bring up the softAP and network interface */
    wiced_network_up( WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL );

    /* Create UDP socket */
    if ( wiced_udp_create_socket( &udp_socket, DTLS_TARGET_PORT, WICED_STA_INTERFACE ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ( "UDP socket creation failed\n" ) );
        return;
    }

    wiced_dtls_context_t* context = malloc_named("DTLS", sizeof(wiced_dtls_context_t));

    wiced_udp_enable_dtls( &udp_socket, context );

    /* Setup DTLS identity */
    result = wiced_dtls_init_identity( &dtls_identity, Client_Key, Client_Identifier, strlen( (char*) Client_Identifier ) );
    if ( result != WICED_SUCCESS )
    {
        WPRINT_APP_INFO(( "Unable to initialize DTLS identity. Error = [%d]\n", result ));
        return;
    }

    wiced_dtls_init_context( context, &dtls_identity, 0 );

    if ( wiced_udp_start_dtls( &udp_socket, target_ip_addr, WICED_DTLS_AS_CLIENT, DTLS_NO_VERIFICATION ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO(( "DTLS handshake failed\n" ));
        return;
    }

    wiced_packet_create_udp( &udp_socket, 30, &packet, &data, &available_space );

    /* Write the message into tx_data"  */
    sprintf( (char*) data, "%s", "Hello from WICED\n" );

    /* Set the end of the data portion */
    wiced_packet_set_data_end( packet, (uint8_t*) data + strlen( (char*) data ) );

    wiced_udp_send( &udp_socket, &target_ip_addr, DTLS_TARGET_PORT, packet );

    result = wiced_udp_receive( &udp_socket, &packet, 20000 );

    /* Get info about the received UDP packet */
    wiced_udp_packet_get_info( packet, &udp_src_ip_addr, &udp_src_port );

    /* Extract the received data from the UDP packet */
    wiced_packet_get_data( packet, 0, (uint8_t**) &rx_data, &rx_data_length, &available_data_length );

    /* Null terminate the received data, just in case the sender didn't do this */
    rx_data[ rx_data_length ] = '\x0';

    WPRINT_APP_INFO ( ("UDP Rx: \"%s\" from IP %u.%u.%u.%u:%d\n", rx_data,
                    (unsigned char) ( ( GET_IPV4_ADDRESS(udp_src_ip_addr) >> 24 ) & 0xff ),
                    (unsigned char) ( ( GET_IPV4_ADDRESS(udp_src_ip_addr) >> 16 ) & 0xff ),
                    (unsigned char) ( ( GET_IPV4_ADDRESS(udp_src_ip_addr) >> 8 ) & 0xff ),
                    (unsigned char) ( ( GET_IPV4_ADDRESS(udp_src_ip_addr) >> 0 ) & 0xff ),
                    udp_src_port ) );

    wiced_dtls_deinit_context( context );
}
