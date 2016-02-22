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
#include "platform_audio.h"


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
    APOLLO_PLAYER_EVENT_PLAYBACK_STARTED = 0,
    APOLLO_PLAYER_EVENT_PLAYBACK_STOPPED,
    APOLLO_PLAYER_EVENT_SEQ_ERROR
} APOLLO_PLAYER_EVENT_T;

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct apollo_player_s *apollo_player_ref;

typedef int (*apollo_player_event)(apollo_player_ref handle, void* userdata, APOLLO_PLAYER_EVENT_T event, void* arg);

/******************************************************
 *                    Structures
 ******************************************************/

/**
 * Audio format structure passed with APOLLO_PLAYER_EVENT_PLAYBACK_STARTED event.
 */

typedef struct apollo_audio_format_s
{
    uint16_t num_channels;
    uint16_t bits_per_sample;
    uint32_t sample_rate;
} apollo_audio_format_t;

/**
 * Sequence error structure passed with APOLLO_PLAYER_EVENT_SEQ_ERROR event.
 */

typedef struct apollo_seq_err_s
{
    uint16_t last_valid_seq;    /*!< Last valid RTP sequence number received */
    uint16_t cur_seq;           /*!< Current RTP sequence number             */
} apollo_seq_err_t;


/**
 * Stats structure passed with APOLLO_PLAYER_EVENT_PLAYBACK_STOPPED event.
 */

typedef struct
{
    uint32_t rtp_packets_received;      /* Number of RTP packets received               */
    uint32_t rtp_packets_dropped;       /* Number of RTP packets dropped by the network */
    uint64_t total_bytes_received;      /* Total number of RTP bytes received           */
    uint64_t audio_bytes_received;      /* Number of audio bytes received - excludes RTP header bytes and error correction packets */
    uint32_t payload_size;              /* Audio payload size of RTP packets            */
} apollo_player_stats_t;

typedef struct apollo_player_params_s
{

    apollo_player_event     event_cb;       /* Application event callback                               */
    void*                   userdata;       /* Opaque userdata passed back in event callback            */

    wiced_interface_t       interface;      /* Interface to use for RTP socket                          */
    int                     rtp_port;       /* RTP port for socket - 0 to use default port              */
    APOLLO_CHANNEL_MAP_T    channel;        /* Audio channel to process                                 */
    int                     volume;         /* Audio volume (0 - 100)                                   */

    /* Audio render playback parameters */

    platform_audio_device_id_t  device_id;      /* Audio device ID for audio playback                       */
    uint32_t                    buffer_nodes;   /* Number of buffer nodes for audio render to allocate      */
    uint32_t                    buffer_ms;      /* Buffering (pre-roll) time that audio render should use   */
    uint32_t                    threshold_ms;   /* Threshold in ms for adding silence/dropping audio frames */
    int                         clock_enable;   /* 0 = disable (blind push), 1 = enable                     */

    /* Audio PLL tuning control parameters */

    int                         pll_tuning_enable;  /* 0 = disable audio PLL tuning, 1 = enable             */

} apollo_player_params_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/** Initialize the apollo player library.
 *
 * @param[in] params      : Pointer to the configuration parameters.
 *
 * @return Pointer to the apollo player instance or NULL
 */
apollo_player_ref apollo_player_init(apollo_player_params_t* params);

/** Deinitialize the apollo player library.
 *
 * @param[in] apollo_player : Pointer to the apollo player instance.
 *
 * @return    Status of the operation.
 */
wiced_result_t apollo_player_deinit(apollo_player_ref apollo_player);

/** Set the volume for the apollo player library.
 *
 * @param[in] apollo_player : Pointer to the apollo player instance.
 * @param[in] volume        : New volume level (0-100).
 *
 * @return    Status of the operation.
 */
wiced_result_t apollo_player_set_volume(apollo_player_ref apollo_player, int volume);

/** Get the stats for the apollo player session.
 *
 * @param[in] apollo_player : Pointer to the apollo player instance.
 * @param[out] stats        : Pointer to an apollo stats structure.
 *
 * @return    Status of the operation.
 */
wiced_result_t apollo_player_get_stats(apollo_player_ref apollo_player, apollo_player_stats_t* stats);

#ifdef __cplusplus
} /* extern "C" */
#endif
