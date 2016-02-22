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
 * www.httpbin.org Client Application
 *
 * This application snippet demonstrates how to use the WICED HTTP Client API
 * to connect to https://www.httpbin.org
 *
 * Features demonstrated
 *  - Wi-Fi client mode
 *  - DNS lookup
 *  - Secure HTTPS client connection
 *
 * Application Instructions
 * 1. Modify the CLIENT_AP_SSID/CLIENT_AP_PASSPHRASE Wi-Fi credentials
 *    in the wifi_config_dct.h header file to match your Wi-Fi access point
 * 2. Connect a PC terminal to the serial port of the WICED Eval board,
 *    then build and download the application as described in the WICED
 *    Quick Start Guide
 *
 * After the download completes, the application :
 *  - Connects to the Wi-Fi network specified
 *  - Resolves the www.httpbin.org IP address using a DNS lookup
 *  - Sends multiple GET requests to https://www.httpbin.org
 *  - Prints the results to the UART
 *
 */

#include <stdlib.h>
#include "wiced.h"
#include "wiced_tls.h"
#include "http_client.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define SERVER_HOST        "www.httpbin.org"
#define SERVER_PORT        ( 443 )
#define DNS_TIMEOUT_MS     ( 10000 )
#define CONNECT_TIMEOUT_MS ( 3000 )
#define TOTAL_REQUESTS     ( 2 )

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

static void  event_handler( http_client_t* client, http_event_t event, http_response_t* response );
static void  print_data   ( char* data, uint32_t length );
static void  print_content( char* data, uint32_t length );
static void  print_header ( http_header_field_t* header, uint32_t number_of_fields );

/******************************************************
 *               Variable Definitions
 ******************************************************/

