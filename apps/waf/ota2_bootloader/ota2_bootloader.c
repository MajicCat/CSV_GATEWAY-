/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "wicedfs.h"
#include "platform_dct.h"
#include "elf.h"
#include "wiced_framework.h"
#include "wiced_utilities.h"
#include "platform_config.h"
#include "platform_resource.h"
#include "waf_platform.h"
#include "wwd_rtos.h"
#include "wiced_rtos.h"

#include "spi_flash.h"
#include "platform.h"
#include "platform_init.h"
#include "platform_stdio.h"
#include "platform_peripheral.h"

#include "wiced_ota2_image.h"

#include "mini_printf.h"

#define OTA2_BOOTLOADER_NO_SOFTAP_SUPPORT    1

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#ifndef OTA2_BOOTLOADER_NO_SOFTAP_SUPPORT
#define SOFTAP_START_BUTTON_MILLISECONDS_MIN     4000           // allow 4 - 6 seconds
#define SOFTAP_START_BUTTON_MILLISECONDS_MAX     6000
#define FACTORY_RESET_BUTTON_MILLISECONDS_MIN    9000           // allow 9 - 11 seconds
#define FACTORY_RESET_BUTTON_MILLISECONDS_MAX   11000
#else
/* no softap on startup here */
#define FACTORY_RESET_BUTTON_MILLISECONDS_MIN    4000           // allow 4 - 6 seconds
#define FACTORY_RESET_BUTTON_MILLISECONDS_MAX    6000
#endif

/******************************************************
 *                   Enumerations
 ******************************************************/
typedef enum
{
    START_SEQUENCE_RUN_APP          = 0,
    START_SEQUENCE_FACTORY_RESET,
    START_SEQUENCE_OTA_UPDATE,

#ifdef OTA2_USE_LAST_KNOWN_GOOD_AREA
    START_SEQUENCE_RESTORE_FROM_LKG,
#endif

#ifndef OTA2_BOOTLOADER_NO_SOFTAP_SUPPORT
    START_SEQUENCE_SOFT_AP,
#endif

} bootloader_start_sequence_t;


typedef enum
{
    START_RESULT_OK = 0,
    START_RESULT_ERROR_BATTERY_LOW,
    START_RESULT_ERROR_DCT_COPY_FAIL,
    START_RESULT_ERROR_UPGRADE_FAIL,
    START_RESULT_ERROR_FACTORY_RESET_FAIL,

#ifdef OTA2_USE_LAST_KNOWN_GOOD_AREA
    START_RESULT_ERROR_LKG_COPY_FAIL,
    START_RESULT_ERROR_LKG_RESTORE_FAIL,
#endif

} bootloader_start_result_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

static wiced_result_t load_program( const load_details_t * load_details, uint32_t* new_entry_point );

#ifndef OTA2_BOOTLOADER_NO_SOFTAP_SUPPORT
static int start_softap_and_webserver( void );
#endif

#ifdef OTA2_USE_LAST_KNOWN_GOOD_AREA
static int copy_current_apps_and_DCT_to_LKG( void );
static int copy_LKG_to_current_apps( void );
#endif

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

