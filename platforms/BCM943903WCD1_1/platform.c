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
 * Defines board support package for BCM943903WCD1_1 board
 */
#include "stdio.h"
#include "string.h"
#include "platform.h"
#include "gpio_irq.h"
#include "platform_config.h"
#include "platform_peripheral.h"
#include "platform_mcu_peripheral.h"
#include "platform_ethernet.h"
#include "wwd_assert.h"
#include "platform/wwd_platform_interface.h"
#include "wiced_platform.h"
#include "platform_appscr4.h"
#include "platform_bluetooth.h"
#include "platform_mfi.h"
#include "gpio_button.h"

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
 *               Variables Definitions
 ******************************************************/

/* Platform pin mapping table. Used by WICED/platform/MCU/wiced_platform_common.c.*/
const platform_gpio_t platform_gpio_pins[] =
{
    [WICED_GPIO_1          ]    = { PIN_GPIO_0 },
    [WICED_GPIO_2          ]    = { PIN_GPIO_1 },
    [WICED_GPIO_3          ]    = { PIN_GPIO_7 },
    [WICED_GPIO_4          ]    = { PIN_GPIO_8 },
    [WICED_GPIO_5          ]    = { PIN_GPIO_9 },
    [WICED_GPIO_6          ]    = { PIN_GPIO_10 },
    [WICED_GPIO_7          ]    = { PIN_GPIO_11 },
    [WICED_GPIO_8          ]    = { PIN_GPIO_12 },
    [WICED_GPIO_9          ]    = { PIN_GPIO_13 },
    [WICED_GPIO_10         ]    = { PIN_GPIO_14 },
    [WICED_GPIO_11         ]    = { PIN_GPIO_15 },
    [WICED_GPIO_12         ]    = { PIN_GPIO_16 },
    [WICED_GPIO_13         ]    = { PIN_SPI_0_MISO },
    [WICED_GPIO_14         ]    = { PIN_SPI_0_CLK },
    [WICED_GPIO_15         ]    = { PIN_SPI_0_MOSI },
    [WICED_GPIO_16         ]    = { PIN_SPI_0_CS },
    [WICED_GPIO_17         ]    = { PIN_GPIO_2 },
    [WICED_GPIO_18         ]    = { PIN_GPIO_3 },
    [WICED_GPIO_19         ]    = { PIN_GPIO_4 },
    [WICED_GPIO_20         ]    = { PIN_GPIO_5 },
    [WICED_GPIO_21         ]    = { PIN_GPIO_6 },
    [WICED_PERIPHERAL_PIN_1]    = { PIN_RF_SW_CTRL_6 },
    [WICED_PERIPHERAL_PIN_2]    = { PIN_RF_SW_CTRL_7 },
    [WICED_PERIPHERAL_PIN_3]    = { PIN_UART0_RXD },
    [WICED_PERIPHERAL_PIN_4]    = { PIN_UART0_TXD },
    [WICED_PERIPHERAL_PIN_5]    = { PIN_UART0_CTS },
    [WICED_PERIPHERAL_PIN_6]    = { PIN_UART0_RTS },
    [WICED_PERIPHERAL_PIN_7]    = { PIN_RF_SW_CTRL_8 },
    [WICED_PERIPHERAL_PIN_8]    = { PIN_RF_SW_CTRL_9 },
};

const platform_spi_t platform_spi_peripherals[] =
{
    [WICED_SPI_1]  =
    {
        .port                    = BCM4390X_SPI_0,
        .gpio_alternate_function = 1,
        .pin_mosi                = &platform_gpio_pins[WICED_GPIO_15],
        .pin_miso                = &platform_gpio_pins[WICED_GPIO_13],
        .pin_clock               = &platform_gpio_pins[WICED_GPIO_14],
    },
};


/* I2C peripherals. Used by WICED/platform/MCU/wiced_platform_common.c */
const platform_i2c_t platform_i2c_peripherals[] =
{
    [WICED_I2C_1] =
    {
        .port                    = BCM4390X_I2C_0,
        .driver                  = &i2c_gsio_driver,
    },
};

/* UART peripherals and runtime drivers. Used by WICED/platform/MCU/wiced_platform_common.c */
const platform_uart_t platform_uart_peripherals[] =
{
    [WICED_UART_1] = /* ChipCommon Slow UART */
    {
        .port    = UART_SLOW,
        .rx_pin  = &platform_gpio_pins[WICED_PERIPHERAL_PIN_1],
        .tx_pin  = &platform_gpio_pins[WICED_PERIPHERAL_PIN_2],
        .cts_pin = NULL,
        .rts_pin = NULL,
        .src_clk = CLOCK_ALP,
    },
    [WICED_UART_2] = /* ChipCommon Fast UART */
    {
        .port    = UART_FAST,
        .rx_pin  = &platform_gpio_pins[WICED_PERIPHERAL_PIN_3],
        .tx_pin  = &platform_gpio_pins[WICED_PERIPHERAL_PIN_4],
        .cts_pin = &platform_gpio_pins[WICED_PERIPHERAL_PIN_5],
        .rts_pin = &platform_gpio_pins[WICED_PERIPHERAL_PIN_6],
        .src_clk = CLOCK_HT,
    },
    [WICED_UART_3] = /* GCI UART */
    {
        .port    = UART_GCI,
        .rx_pin  = &platform_gpio_pins[WICED_PERIPHERAL_PIN_7],
        .tx_pin  = &platform_gpio_pins[WICED_PERIPHERAL_PIN_8],
        .cts_pin = NULL,
        .rts_pin = NULL,
        .src_clk = CLOCK_ALP,
    }
};
platform_uart_driver_t platform_uart_drivers[WICED_UART_MAX];