static const char httpbin_root_ca_certificate[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIGCDCCA/CgAwIBAgIQKy5u6tl1NmwUim7bo3yMBzANBgkqhkiG9w0BAQwFADCB\n"
    "hTELMAkGA1UEBhMCR0IxGzAZBgNVBAgTEkdyZWF0ZXIgTWFuY2hlc3RlcjEQMA4G\n"
    "A1UEBxMHU2FsZm9yZDEaMBgGA1UEChMRQ09NT0RPIENBIExpbWl0ZWQxKzApBgNV\n"
    "BAMTIkNPTU9ETyBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTQwMjEy\n"
    "MDAwMDAwWhcNMjkwMjExMjM1OTU5WjCBkDELMAkGA1UEBhMCR0IxGzAZBgNVBAgT\n"
    "EkdyZWF0ZXIgTWFuY2hlc3RlcjEQMA4GA1UEBxMHU2FsZm9yZDEaMBgGA1UEChMR\n"
    "Q09NT0RPIENBIExpbWl0ZWQxNjA0BgNVBAMTLUNPTU9ETyBSU0EgRG9tYWluIFZh\n"
    "bGlkYXRpb24gU2VjdXJlIFNlcnZlciBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEP\n"
    "ADCCAQoCggEBAI7CAhnhoFmk6zg1jSz9AdDTScBkxwtiBUUWOqigwAwCfx3M28Sh\n"
    "bXcDow+G+eMGnD4LgYqbSRutA776S9uMIO3Vzl5ljj4Nr0zCsLdFXlIvNN5IJGS0\n"
    "Qa4Al/e+Z96e0HqnU4A7fK31llVvl0cKfIWLIpeNs4TgllfQcBhglo/uLQeTnaG6\n"
    "ytHNe+nEKpooIZFNb5JPJaXyejXdJtxGpdCsWTWM/06RQ1A/WZMebFEh7lgUq/51\n"
    "UHg+TLAchhP6a5i84DuUHoVS3AOTJBhuyydRReZw3iVDpA3hSqXttn7IzW3uLh0n\n"
    "c13cRTCAquOyQQuvvUSH2rnlG51/ruWFgqUCAwEAAaOCAWUwggFhMB8GA1UdIwQY\n"
    "MBaAFLuvfgI9+qbxPISOre44mOzZMjLUMB0GA1UdDgQWBBSQr2o6lFoL2JDqElZz\n"
    "30O0Oija5zAOBgNVHQ8BAf8EBAMCAYYwEgYDVR0TAQH/BAgwBgEB/wIBADAdBgNV\n"
    "HSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwGwYDVR0gBBQwEjAGBgRVHSAAMAgG\n"
    "BmeBDAECATBMBgNVHR8ERTBDMEGgP6A9hjtodHRwOi8vY3JsLmNvbW9kb2NhLmNv\n"
    "bS9DT01PRE9SU0FDZXJ0aWZpY2F0aW9uQXV0aG9yaXR5LmNybDBxBggrBgEFBQcB\n"
    "AQRlMGMwOwYIKwYBBQUHMAKGL2h0dHA6Ly9jcnQuY29tb2RvY2EuY29tL0NPTU9E\n"
    "T1JTQUFkZFRydXN0Q0EuY3J0MCQGCCsGAQUFBzABhhhodHRwOi8vb2NzcC5jb21v\n"
    "ZG9jYS5jb20wDQYJKoZIhvcNAQEMBQADggIBAE4rdk+SHGI2ibp3wScF9BzWRJ2p\n"
    "mj6q1WZmAT7qSeaiNbz69t2Vjpk1mA42GHWx3d1Qcnyu3HeIzg/3kCDKo2cuH1Z/\n"
    "e+FE6kKVxF0NAVBGFfKBiVlsit2M8RKhjTpCipj4SzR7JzsItG8kO3KdY3RYPBps\n"
    "P0/HEZrIqPW1N+8QRcZs2eBelSaz662jue5/DJpmNXMyYE7l3YphLG5SEXdoltMY\n"
    "dVEVABt0iN3hxzgEQyjpFv3ZBdRdRydg1vs4O2xyopT4Qhrf7W8GjEXCBgCq5Ojc\n"
    "2bXhc3js9iPc0d1sjhqPpepUfJa3w/5Vjo1JXvxku88+vZbrac2/4EjxYoIQ5QxG\n"
    "V/Iz2tDIY+3GH5QFlkoakdH368+PUq4NCNk+qKBR6cGHdNXJ93SrLlP7u3r7l+L4\n"
    "HyaPs9Kg4DdbKDsx5Q5XLVq4rXmsXiBmGqW5prU5wfWYQ//u+aen/e7KJD2AFsQX\n"
    "j4rBYKEMrltDR5FL1ZoXX/nUh8HCjLfn4g8wGTeGrODcQgPmlKidrv0PJFGUzpII\n"
    "0fxQ8ANAe4hZ7Q7drNJ3gjTcBpUC2JD5Leo31Rpg0Gcg19hCC0Wvgmje3WYkN5Ap\n"
    "lBlGGSW4gNfL1IYoakRwJiNiqZ+Gb7+6kHDSVneFeO/qJakXzlByjAA6quPbYzSf\n"
    "+AZxAeKCINT+b72x\n"
    "-----END CERTIFICATE-----\n";


/******************************************************
 *               Function Definitions
 ******************************************************/

static http_client_t  client;
static http_request_t requests[TOTAL_REQUESTS];

static const char* request_uris[] =
{
    [0] = "/get",
    [1] = "/html",
};

void application_start( void )
{
    wiced_ip_address_t  ip_address;
    wiced_result_t      result;
    http_header_field_t header;

    wiced_init( );
    wiced_network_up(WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL);

    WPRINT_APP_INFO( ( "Resolving IP address of %s\n", SERVER_HOST ) );
    wiced_hostname_lookup( SERVER_HOST, &ip_address, DNS_TIMEOUT_MS );
    WPRINT_APP_INFO( ( "%s is at %u.%u.%u.%u\n", SERVER_HOST,
                                                 (uint8_t)(GET_IPV4_ADDRESS(ip_address) >> 24),
                                                 (uint8_t)(GET_IPV4_ADDRESS(ip_address) >> 16),
                                                 (uint8_t)(GET_IPV4_ADDRESS(ip_address) >> 8),
                                                 (uint8_t)(GET_IPV4_ADDRESS(ip_address) >> 0) ) );

    /* Initialize the root CA certificate */
    result = wiced_tls_init_root_ca_certificates( httpbin_root_ca_certificate );
    if ( result != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ( "Error: Root CA certificate failed to initialize: %u\n", result) );
        return;
    }

    http_client_init( &client, WICED_STA_INTERFACE, event_handler, NULL );
    WPRINT_APP_INFO( ( "Connecting to %s\n", SERVER_HOST ) );
    http_client_connect( &client, (const wiced_ip_address_t*)&ip_address, SERVER_PORT, HTTP_USE_TLS, CONNECT_TIMEOUT_MS );
    WPRINT_APP_INFO( ( "Connected\n" ) );

    header.field        = HTTP_HEADER_HOST;
    header.field_length = sizeof( HTTP_HEADER_HOST ) - 1;
    header.value        = SERVER_HOST;
    header.value_length = sizeof( SERVER_HOST ) - 1;

    http_request_init( &requests[0], &client, HTTP_GET, request_uris[0], HTTP_1_1 );
    http_request_write_header( &requests[0], &header, 1 );
    http_request_write_end_header( &requests[0] );
    http_request_init( &requests[1], &client, HTTP_GET, request_uris[1], HTTP_1_1 );
    http_request_write_header( &requests[1], &header, 1 );
    http_request_write_end_header( &requests[1] );

    WPRINT_APP_INFO( ( "Sending back-to-back requests\n" ) );
    http_request_flush( &requests[0] );
    http_request_flush( &requests[1] );
}

