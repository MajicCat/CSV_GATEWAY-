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
 * Proximity Server Sample Application (GATT Server database definitions)
 *
 */
#include "wiced.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    // ***** Primary service 'Generic Attribute'
    HDLS_GENERIC_ATTRIBUTE      =  0x0100,
    HDLC_GENERIC_ATTRIBUTE_SERVICE_CHANGED,
    HDLC_GENERIC_ATTRIBUTE_SERVICE_CHANGED_VALUE,

    // ***** Primary service 'Generic Access'
    HDLS_GENERIC_ACCESS,
    HDLC_GENERIC_ACCESS_DEVICE_NAME,
    HDLC_GENERIC_ACCESS_DEVICE_NAME_VALUE,
    HDLC_GENERIC_ACCESS_APPEARANCE,
    HDLC_GENERIC_ACCESS_APPEARANCE_VALUE,

    // ***** Primary service 'Link Loss'
    HDLS_LINK_LOSS,
    HDLC_LINK_LOSS_ALERT_LEVEL,
    HDLC_LINK_LOSS_ALERT_LEVEL_VALUE,

    // ***** Primary service 'Immediate Alert'
    HDLS_IMMEDIATE_ALERT,
    HDLC_IMMEDIATE_ALERT_LEVEL,
    HDLC_IMMEDIATE_ALERT_LEVEL_VALUE,

    // ***** Primary service 'TX Power'
    HDLS_TX_POWER,
    HDLC_TX_POWER_LEVEL,
    HDLC_TX_POWER_LEVEL_VALUE,
};


extern const uint8_t  gatt_db[];
extern const uint16_t gatt_db_size;

#ifdef __cplusplus
}
#endif
