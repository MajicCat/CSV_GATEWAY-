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

#include "wiced_flac_interface.h"
#include "platform_audio.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define FLAC_BUFFERING_MS_MIN        0
#define FLAC_BUFFERING_MS_DEFAULT   50
#define FLAC_BUFFERING_MS_MAX     1000

#define FLAC_THRESHOLD_MS_MIN        0
#define FLAC_THRESHOLD_MS_DEFAULT   40
#define FLAC_THRESHOLD_MS_MAX     1000

#define FLAC_VOLUME_MIN              0
#define FLAC_VOLUME_DEFAULT         50
#define FLAC_VOLUME_MAX            100

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

typedef struct flac_app_dct_s {
    AUDIO_CHANNEL_MAP_T channel;
    int buffering_ms;
    int threshold_ms;
    int clock_enable;   /* enable AS clock */
    int volume;
    platform_audio_device_id_t audio_device_rx;
    platform_audio_device_id_t audio_device_tx;
} flac_app_dct_t;


#ifdef __cplusplus
} /* extern "C" */
#endif
