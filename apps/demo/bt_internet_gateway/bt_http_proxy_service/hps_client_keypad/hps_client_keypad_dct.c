/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

#include "wiced_framework.h"
#include "hps_client_keypad_dct.h"


/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/* Put your ThingSpeak channel and key here */
#define THINGSPEAK_KEY     ""   /* 16 characters */
#define THINGSPEAK_CHANNEL ""   /* 5 characters*/

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
 *               Variable Definitions
 ******************************************************/

DEFINE_APP_DCT(user_dct_data_t)
{
    .thingspeak_channel = THINGSPEAK_CHANNEL,
    .thingspeak_key     = THINGSPEAK_KEY,
};

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/