/* Bluetooth UART pins. Used by libraries/bluetooth/internal/bus/UART/bt_bus.c */
const platform_gpio_t* wiced_bt_uart_pins[] =
{
    [WICED_BT_PIN_UART_TX ] = &platform_gpio_pins[WICED_PERIPHERAL_PIN_4],
    [WICED_BT_PIN_UART_RX ] = &platform_gpio_pins[WICED_PERIPHERAL_PIN_3],
    [WICED_BT_PIN_UART_CTS] = &platform_gpio_pins[WICED_PERIPHERAL_PIN_5],
    [WICED_BT_PIN_UART_RTS] = &platform_gpio_pins[WICED_PERIPHERAL_PIN_6],
};

/* Bluetooth UART peripheral and runtime driver. Used by libraries/bluetooth/internal/bus/UART/bt_bus.c */
const platform_uart_t*  wiced_bt_uart_peripheral = &platform_uart_peripherals[WICED_UART_2];
platform_uart_driver_t* wiced_bt_uart_driver     = &platform_uart_drivers    [WICED_UART_2];

/* Bluetooth UART configuration. Used by libraries/bluetooth/internal/bus/UART/bt_bus.c */
const platform_uart_config_t wiced_bt_uart_config =
{
    .baud_rate    = 115200,
    .data_width   = DATA_WIDTH_8BIT,
    .parity       = NO_PARITY,
    .stop_bits    = STOP_BITS_1,
    .flow_control = FLOW_CONTROL_CTS_RTS,
};

/*BT chip specific configuration information*/
const platform_bluetooth_config_t wiced_bt_config =
{
    .patchram_download_mode      = PATCHRAM_DOWNLOAD_MODE_MINIDRV_CMD,
    .patchram_download_baud_rate = 115200,
    .featured_baud_rate          = 3000000
};

/* UART standard I/O configuration */
#ifndef WICED_DISABLE_STDIO
static const platform_uart_config_t stdio_config =
{
    .baud_rate    = 115200,
    .data_width   = DATA_WIDTH_8BIT,
    .parity       = NO_PARITY,
    .stop_bits    = STOP_BITS_1,
    .flow_control = FLOW_CONTROL_DISABLED,
};
#endif

const platform_hibernation_t hibernation_config =
{
    .clock              = PLATFORM_HIBERNATION_CLOCK_EXTERNAL,
    .hib_ext_clock_freq = 0, /* use default settings */
    .rc_code            = -1 /* use default settings */
};

/* Bluetooth control pins. Used by libraries/bluetooth/internal/bus/UART/bt_bus.c */
const platform_gpio_t* wiced_bt_control_pins[WICED_BT_PIN_MAX] =
{
    [WICED_BT_PIN_POWER]       = NULL,
    [WICED_BT_PIN_RESET]       = &platform_gpio_pins[WICED_GPIO_11],
    [WICED_BT_PIN_HOST_WAKE]   = &platform_gpio_pins[WICED_GPIO_10],
    [WICED_BT_PIN_DEVICE_WAKE] = &platform_gpio_pins[WICED_GPIO_2]
};

gpio_button_t platform_gpio_buttons[] =
{
    [PLATFORM_BUTTON_1] =
    {
        .polarity   = WICED_ACTIVE_HIGH,
        .gpio       = WICED_BUTTON1,
        .trigger    = IRQ_TRIGGER_BOTH_EDGES,
    },

    [PLATFORM_BUTTON_2] =
    {
        .polarity   = WICED_ACTIVE_HIGH,
        .gpio       = WICED_BUTTON2,
        .trigger    = IRQ_TRIGGER_BOTH_EDGES,
    },
};

/* MFI-related variables */
const wiced_i2c_device_t auth_chip_i2c_device =
{
    .port          = AUTH_IC_I2C_PORT,
    .address       = 0x11,
    .address_width = I2C_ADDRESS_WIDTH_7BIT,
    .speed_mode    = I2C_STANDARD_SPEED_MODE,
};

const platform_mfi_auth_chip_t platform_auth_chip =
{
    .i2c_device = &auth_chip_i2c_device,
    .reset_pin  = WICED_GPIO_AUTH_RST
};

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

void platform_init_external_devices( void )
{
    /* Initialise LEDs and turn off by default */
    platform_gpio_init( &platform_gpio_pins[WICED_LED1], OUTPUT_PUSH_PULL );
    platform_gpio_init( &platform_gpio_pins[WICED_LED2], OUTPUT_PUSH_PULL );
    platform_gpio_output_low( &platform_gpio_pins[WICED_LED1] );
    platform_gpio_output_low( &platform_gpio_pins[WICED_LED2] );

    /* Initialise buttons to input by default */
    platform_gpio_init( &platform_gpio_pins[WICED_BUTTON1], INPUT_PULL_UP );
    platform_gpio_init( &platform_gpio_pins[WICED_BUTTON2], INPUT_PULL_UP );

#ifndef WICED_DISABLE_STDIO
    /* Initialise UART standard I/O */
    platform_stdio_init( &platform_uart_drivers[STDIO_UART], &platform_uart_peripherals[STDIO_UART], &stdio_config );
#endif
}