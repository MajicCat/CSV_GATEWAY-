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

#ifdef __cplusplus
extern "C" {
#endif

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

/*  For lengths of these fields, see wiced_ssdp.h */
typedef struct application_dct_s {
        uint16_t    server_port;                /* port for server (not multicast port)     */
        uint32_t    notify_time;                /* mcast_notify_time                        */
        uint8_t     serve_path_page[128 + 1];   /* server path used in M-SEARCH response    */

} application_dct_t;


#ifdef __cplusplus
} /* extern "C" */
#endif