int main( void )
{
    bootloader_start_sequence_t start_sequence;
    const load_details_t*       load_details;
    uint32_t                    entry_point;
    boot_detail_t               boot;
    wiced_result_t              result;
    bootloader_start_result_t   boot_result;

    wiced_ota2_image_status_t   factory_reset_app_status;
    wiced_ota2_image_status_t   curr_app_status;
    wiced_ota2_image_status_t   staging_image_status;

    uint32_t                    button_pressed_milliseconds;

    boot_result = START_RESULT_OK;

    mini_printf("OTA2 Bootloader!\r\n");

    /* set up timing for no Operating System, we haven't started an OS yet */
    NoOS_setup_timing( );

    /* Assume a factory reset */
    start_sequence = START_SEQUENCE_FACTORY_RESET;
    result = wiced_ota2_image_get_status( WICED_OTA2_IMAGE_TYPE_FACTORY_RESET_APP, &factory_reset_app_status );
    if ((result != WICED_SUCCESS) && (factory_reset_app_status != WICED_OTA2_IMAGE_VALID))
    {
        mini_printf("Factory Reset Image invalid !!!!!!!!!!!!\r\n");
        start_sequence = START_SEQUENCE_RUN_APP;
    }

    /* Check to see if the Application is valid */
    result = wiced_ota2_image_get_status( WICED_OTA2_IMAGE_TYPE_CURRENT_APP, &curr_app_status );
    if ((result == WICED_SUCCESS) && (curr_app_status == WICED_OTA2_IMAGE_VALID))
    {
        /* if we don't have an override with OTA Update or Reset Button, use the current app */
        start_sequence = START_SEQUENCE_RUN_APP;
    }

    /* Determine if OTA Image Update is available
     *
     * NOTE: This must NOT over-ride the factory reset OR SoftAP sequences
     *       So we check it first, but do not act on it until later.
     *
    */
    result = wiced_ota2_image_get_status ( WICED_OTA2_IMAGE_TYPE_STAGED, &staging_image_status );
    if ((result == WICED_SUCCESS) && (staging_image_status == WICED_OTA2_IMAGE_EXTRACT_ON_NEXT_BOOT))
    {
        start_sequence = START_SEQUENCE_OTA_UPDATE;
    }

    /* check for factory reset button being pressed */
    button_pressed_milliseconds = wiced_waf_get_button_press_time();
    if (button_pressed_milliseconds > 1000)
    {
        mini_printf("button_pressed_milliseconds: %d\r\n", button_pressed_milliseconds);
    }

#ifndef OTA2_BOOTLOADER_NO_SOFTAP_SUPPORT
    /* determine if button held for ~5 seconds, start SoftAP for manual OTA upload */
    if ((button_pressed_milliseconds >= SOFTAP_START_BUTTON_MILLISECONDS_MIN) &&
        (button_pressed_milliseconds <= SOFTAP_START_BUTTON_MILLISECONDS_MAX))
    {
        /* go into SoftAP, DHCP & minimal WebServer */
        start_sequence = START_SEQUENCE_SOFT_AP;
    }
#endif
    /* determine if button held for ~10 seconds, run factory reset */
    if ((button_pressed_milliseconds >= FACTORY_RESET_BUTTON_MILLISECONDS_MIN) &&
        (button_pressed_milliseconds <= FACTORY_RESET_BUTTON_MILLISECONDS_MAX))
    {
        /* OTA Image factory reset here !!! */
        start_sequence = START_SEQUENCE_FACTORY_RESET;
    }

#ifdef CHECK_BATTERY_LEVEL_BEFORE_OTA2_UPGRADE
    if ((start_sequence == START_SEQUENCE_OTA_UPDATE) ||
        (start_sequence == START_SEQUENCE_FACTORY_RESET))
    {
        /* check for battery level before doing any writing! */
        if (platform_check_battery_level(CHECK_BATTERY_LEVEL_OTA2_UPGRADE_MINIMUM) != WICED_SUCCESS)
        {
            /* check_battery_level() failed */
            battery_ok = 0;
            start_sequence = START_SEQUENCE_RUN_APP;
            boot_result = START_RESULT_ERROR_BATTERY_LOW;
        }
    }
#endif /* CHECK_BATTERY_LEVEL_BEFORE_OTA2_UPGRADE */


    result = WICED_SUCCESS;
    if ((start_sequence == START_SEQUENCE_OTA_UPDATE) ||
        (start_sequence == START_SEQUENCE_FACTORY_RESET))
    {
#ifdef OTA2_USE_LAST_KNOWN_GOOD_AREA
        result = wiced_dct_ota2_save_copy(OTA2_COPY_DCT_LKG);
        if (result != WICED_SUCCESS)
        {
            start_sequence = START_SEQUENCE_RUN_APP;    /* stops any other upgrades from happening */
            boot_result = START_RESULT_ERROR_DCT_COPY_FAIL;
        }
        if (copy_current_apps_to_LKG() != WICED_SUCCESS)
        {
            /* LKG copy failed */
            start_sequence = START_SEQUENCE_RUN_APP;    /* stops any other upgrades from happening */
            boot_result = START_RESULT_ERROR_LKG_COPY_FAIL;
        }
#endif

        if (start_sequence == START_SEQUENCE_OTA_UPDATE)
        {
            result = wiced_dct_ota2_save_copy(OTA2_BOOT_UPDATE);
            if (result != WICED_SUCCESS)
            {
                start_sequence = START_SEQUENCE_RUN_APP;    /* stops any other upgrades from happening */
                boot_result = START_RESULT_ERROR_DCT_COPY_FAIL;
            }
            result = wiced_ota2_image_extract ( WICED_OTA2_IMAGE_TYPE_STAGED );
            mini_printf("Extract STAGED OTA2 Image Done - result:%d\r\n", result);
            if (result != WICED_SUCCESS)
            {
                start_sequence = START_SEQUENCE_RUN_APP;    /* stops any other upgrades from happening */
                boot_result = START_RESULT_ERROR_UPGRADE_FAIL;
            }
            else
            {
                start_sequence = START_SEQUENCE_RUN_APP;    /* stops any other upgrades from happening */
                boot_result = START_RESULT_OK;
            }
        }
        else if (start_sequence == START_SEQUENCE_FACTORY_RESET)
        {
            /* OTA Image ready - extract! */
            mini_printf("Extract Factory -- save copy!\r\n");
            result = wiced_dct_ota2_save_copy(OTA2_BOOT_FACTORY_RESET);
            if (result != WICED_SUCCESS)
            {
                start_sequence = START_SEQUENCE_RUN_APP;    /* stops any other upgrades from happening */
                boot_result = START_RESULT_ERROR_DCT_COPY_FAIL;
            }
            result = wiced_ota2_image_extract ( WICED_OTA2_IMAGE_TYPE_FACTORY_RESET_APP );
            mini_printf("Extract Factory Reset OTA2 Image Done - result:%d\r\n", result);
            if (result != WICED_SUCCESS)
            {
                boot_result = START_RESULT_ERROR_FACTORY_RESET_FAIL;
            }
            start_sequence = START_SEQUENCE_RUN_APP;    /* stops any other upgrades from happening */
        }

        if ((boot_result == START_RESULT_ERROR_UPGRADE_FAIL) ||
            (boot_result == START_RESULT_ERROR_FACTORY_RESET_FAIL))
        {
            mini_printf("Extract FAIL !!!\r\n");
#ifdef OTA2_USE_LAST_KNOWN_GOOD_AREA
            if (copy_LKG_to_current_apps() != WICED_SUCCESS)
            {
                start_sequence = START_SEQUENCE_RUN_APP;    /* stops any other upgrades from happening */
                boot_result = START_RESULT_ERROR_LKG_RESTORE_FAIL;
            }
#endif
        }
    }
    NoOS_stop_timing( );    /* TODO: is this the correct place? */

#ifndef OTA2_BOOTLOADER_NO_SOFTAP_SUPPORT
    if (start_sequence == START_SEQUENCE_SOFT_AP)
    {
            /* go into SoftAP, DHCP & minimal WebServer */
        start_softap_and_webserver();
    }
#endif

    mini_printf("OTA2 Bootloader -- start_sequence : %d ", start_sequence);
    mini_printf("curr_app_status: %d\r\n", curr_app_status  );

    if (start_sequence == START_SEQUENCE_RUN_APP)
    {
        /* after all the upgrade / factory reset / LKG work, check that our app is valid before running! */
        result = wiced_ota2_image_get_status ( WICED_OTA2_IMAGE_TYPE_CURRENT_APP, &curr_app_status);
        if ((result == WICED_SUCCESS) &&
            ((curr_app_status == WICED_OTA2_IMAGE_DOWNLOAD_COMPLETE) ||
             (curr_app_status == WICED_OTA2_IMAGE_VALID)))
        {

            /* boot the device with the current app */
            boot.load_details.valid = 1;
            boot.entry_point        = 0;
            wiced_dct_read_with_copy( &boot, DCT_INTERNAL_SECTION, OFFSET( platform_dct_header_t, boot_detail ), sizeof(boot_detail_t) );

            load_details = &boot.load_details;
            entry_point  = boot.entry_point;

            if ( load_details->valid != 0 )
            {
                if (load_program( load_details, &entry_point ) == WICED_SUCCESS)
                {
                    wiced_waf_start_app( entry_point );
                }
            }
        }
    }


    while(1)
    {
        (void)boot_result;
    }

    /* Should never get here */
    return 0;
}

