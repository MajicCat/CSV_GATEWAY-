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

#include "apollocore.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
    APOLLO_CMD_EVENT_QUERY_SPEAKER,
    APOLLO_CMD_EVENT_SET_SPEAKER,
    APOLLO_CMD_EVENT_SET_VOLUME
} APOLLO_CMD_EVENT_T;

/******************************************************
 *                    Structures
 ******************************************************/

/**
 * Speaker structure used with QUERY_SPEAKER and GET_SPEAKER events.
 * For QUERY_SPEAKER events, the callback routine is
 * expected to fill in the structure elements.
 * For SET_SPEAKER events, the apollo command component
 * is passing the received information to the callback.
 */

typedef struct apollo_cmd_speaker_s
{
    char *speaker_name;                     /* Speaker name string (not nul terminated) */
    int speaker_name_len;                   /* Length of speaker name string            */
    APOLLO_CHANNEL_MAP_T speaker_channel;   /* Speaker channel mask                     */
} apollo_cmd_speaker_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef wiced_result_t (*apollo_cmd_callback_t)(void* handle, void* userdata, APOLLO_CMD_EVENT_T event, void* arg);

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/** Initialize the Apollo command library.
 *
 * @param[in] interface   : Interface for the command listener socket.
 * @param[in] mac         : Client MAC address to use for processing commands.
 * @param[in] userdata    : Userdata pointer passed back in event callback.
 * @param[in] callback    : Callback handler for command events.
 *
 * @return Pointer to the command instance or NULL
 */
void* apollo_cmd_init(wiced_interface_t interface, wiced_mac_t* mac, void* userdata, apollo_cmd_callback_t callback);


/** Deinitialize the Apollo command library.
 *
 * @param[in] handle  : handle to the command instance.
 *
 * @return    Status of the operation.
 */
wiced_result_t apollo_cmd_deinit(void *handle);

#ifdef __cplusplus
} /* extern "C" */
#endif
