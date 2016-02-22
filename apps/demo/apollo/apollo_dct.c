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

#include "wiced_framework.h"
#include "platform_audio.h"
#include "apollo_dct.h"
#include "apollo_rtp_params.h"
#include "apollo_streamer.h"

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

DEFINE_APP_DCT(app_dct_t)
{
    .apollo_dct =
    {
        .is_configured       = 1,
        .apollo_role         = APOLLO_ROLE_SINK,
        .speaker_name        = "Apollo",
        .speaker_channel     = CHANNEL_MAP_FR,
        .log_level           = APOLLO_LOG_ERR,
        .buffering_ms        = APOLLO_BUFFERING_MS_DEFAULT,
        .threshold_ms        = APOLLO_THRESHOLD_MS_DEFAULT,
        .auto_start          = 1,
        .clock_enable        = 1,  /* 0 = blind push, 1 = use AS clock */
        .pll_tuning_enable   = 1,
        .volume              = APOLLO_VOLUME_DEFAULT,
        .payload_size        = RTP_PACKET_MAX_DATA,
        .fec_order           = RTP_AUDIO_FEC_PRIOR,
        .fec_length          = APOLLO_FEC_LENGTH_DEFAULT,
        .source_type         = APOLLO_AUDIO_SOURCE_BT,
        .clientaddr          = { WICED_IPV4, { .v4 = (APOLLO_MULTICAST_IPV4_ADDRESS_DEFAULT) } },
        .rtp_port            = RTP_DEFAULT_PORT,
        .audio_device_rx     = PLATFORM_DEFAULT_AUDIO_INPUT,
        .audio_device_tx     = PLATFORM_DEFAULT_AUDIO_OUTPUT,
        .input_sample_rate   = APOLLO_INPUT_SAMPLE_RATE_DEFAULT,
        .input_sample_size   = APOLLO_INPUT_SAMPLE_SIZE_DEFAULT,
        .input_channel_count = APOLLO_INPUT_CHANNEL_COUNT,
    },
    .apollo_bt_dct =
    {
        .bt_hash_table              = {0},
        .bt_paired_device_info[0]   = {0},
        .bt_local_id_keys           = {{0}}
    }
};

/******************************************************
 *               Function Definitions
 ******************************************************/