static wiced_result_t load_program( const load_details_t * load_details, uint32_t* new_entry_point )
{
    wiced_result_t result = WICED_ERROR;

    if ( load_details->destination.id == EXTERNAL_FIXED_LOCATION )
    {
        /* External serial flash destination. Currently not allowed */
        result = WICED_ERROR;
    }
    else if ( load_details->source.id == EXTERNAL_FIXED_LOCATION )
    {
        /* Fixed location in serial flash source - i.e. no filesystem */
        result = wiced_waf_app_load( &load_details->source, new_entry_point );
    }
    else if ( load_details->source.id == EXTERNAL_FILESYSTEM_FILE )
    {
        /* Filesystem location in serial flash source */
        result = wiced_waf_app_load( &load_details->source, new_entry_point );
    }

    return result;
}

#ifdef OTA2_USE_LAST_KNOWN_GOOD_AREA
static int copy_current_apps_to_LKG( void )  // TODO: LKG support
{
    /* todo: copy current application area (all components) to LKG area */

    return WICED_ERROR;
}

static int copy_LKG_to_current_apps( void )  // TODO: LKG support
{
    /* todo: restore all LKG components to current application area */

    return WICED_ERROR;
}
#endif /* OTA2_USE_LAST_KNOWN_GOOD_AREA */

#ifndef OTA2_BOOTLOADER_NO_SOFTAP_SUPPORT
static int start_softap_and_webserver( void )
{
    return WICED_ERROR;
}
#endif /* OTA2_BOOTLOADER_NO_SOFTAP_SUPPORT */

