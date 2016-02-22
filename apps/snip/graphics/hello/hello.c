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
 * U8G Library HelloWorld Sample Application
 *
 * Features demonstrated
 *  - Basic use of u8g library for i2c displays
 *  - Wiced I2C communication protocol
 *
 * On startup this demo:
 *  - Displays "Hello World!" on an attached display
 *
 * Application Instructions
 *   For 128x64 ssd1306 OLED displays:
 *     Attach the display to the WICED Eval board such that:
 *       - VCC goes to 3.3V power (J5 pin 5)
 *       - GND goes to ground     (J5 pin 2)
 *       - SCL goes to I2C_1_SCL  (J5 pin 1)
 *       - SDA goes to I2C_1_SDA  (J5 pin 3)
 *       (Pin headers/numbers apply to P201 Eval board)
 *     Build, download, and run the application. The display
 *     should show Hello World!
 *
 *   For other displays:
 *     Modify the wiced_i2c_device_t struct below for your
 *     specific device.
 *     Modify arg 2 of u8g_InitComFn() in application_start()
 *     to reflect the type of display being used. The u8g library
 *     supports many different types of displays; you can look
 *     through the various u8g_dev_* files for I2C constructors.
 *     Attach, build, download, and run as described above.
 *
 */

#include "u8g_arm.h"

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

/******************************************************
 *               Function Definitions
 ******************************************************/

void draw(u8g_t* u8g)
{
    u8g_SetFont(u8g, u8g_font_unifont);
    u8g_SetFontPosTop(u8g);
    u8g_DrawStr(u8g, 0, 10, "Hello World!");
}

void application_start()
{
    wiced_i2c_device_t oled_display =
    {
        .port          = WICED_I2C_2,
        .address       = 0x3C,
        .address_width = I2C_ADDRESS_WIDTH_7BIT,
        .flags         = 0,
        .speed_mode    = I2C_HIGH_SPEED_MODE,
    };
    u8g_t u8g;

    u8g_init_wiced_i2c_device(&oled_display);
    u8g_InitComFn(&u8g, &u8g_dev_ssd1306_128x64_i2c, u8g_com_hw_i2c_fn);

    while(1)
    {
        u8g_FirstPage(&u8g);

        do {
            draw(&u8g);
        } while (u8g_NextPage(&u8g));
    }
}
