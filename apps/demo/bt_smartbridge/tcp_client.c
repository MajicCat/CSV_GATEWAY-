/*
 * tcp_client.c
 *
 *  Created on: Feb 18, 2016
 *      Author: msenel
 */
#include "wiced.h"


/******************************************************
 *                      Macros
 ******************************************************/

#define TCP_PACKET_MAX_DATA_LENGTH        30
#define TCP_CLIENT_INTERVAL               2
//#define TCP_CLIENT_INTERVAL               10
#define TCP_SERVER_PORT                   50007
#define TCP_CLIENT_CONNECT_TIMEOUT        500
#define TCP_CLIENT_RECEIVE_TIMEOUT        300
#define TCP_CONNECTION_NUMBER_OF_RETRIES  3

/* Change the server IP address to match the TCP echo server address */
//#define TCP_SERVER_IP_ADDRESS MAKE_IPV4_ADDRESS(192,168,1,1)
#define TCP_SERVER_IP_ADDRESS MAKE_IPV4_ADDRESS(192,168,1,2)

/******************************************************
 *               Static Function Declarations
 ******************************************************/

static wiced_result_t tcp_client();
/******************************************************
 *               Variable Definitions
 ******************************************************/

static wiced_tcp_socket_t  tcp_client_socket;
static wiced_timed_event_t tcp_client_event;

wiced_result_t tcp_client_init(wiced_interface_t interface)
{
	/* Create a TCP socket */
	if ( wiced_tcp_create_socket( &tcp_client_socket, interface ) != WICED_SUCCESS ) {
		WPRINT_APP_ERROR( ("TCP socket creation failed\n") );
		return WICED_ERROR;
	}

	/* Bind to the socket */
	if (wiced_tcp_bind( &tcp_client_socket, TCP_SERVER_PORT ) != WICED_SUCCESS ) {
		WPRINT_APP_ERROR( ("TCP bind failed\n") );
		return WICED_ERROR;
	}

	/* Register a function to send TCP packets */
	if (wiced_rtos_register_timed_event( &tcp_client_event,
			WICED_NETWORKING_WORKER_THREAD, &tcp_client, TCP_CLIENT_INTERVAL * SECONDS, 0 ) != WICED_SUCCESS ) {
		WPRINT_APP_ERROR( ("TCP Client Event Register faiuled\n") );
		return WICED_ERROR;
	}

	WPRINT_APP_INFO(("Connecting to the remote TCP server every %d seconds ...\n", TCP_CLIENT_INTERVAL));
	return WICED_SUCCESS;
}

wiced_result_t tcp_client( void* arg )
{
    wiced_result_t           result;
    wiced_packet_t*          packet;
    wiced_packet_t*          rx_packet;
    char*                    tx_data;
    char*                    rx_data;
    uint16_t                 rx_data_length;
    uint16_t                 available_data_length;
    const wiced_ip_address_t INITIALISER_IPV4_ADDRESS( server_ip_address, TCP_SERVER_IP_ADDRESS );
    int                      connection_retries;
    UNUSED_PARAMETER( arg );

    /* Connect to the remote TCP server, try several times */
    connection_retries = 0;
    do
    {
        result = wiced_tcp_connect( &tcp_client_socket, &server_ip_address, TCP_SERVER_PORT, TCP_CLIENT_CONNECT_TIMEOUT );
        connection_retries++;
    }
    while( ( result != WICED_SUCCESS ) && ( connection_retries < TCP_CONNECTION_NUMBER_OF_RETRIES ) );
    if( result != WICED_SUCCESS)
    {
        WPRINT_APP_INFO(("Unable to connect to the server! Halt.\n"));
    }

    /* Create the TCP packet. Memory for the tx_data is automatically allocated */
    if (wiced_packet_create_tcp(&tcp_client_socket, TCP_PACKET_MAX_DATA_LENGTH, &packet, (uint8_t**)&tx_data, &available_data_length) != WICED_SUCCESS)
    {
        WPRINT_APP_INFO(("TCP packet creation failed\n"));
        return WICED_ERROR;
    }

    /* Write the message into tx_data"  */
    sprintf(tx_data, "%s", "Hello from WICED\n");

    /* Set the end of the data portion */
    wiced_packet_set_data_end(packet, (uint8_t*)tx_data + strlen(tx_data));

    /* Send the TCP packet */
    if (wiced_tcp_send_packet(&tcp_client_socket, packet) != WICED_SUCCESS)
    {
        WPRINT_APP_INFO(("TCP packet send failed\n"));

        /* Delete packet, since the send failed */
        wiced_packet_delete(packet);

        /* Close the connection */
        wiced_tcp_disconnect(&tcp_client_socket);
        return WICED_ERROR;
    }

    /* Receive a response from the server and print it out to the serial console */
    result = wiced_tcp_receive(&tcp_client_socket, &rx_packet, TCP_CLIENT_RECEIVE_TIMEOUT);
    if( result != WICED_SUCCESS )
    {
        WPRINT_APP_INFO(("TCP packet reception failed\n"));

        /* Delete packet, since the receive failed */
        wiced_packet_delete(rx_packet);

        /* Close the connection */
        wiced_tcp_disconnect(&tcp_client_socket);
        return WICED_ERROR;
    }

    /* Get the contents of the received packet */
    wiced_packet_get_data(rx_packet, 0, (uint8_t**)&rx_data, &rx_data_length, &available_data_length);

    /* Null terminate the received string */
    rx_data[rx_data_length] = '\x0';
    WPRINT_APP_INFO(("%s", rx_data));

    /* Delete the packet and terminate the connection */
    wiced_packet_delete(rx_packet);
    wiced_tcp_disconnect(&tcp_client_socket);

    return WICED_SUCCESS;

}
