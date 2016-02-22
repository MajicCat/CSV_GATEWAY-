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
#include "ssdp_app_dct.h"

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

DEFINE_APP_DCT(application_dct_t)
{
    .server_port        = 26790,                /* todo: choose a port                              */
    .notify_time        = 120,                  /* todo: choose notify timeout ( 60 - 1800 seconds) */
    .serve_path_page    = "index.html",         /* todo: set your page path                         */
};


/******************************************************
 *               Function Definitions
 ******************************************************/
