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

#ifndef  __LIBFLAC_INTERNAL_H__
#define  __LIBFLAC_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "wiced.h"
#include "platform_audio.h"
#include "audio_render.h"
#include "resources.h"

/* FLAC library includes */
#include <FLAC/ordinals.h>
#include <stream_decoder.h>
#include <metadata.h>

#include "flac_test.h"
#include "wiced_flac_interface.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define FLAC_LIB_PRINT(level, arg)   if ((internal != NULL) && (internal->log_level >= level))   WPRINT_LIB_INFO(arg);

/******************************************************
 *                    Constants
 ******************************************************/
#define FLAC_INTERNAL_TAG_VALID                 0x61EDBA16
#define FLAC_INTERNAL_TAG_INVALID               0xDEADBEEF

#define APP_QUEUE_MAX_ENTRIES                   20

#define FLAC_WORKER_THREAD_PRIORITY             (WICED_DEFAULT_WORKER_PRIORITY)
#define FLAC_WORKER_STACK_SIZE                  (8*1024)


#define FLAC_FLAGS_TIMEOUT                      (100)  /* wait for a command before doing anything          */
#define FLAC_QUEUE_PUSH_TIMEOUT                 (100)  /* ms wait for pushing packet to the queue           */
#define FLAC_QUEUE_POP_TIMEOUT                    (1)  /* ms wait for popping packet from the queue         */
#define FLAC_THREAD_SHUTDOWN_WAIT               (100)  /* wait for shutdown done flag */

typedef enum {
    FLAC_EVENT_WORKER_THREAD_SHUTDOWN   = (1 << 0),
    FLAC_EVENT_WORKER_THREAD_DONE       = (1 << 1),

    /* this starts "Decode to the end of the next frame" */
    FLAC_EVENT_WORKER_DECODE_FRAME      = (1 << 2),

} FLAC_EVENTS_T;

#define FLAC_EVENT_WORKER_THREAD_EVENTS  (FLAC_EVENT_WORKER_DECODE_FRAME  | FLAC_EVENT_WORKER_THREAD_SHUTDOWN)

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum {
    FLAC_INPUT_BUFFER_FL = 0,
    FLAC_INPUT_BUFFER_FR,
    FLAC_INPUT_BUFFER_FC,
    FLAC_INPUT_BUFFER_LFE,
    FLAC_INPUT_BUFFER_BL,
    FLAC_INPUT_BUFFER_BR,

    FLAC_INPUT_BUFFER_MAX
} wiced_flac_input_buffer_map;

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/


typedef struct flac_decoder_stream_info_s {
    /* within the entire streaming file
    * currently also used for creating the pts value for the renderer
    */
    uint32_t                    data_offset;
    wiced_bool_t                end_of_input_stream;

    /* we may use one packet for 2 or more "read" callbacks */
    flac_packet_info_t          packet;

    /* from Metadata */
    uint64_t                    total_samples;  /* total sample in stream   */
    uint16_t                    sample_rate;    /* sample rate              */
    uint8_t                     channels;       /* number of channels       */
    uint8_t                     bps;            /* bits per sample          */

} flac_decoder_stream_info_t;


typedef struct flac_internal_s {

    uint32_t                        tag;

    wiced_flac_log_level_t log_level;

    /* app callbacks to get & push audio buffers */
    wiced_flac_audio_buffer_get     buffer_get;   /* get a buffer to fill with LPCM and submit  */
    wiced_flac_audio_buffer_push    buffer_push;  /* push a buffer with LPCM data in it to the audio render */

    wiced_thread_t                  flac_worker_thread;
    wiced_thread_t*                 flac_worker_thread_ptr;
    wiced_event_flags_t             flac_events;

    FLAC__StreamDecoder*            flac_decoder;
    FLAC__StreamMetadata*           flac_metadata;
    wiced_bool_t                    flac_stream_inited;

    wiced_queue_t                   flac_packet_queue;


    /* the stream we are decoding */
    flac_decoder_stream_info_t      stream;
    wiced_flac_source_info_t        source_info;    /* currently playing source information (if known) */
    wiced_bool_t                    user_stop;      /* user requested stop */
    wiced_bool_t                    shut_it_down;   /* internal signal to actually deinit the stream */

        /* debug    */
    uint16_t                        debug_queue_max;        /* max count on the queue   */
    uint16_t                        debug_queue_count;      /* count stuff on the queue */

    /* FLAC Encoder specific structs / data - todo: just for testing the lib link at the moment :) */
    FLAC__StreamEncoder*            flac_encoder;


} flac_internal_t;


/******************************************************
 *               Function Declarations
 ******************************************************/
/******************************************************
 *               Variables Definitions
 ******************************************************/
/******************************************************
 *               Function Definitions
 ******************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* __LIBFLAC_INTERNAL_H__    */
