/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

/** @file Audio Display Test App
 *
 * Audio Display Sample Application
 *
 * Features demonstrated:
 * - How to use the Audio Display library
 *
 * On startup this demo:
 * - Creates the audio display management thread
 * - Updates the screen to simulate use
 * - Demonstrates several options of the audio display library
 *
 * Application Instructions:
 * - Plug ssd1306_128x64_i2c_oled display into "display" header on BCM943907WAE_1 board
 *     - This uses I2C_1
 * - Power up the board, then build and download the application
 * - The display should cycle through several scenes
 *     - Header contains: signal strength, bluetooth, battery
 *     - Footer contains: song information or status messages
 *
 * General Audio Display Notes:
 * - The management thread updates battery and signal strength automatically
 *     - Battery is updated once per second, signal strength once every three seconds
 *     - User must update options for signal strength and battery
 * - Thread handles all of the drawing - user just needs to update icons
 *
 */

#include "audio_display.h"

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

void application_start()
{
    wiced_thread_t thread;

    audio_display_create_management_thread(&thread);

    while (1)
    {
        /* Display header icons - Signal strength, bluetooth, battery */
        audio_display_header_update_options(BATTERY_ICON_IS_VISIBLE | BATTERY_ICON_SHOW_PERCENT);
        u8g_Delay(3000);
        audio_display_header_update_options(BATTERY_ICON_IS_VISIBLE | BATTERY_ICON_SHOW_PERCENT | SIGNAL_STRENGTH_IS_VISIBLE);
        u8g_Delay(3000);
        audio_display_header_update_bluetooth(BLUETOOTH_IS_CONNECTED);
        u8g_Delay(5000);

        /* Song information can be updated all at once. */
        audio_display_update_footer("Song Title", "Artist", 34, 124, FOOTER_IS_VISIBLE | FOOTER_CENTER_ALIGN);
        u8g_Delay(1000);
        audio_display_footer_update_time_info(35, 124);
        u8g_Delay(1000);
        audio_display_footer_update_time_info(36, 124);
        u8g_Delay(1000);
        audio_display_footer_update_time_info(37, 124);
        u8g_Delay(1000);
        audio_display_footer_update_time_info(38, 124);
        u8g_Delay(1000);
        audio_display_footer_update_time_info(39, 124);
        u8g_Delay(3000);

        /* Song information can also be updated separately */
        audio_display_footer_update_song_info("New Song", "New Artist");
        audio_display_footer_update_time_info(0, 234);
        u8g_Delay(1000);
        audio_display_footer_update_time_info(1, 234);
        u8g_Delay(1000);
        audio_display_footer_update_time_info(2, 234);
        u8g_Delay(1000);
        audio_display_footer_update_time_info(3, 234);
        u8g_Delay(1000);
        audio_display_footer_update_time_info(4, 234);
        u8g_Delay(1000);
        audio_display_footer_update_time_info(5, 234);
        u8g_Delay(3000);

        /* Song duration can be toggled off.
         * The battery charging icon and bluetooth connected icon can change colors.
         */
        audio_display_footer_update_song_info("Battery and Bluetooth", "Colors can be toggled");
        audio_display_footer_update_options(FOOTER_IS_VISIBLE | FOOTER_CENTER_ALIGN | FOOTER_HIDE_SONG_DURATION);
        u8g_Delay(3000);
        audio_display_header_update_options(BATTERY_ICON_IS_VISIBLE | BATTERY_ICON_SHOW_PERCENT | INVERT_BATTERY_CHARGING_COLORS | BLUETOOTH_IS_CONNECTED | INVERT_BLUETOOTH_ICON_COLORS | SIGNAL_STRENGTH_IS_VISIBLE);
        u8g_Delay(5000);

        /* All icons can be hidden or made visible. */
        audio_display_footer_update_options(0x00);
        audio_display_header_update_options(0x00);
        u8g_Delay(1000);
    }
}
