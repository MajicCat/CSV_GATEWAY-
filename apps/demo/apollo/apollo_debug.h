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

#include "wiced.h"
#include "apollo_log.h"

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
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

wiced_result_t apollo_debug_create_tcp_data_socket(wiced_ip_address_t* ip_addr, int port);
wiced_result_t apollo_debug_close_tcp_data_socket(void);
wiced_result_t apollo_debug_send_tcp_data(uint8_t* data, int datalen);
int apollo_debug_tcp_log_output_handler(APOLLO_LOG_LEVEL_T level, char *logmsg);

#ifdef __cplusplus
} /* extern "C" */
#endif
