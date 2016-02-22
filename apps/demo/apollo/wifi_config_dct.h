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

/*
 * AP settings in this file are stored in the DCT. These
 * settings may be overwritten at manufacture when the
 * DCT is written with the final production configuration
 */

/* This is the soft AP used for device configuration */
#define CONFIG_AP_SSID       "NOT USED FOR THIS APP"
#define CONFIG_AP_PASSPHRASE "NOT USED FOR THIS APP"
#define CONFIG_AP_SECURITY   WICED_SECURITY_OPEN
#define CONFIG_AP_CHANNEL    1
#define CONFIG_AP_VALID      WICED_FALSE

/* This is the soft AP available for normal operation */
#define SOFT_AP_SSID         "apollo"
#define SOFT_AP_PASSPHRASE   "abcd1234"
#define SOFT_AP_SECURITY     WICED_SECURITY_OPEN
#define SOFT_AP_CHANNEL      149
//#define SOFT_AP_VALID        WICED_TRUE

/* This is the default AP the device will connect to (as a client)*/
#define CLIENT_AP_SSID       "apollo"
#define CLIENT_AP_PASSPHRASE "abcd1234"
#define CLIENT_AP_BSS_TYPE   WICED_BSS_TYPE_ADHOC
#define CLIENT_AP_SECURITY   WICED_SECURITY_OPEN
#define CLIENT_AP_CHANNEL    149
#define CLIENT_AP_BAND       WICED_802_11_BAND_5GHZ

/* Override default country code */
#define WICED_COUNTRY_CODE    WICED_COUNTRY_UNITED_STATES

/* This is the network interface the device will work with */
#define WICED_NETWORK_INTERFACE   WICED_STA_INTERFACE

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

#ifdef __cplusplus
} /* extern "C" */
#endif
