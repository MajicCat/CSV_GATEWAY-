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
 * USB Test Application
 *
 * This program calls the USB Host module init process, and then test by using a USB mass storage device.
 *
 * Application Instructions
 *   Connect a PC terminal to the serial port of the WICED Eval board,
 *   then build and download the application as described in the WICED
 *   Quick Start Guide
 *
 *   After download, the app runs through the different USB functions
 *
 */
#include "typedefs.h"
#include "bcmdevs.h"
#include "wiced_osl.h"
#include "wiced.h"
#include "command_console.h"
#include "command_console_fs.h"
#include "wiced_management.h"
#include "wiced_resource.h"
#include "wiced_filesystem.h"
#include "wiced_usb.h"
#include "usb.h"


/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
#define TEST_BUFFER_SIZE        (64*1024)

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
 *               Static Function Declarations
 ******************************************************/
static wiced_result_t usb_host_test_event_handler( uint32_t evt, void *param1, void *param2 );

/******************************************************
 *               Variable Definitions
 ******************************************************/
static int                  is_usb_init_ok = 0;
static int                  is_mass_storage_ready = 0;

/******************************************************
 *               Function Definitions
 ******************************************************/
int usb_host_test(void)
{
    wiced_usb_user_config_t user_cfg;
    UINT status;
    int ret_code = 0;
    int cnt;
    //CHAR* src_file_name = "\\IMAGE.TXT";
    //CHAR* dst_file_name = "\\COPYFILE.TXT";

    printf("\n*** USB20 Host diagnostic start!\n");

    if (is_usb_init_ok == 0)
    {
        /* Init Filesystem  */
        status = wiced_filesystem_init();
        if(status != WICED_SUCCESS)
        {
            printf("Filesystem init failed. status=%d\n", status);
            ret_code = -1;
            goto exit;
        }

        /* Init USB20 Host SW & HW  */
        is_mass_storage_ready = 0;

        memset((void*)&user_cfg, 0, sizeof(user_cfg));
        user_cfg.host_event_callback = usb_host_test_event_handler;

        status = wiced_usb_host_init(&user_cfg);
        if(status != WICED_SUCCESS)
        {
            printf("USB20 Host init failed. status=%d\n", status);
            ret_code = -2;
            goto exit;
        }
        is_usb_init_ok = 1;

        wiced_rtos_delay_milliseconds(500);
    }

    /* Wait USB disk plugged in, or timeout to error  */
    cnt = 300;
    while ((is_mass_storage_ready == 0) && (cnt > 0))
    {
        printf("Please plug in USB disk...\n");
        wiced_rtos_delay_milliseconds(1000);
        cnt --;
        if (cnt == 0)
        {
            printf("No disk plugged!\n");
            ret_code = -3;
            goto exit;
        }
    }

    printf("USB disk ready. Starting file read/write test...\n");

    /* USB Host mass storage single file read/write test  */
    if (file_rw_sha1sum_test(0, NULL) != ERR_CMD_OK)
    {
        printf("File read/write with SHA1sum check failed!\n");
        ret_code = -4;
        goto exit;
    }

exit:
    printf("\n*** USB20 Host diagnostic: %s (ret_code=%d)\n", (ret_code == 0) ? "SUCCEED" : "FAILED", ret_code);

    return ret_code;
}

static wiced_result_t usb_host_test_event_handler( uint32_t evt, void *param1, void *param2 )
{
    printf("USB20 Host: evt (%ld)\n", evt);

    switch (evt)
    {
        case USB_HOST_EVENT_HID_DEVICE_INSERTION:
            printf("USB_HOST_EVENT_HID_DEVICE_INSERTION\n");
            break;

        case USB_HOST_EVENT_HID_DEVICE_REMOVAL:
            printf("USB_HOST_EVENT_HID_DEVICE_REMOVAL\n");
            break;

        case USB_HOST_EVENT_STORAGE_DEVICE_INSERTION:
            printf("USB_HOST_EVENT_STORAGE_DEVICE_INSERTION\n");
            is_mass_storage_ready = 1;
            break;

        case USB_HOST_EVENT_STORAGE_DEVICE_REMOVAL:
            printf("USB_HOST_EVENT_STORAGE_DEVICE_REMOVAL\n");
            is_mass_storage_ready = 0;
            break;

        case USB_HOST_EVENT_AUDIO_DEVICE_INSERTION:
            printf("USB_HOST_EVENT_AUDIO_DEVICE_INSERTION\n");
            break;

        case USB_HOST_EVENT_AUDIO_DEVICE_REMOVAL:
            printf("USB_HOST_EVENT_AUDIO_DEVICE_REMOVAL\n");
            break;

        default :
            printf("USB20 Host: unknown evt (%ld)\n", evt);
            break;
    }

    return WICED_SUCCESS;
}
