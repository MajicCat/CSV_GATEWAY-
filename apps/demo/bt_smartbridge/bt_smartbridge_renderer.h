/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */
#pragma once

#include "http_server.h"
#include "wiced_bt_smartbridge.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define WEB_PAGE_TIME_LENGTH                8
#define WEB_PAGE_DATE_LENGTH                10

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef wiced_result_t (*attribute_renderer_t)( wiced_http_response_stream_t* stream, wiced_bt_smartbridge_socket_t* socket );

/******************************************************
 *                    Structures
 ******************************************************/

typedef struct
{
    char*                device_name;
    attribute_renderer_t renderer;
} bt_smartbridge_attribute_renderer_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

/* UUID constants */
extern const wiced_bt_uuid_t uuid_list[];

/******************************************************
 *               Function Declarations
 ******************************************************/

/* Renderers */
extern wiced_result_t wiced_sense_attribute_renderer( wiced_http_response_stream_t* stream, wiced_bt_smartbridge_socket_t* socket );
extern wiced_result_t default_attribute_renderer    ( wiced_http_response_stream_t* stream, wiced_bt_smartbridge_socket_t* socket );


#ifdef __cplusplus
} /* extern "C" */
#endif