static void event_handler( http_client_t* client, http_event_t event, http_response_t* response )
{
    switch( event )
    {
        case HTTP_CONNECTED:
            WPRINT_APP_INFO(( "Connected to %s\n", SERVER_HOST ));
            break;

        case HTTP_DISCONNECTED:
        {
            WPRINT_APP_INFO(( "Disconnected from %s\n", SERVER_HOST ));
            http_request_deinit( &requests[0] );
            http_request_deinit( &requests[1] );
            break;
        }

        case HTTP_DATA_RECEIVED:
        {
            if ( response->request == &requests[0] )
            {
                /* Response to first request. Simply print the result */
                WPRINT_APP_INFO( ( "\nRecieved response for request #1. Content received:\n" ) );
                print_content( (char*)response->data, response->length );
            }
            else if ( response->request == &requests[1] )
            {
                /* Response to second request. Parse header for "Date" and "Content-Length" */
                http_header_field_t header_fields[2];
                uint32_t            size = sizeof( header_fields ) / sizeof( http_header_field_t );

                header_fields[0].field        = HTTP_HEADER_DATE;
                header_fields[0].field_length = sizeof( HTTP_HEADER_DATE ) - 1;
                header_fields[0].value        = NULL;
                header_fields[0].value_length = 0;
                header_fields[1].field        = HTTP_HEADER_CONTENT_LENGTH;
                header_fields[1].field_length = sizeof( HTTP_HEADER_CONTENT_LENGTH ) - 1;
                header_fields[1].value        = NULL;
                header_fields[1].value_length = 0;

                if ( http_parse_header( response->data, response->length, header_fields, size ) == WICED_SUCCESS )
                {
                    WPRINT_APP_INFO( ( "\nParsing response of request #2 for \"Date\" and \"Content-Length\". Fields found:\n" ) );
                    print_header( header_fields, size );
                }
            }
            break;
        }
        default:
            break;
    }
}

static void print_data( char* data, uint32_t length )
{
    uint32_t a;

    for ( a = 0; a < length; a++ )
    {
        WPRINT_APP_INFO( ( "%c", data[a] ) );
    }
}

static void print_content( char* data, uint32_t length )
{
    WPRINT_APP_INFO(( "==============================================\n" ));
    print_data( (char*)data, length );
    WPRINT_APP_INFO(( "\n==============================================\n" ));
}

static void print_header( http_header_field_t* header_fields, uint32_t number_of_fields )
{
    uint32_t a;

    WPRINT_APP_INFO(( "==============================================\n" ));
    for ( a = 0; a < 2; a++ )
    {
        print_data( header_fields[a].field, header_fields[a].field_length );
        WPRINT_APP_INFO(( " : " ));
        print_data( header_fields[a].value, header_fields[a].value_length );
        WPRINT_APP_INFO(( "\n" ));
    }
    WPRINT_APP_INFO(( "==============================================\n" ));
}
