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
 */
#include "bt_smartbridge_renderer.h"
#include "resources.h"

/******************************************************
 *                      Macros
 ******************************************************/

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

/******************************************************
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

/* Display list of attributes to the webpage */
wiced_result_t default_attribute_renderer( wiced_http_response_stream_t* stream, wiced_bt_smartbridge_socket_t* socket )
{
    char                          buffer[256];
    wiced_iso8601_time_t          curr_time;
    wiced_bt_smart_attribute_t    attribute;
    uint16_t                      starting_handle = 0;
    uint32_t                      buffer_length   = 0;

    /* Write device name and address */
    wiced_http_response_stream_write_resource( stream, &resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_page_start );
    wiced_http_response_stream_write( stream, socket->remote_device.name, strlen( socket->remote_device.name ) );
    wiced_http_response_stream_write( stream, " ( ", 3 );

    WPRINT_LIB_INFO( ("[MURAT] Coming here:1\n") );

    memset( buffer, 0, sizeof( buffer ) );
    buffer_length = sprintf( buffer, "%02X:%02X:%02X:%02X:%02X:%02X",
            socket->remote_device.address[5],
            socket->remote_device.address[4],
            socket->remote_device.address[3],
            socket->remote_device.address[2],
            socket->remote_device.address[1],
            socket->remote_device.address[0]);

    wiced_http_response_stream_write( stream, buffer, buffer_length );
    wiced_http_response_stream_write( stream, " )", 2 );
    wiced_http_response_stream_write_resource( stream, &resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_page_start_end );

    /* Write the time */
    wiced_time_get_iso8601_time( &curr_time );
    wiced_http_response_stream_write_resource( stream, &resources_apps_DIR_bt_smartbridge_DIR_data_html_time_start );
    wiced_http_response_stream_write( stream, curr_time.hour, WEB_PAGE_TIME_LENGTH );
    wiced_http_response_stream_write_resource( stream, &resources_apps_DIR_bt_smartbridge_DIR_data_html_time_end );

    /* Write the date */
    wiced_http_response_stream_write_resource( stream, &resources_apps_DIR_bt_smartbridge_DIR_data_html_date_start );
    wiced_http_response_stream_write( stream, curr_time.year, WEB_PAGE_DATE_LENGTH );
    wiced_http_response_stream_write_resource( stream, &resources_apps_DIR_bt_smartbridge_DIR_data_html_date_end );

    /* Look for Primary Service. If found, write the Service UUID and Characteristic Value(s) to the HTTP server */
    while ( wiced_bt_smartbridge_get_attribute_cache_by_uuid( socket, &uuid_list[0], starting_handle, 0xffff, &attribute, sizeof( attribute ) ) == WICED_SUCCESS )
    {
        uint16_t char_start_handle;
        uint16_t char_end_handle;

        /* Update starting handle for the next search */
        starting_handle = attribute.handle + 1;

        /* Copy UUID to buffer */
        memset( buffer, 0, sizeof( buffer ) );

        if ( attribute.value.service.uuid.len == UUID_16BIT )
        {
#ifdef READABLE
            /* GATT Services */
            if ( attribute.value.service.uuid.uu.uuid16 == 0x180F )
            {
                WPRINT_APP_INFO( ("Found Battery Service\r\n") );
                buffer_length = sprintf( buffer, "%04X %s", attribute.value.service.uuid.uu.uuid16, "Battery Level Service" );
            }
            else
            {
                continue;
            }
#else
            buffer_length = sprintf( buffer, "%04X", attribute.value.service.uuid.uu.uuid16 );
#endif
            /* Write Service UUID */
            wiced_http_response_stream_write_resource( stream, &resources_apps_DIR_bt_smartbridge_DIR_data_html_service_name_start );
            wiced_http_response_stream_write( stream, buffer, buffer_length );
            wiced_http_response_stream_write_resource( stream, &resources_apps_DIR_bt_smartbridge_DIR_data_html_service_name_end );
        }
        else
        {
#ifdef READABLE
            if ( (attribute.value.service.uuid.uu.uuid128[1] == 0x0089) &&
                 (attribute.value.service.uuid.uu.uuid128[0] == 0x0085) )
            {
                WPRINT_APP_INFO(("Found WICED Sense Service\r\n"));
                buffer_length = sprintf( buffer, "%04X%04X-%04X-%04X-%04X-%04X%04X%04X %s",
                                         attribute.value.service.uuid.uu.uuid128[7],
                                         attribute.value.service.uuid.uu.uuid128[6],
                                         attribute.value.service.uuid.uu.uuid128[5],
                                         attribute.value.service.uuid.uu.uuid128[4],
                                         attribute.value.service.uuid.uu.uuid128[3],
                                         attribute.value.service.uuid.uu.uuid128[2],
                                         attribute.value.service.uuid.uu.uuid128[1],
                                         attribute.value.service.uuid.uu.uuid128[0],
                                         "WICED Sense Service");
                /* Write Service UUID */
                wiced_http_response_stream_write_resource( stream, &resources_apps_DIR_bt_smartbridge_DIR_data_html_service_name_start );
                wiced_http_response_stream_write( stream, buffer, buffer_length );
                wiced_http_response_stream_write_resource( stream, &resources_apps_DIR_bt_smartbridge_DIR_data_html_service_name_end );
            }
            else
                continue;
#else

            buffer_length = sprintf( buffer, "%04X%04X-%04X-%04X-%04X-%04X%04X%04X",
                    attribute.value.service.uuid.uu.uuid128[7],
                    attribute.value.service.uuid.uu.uuid128[6],
                    attribute.value.service.uuid.uu.uuid128[5],
                    attribute.value.service.uuid.uu.uuid128[4],
                    attribute.value.service.uuid.uu.uuid128[3],
                    attribute.value.service.uuid.uu.uuid128[2],
                    attribute.value.service.uuid.uu.uuid128[1],
                    attribute.value.service.uuid.uu.uuid128[0]);

        /* Write Service UUID */
        wiced_http_response_stream_write_resource( stream, &resources_apps_DIR_bt_smartbridge_DIR_data_html_service_name_start );
        wiced_http_response_stream_write( stream, buffer, buffer_length );
        wiced_http_response_stream_write_resource( stream, &resources_apps_DIR_bt_smartbridge_DIR_data_html_service_name_end );
#endif
        }

        /* Search characteristic and characteristic values. If found, write to HTTP server.
         *          * Set the search start and end handles to the service start and end handles
         *                   */
        char_start_handle = attribute.value.service.start_handle;
        char_end_handle   = attribute.value.service.end_handle;
        while ( wiced_bt_smartbridge_get_attribute_cache_by_uuid( socket, &uuid_list[1], char_start_handle, char_end_handle, &attribute, sizeof( attribute ) ) == WICED_SUCCESS )
        {
            /* Update characteristic start handle */
            char_start_handle = attribute.value.characteristic.value_handle + 1;

            /* Get Characteristic Value using the handle */
            if ( wiced_bt_smartbridge_get_attribute_cache_by_handle( socket, attribute.value.characteristic.value_handle, &attribute, sizeof( attribute ) ) == WICED_SUCCESS )
            {
                uint32_t i = 0;

                /* Copy UUID to buffer */
                memset( buffer, 0, sizeof( buffer ) );

                /* Write Characteristic Value UUID */
                wiced_http_response_stream_write_resource(stream, &resources_apps_DIR_bt_smartbridge_DIR_data_html_char_start );

                if ( attribute.type.len == UUID_16BIT )
                {
                    buffer_length = sprintf( buffer, "%04X", attribute.type.uu.uuid16 );
                }
                else
                {
                    buffer_length = sprintf( buffer, "%04X%04X-%04X-%04X-%04X-%04X%04X%04X",
                            attribute.type.uu.uuid128[7],
                            attribute.type.uu.uuid128[6],
                            attribute.type.uu.uuid128[5],
                            attribute.type.uu.uuid128[4],
                            attribute.type.uu.uuid128[3],
                            attribute.type.uu.uuid128[2],
                            attribute.type.uu.uuid128[1],
                            attribute.type.uu.uuid128[0]);
                }

                wiced_http_response_stream_write( stream, buffer, buffer_length );

                if ( attribute.value_length > 0 )
                {
                    /* Write Characteristic Value */
                    wiced_http_response_stream_write( stream, " [ ", 3 );

                    for ( i = 0; i < attribute.value_length; i++ )
                    {
                        buffer_length = sprintf( buffer, "%02X ", attribute.value.value[i] );
                        wiced_http_response_stream_write( stream, buffer, buffer_length );
                    }

                    wiced_http_response_stream_write( stream, "]", 1 );

#ifdef READABLE
                    /* Human Readable Output */
                    if ( attribute.type.uu.uuid16 == 0x2A19)
                    {
                        /* Battery Level */
                        buffer_length = sprintf(buffer, " Battery: %d ", attribute.value.value[0] );
                        wiced_http_response_stream_write( stream, buffer, buffer_length );

                    }
                    else if ( (attribute.type.uu.uuid128[1] == 0x00A4) &&
                              (attribute.type.uu.uuid128[0] == 0x0059) )
                    {
                        /* WICED Sense */
                        if ( attribute.value.value[ 0 ] == 0x0B )
                        {
                            /* Motion */
                            /* Accelerometer */
                            accel[0] = (attribute.value.value[1] << 8) | attribute.value.value[2];
                            accel[1] = (attribute.value.value[3] << 8) | attribute.value.value[4];
                            accel[2] = (attribute.value.value[5] << 8) | attribute.value.value[6];

                            /* Magnetic/Compass */
                            mag[0] = (attribute.value.value[7] << 8) | attribute.value.value[8];
                            mag[1] = (attribute.value.value[9] << 8) | attribute.value.value[10];
                            mag[2] = (attribute.value.value[11] << 8) | attribute.value.value[12];

                            /* Gyroscope */
                            gyro[0] = (attribute.value.value[13] << 8) | attribute.value.value[14];
                            gyro[1] = (attribute.value.value[15] << 8) | attribute.value.value[16];
                            gyro[2] = (attribute.value.value[17] << 8) | attribute.value.value[18];
                        }
                        else if (attribute.value.value[0] == 0x34)
                        {
                            /* Humidity, Pressure, and Temperature */
                            hpt[0]=(attribute.value.value[1] << 8) | attribute.value.value[2];
                            hpt[1]=(attribute.value.value[3] << 8) | attribute.value.value[4];
                            hpt[2]=(attribute.value.value[5] << 8) | attribute.value.value[6];
                        }

                        /* Accelerometer */
                        buffer_length = sprintf(buffer, "<br>Accelerometer X: %04X Y:%04X Z: %04X", accel[0],accel[1],accel[2]);
                        wiced_http_response_stream_write( stream, buffer, buffer_length );

                        /* Magnetic/Heading */
                        buffer_length = sprintf(buffer, "<br>Magnetic X: %04X Y:%04X Z: %04X", mag[0],mag[1],mag[2]);
                        wiced_http_response_stream_write( stream, buffer, buffer_length );

                        float deg = get_degrees(mag[0],mag[1] );
                        buffer_length = sprintf( buffer, " Degree: %f ", deg );
                        wiced_http_response_stream_write( stream, buffer, buffer_length );

                        /* Gyroscope */
                        buffer_length = sprintf(buffer, "<br>Gyroscope X: %04X Y:%04X Z: %04X", gyro[0],gyro[1],gyro[2]);
                        wiced_http_response_stream_write( stream, buffer, buffer_length );

                        /* Humidity, Pressure, and Temperature */
                        buffer_length = sprintf(buffer, "<br>Humidity    = %d",hpt[0]);
                        wiced_http_response_stream_write( stream, buffer, buffer_length );
                        buffer_length = sprintf(buffer, "<br>Pressure    = %d",hpt[1]);
                        wiced_http_response_stream_write( stream, buffer, buffer_length );
                        buffer_length = sprintf(buffer, "<br>Temperature = %d",hpt[2]);
                        wiced_http_response_stream_write( stream, buffer, buffer_length );
                    }
#endif
                }
                wiced_http_response_stream_write_resource( stream, &resources_apps_DIR_bt_smartbridge_DIR_data_html_char_end );
            }
        }
    }

    /* Send end of html page */
    wiced_http_response_stream_write_resource( stream, &resources_apps_DIR_bt_smartbridge_DIR_data_html_connected_page_end );
    return WICED_SUCCESS;

}
