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

#include "flac_app_dct.h"

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

DEFINE_APP_DCT(flac_app_dct_t)
{
    .channel              = CHANNEL_MAP_FL | CHANNEL_MAP_FR,
    .buffering_ms         = FLAC_BUFFERING_MS_DEFAULT,
    .threshold_ms         = FLAC_THRESHOLD_MS_DEFAULT,
    .clock_enable         = 0,  /* 0 = blind push, 1 = use AS clock */
    .volume               = FLAC_VOLUME_DEFAULT,
    .audio_device_rx      = PLATFORM_DEFAULT_AUDIO_INPUT,
    .audio_device_tx      = PLATFORM_DEFAULT_AUDIO_OUTPUT,
};

/******************************************************
 *               Function Definitions
 ******************************************************/
