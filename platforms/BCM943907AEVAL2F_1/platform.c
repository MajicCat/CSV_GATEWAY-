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
 * Defines board support package for BCM943907AEVAL2F_1 board
 */
#include "platform_config.h"
#include "platform_appscr4.h"
#include "platform_bluetooth.h"
#include "platform_mfi.h"
#include "wiced_platform.h"

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
    [WICED_GPIO_13         ]    = { PIN_PWM_0 },
    [WICED_GPIO_14         ]    = { PIN_PWM_1 },
    [WICED_GPIO_15         ]    = { PIN_PWM_2 },
    [WICED_GPIO_16         ]    = { PIN_PWM_3 },
    [WICED_GPIO_17         ]    = { PIN_PWM_4 },
    [WICED_GPIO_18         ]    = { PIN_PWM_5 },
    [WICED_GPIO_19         ]    = { PIN_SPI_0_MISO },
    [WICED_GPIO_20         ]    = { PIN_SPI_0_CLK },
    [WICED_GPIO_21         ]    = { PIN_SPI_0_MOSI },
    [WICED_GPIO_22         ]    = { PIN_SPI_0_CS },
    [WICED_GPIO_23         ]    = { PIN_GPIO_2 },
    [WICED_GPIO_24         ]    = { PIN_GPIO_3 },
    [WICED_GPIO_25         ]    = { PIN_GPIO_4 },
    [WICED_GPIO_26         ]    = { PIN_GPIO_5 },
    [WICED_GPIO_27         ]    = { PIN_GPIO_6 },
    [WICED_GPIO_28         ]    = { PIN_I2S_MCLK0 },
    [WICED_GPIO_29         ]    = { PIN_I2S_SCLK0 },
    [WICED_GPIO_30         ]    = { PIN_I2S_LRCLK0 },
    [WICED_GPIO_31         ]    = { PIN_I2S_SDATAI0 },
    [WICED_GPIO_32         ]    = { PIN_I2S_SDATAO0 },
    [WICED_GPIO_33         ]    = { PIN_I2S_MCLK1 },
    [WICED_GPIO_34         ]    = { PIN_I2S_SCLK1 },
    [WICED_GPIO_35         ]    = { PIN_I2S_LRCLK1 },
    [WICED_GPIO_36         ]    = { PIN_I2S_SDATAI1 },
    [WICED_GPIO_37         ]    = { PIN_I2S_SDATAO1 },
    [WICED_GPIO_38         ]    = { PIN_SPI_1_CLK },
    [WICED_GPIO_39         ]    = { PIN_SPI_1_MISO },
    [WICED_GPIO_40         ]    = { PIN_SPI_1_MOSI },
    [WICED_GPIO_41         ]    = { PIN_SPI_1_CS },
    [WICED_GPIO_42         ]    = { PIN_SDIO_CLK },
    [WICED_GPIO_43         ]    = { PIN_SDIO_CMD },
    [WICED_GPIO_44         ]    = { PIN_SDIO_DATA_0 },
    [WICED_GPIO_45         ]    = { PIN_SDIO_DATA_1 },
    [WICED_GPIO_46         ]    = { PIN_SDIO_DATA_2 },
    [WICED_GPIO_47         ]    = { PIN_SDIO_DATA_3 },
    [WICED_GPIO_48         ]    = { PIN_I2C0_SDATA },
    [WICED_GPIO_49         ]    = { PIN_I2C0_CLK },
    [WICED_GPIO_50         ]    = { PIN_I2C1_SDATA },
    [WICED_GPIO_51         ]    = { PIN_I2C1_CLK },


    [WICED_PERIPHERAL_PIN_1]    = { PIN_RF_SW_CTRL_6 },
    [WICED_PERIPHERAL_PIN_2]    = { PIN_RF_SW_CTRL_7 },
    [WICED_PERIPHERAL_PIN_3]    = { PIN_UART0_RXD },
    [WICED_PERIPHERAL_PIN_4]    = { PIN_UART0_TXD },
    [WICED_PERIPHERAL_PIN_5]    = { PIN_UART0_CTS },
    [WICED_PERIPHERAL_PIN_6]    = { PIN_UART0_RTS },
    [WICED_PERIPHERAL_PIN_7]    = { PIN_RF_SW_CTRL_8 },
    [WICED_PERIPHERAL_PIN_8]    = { PIN_RF_SW_CTRL_9 },
};

/* PWM peripherals. Used by WICED/platform/MCU/wiced_platform_common.c */
const platform_pwm_t platform_pwm_peripherals[] =
{
    [WICED_PWM_1]  = {PIN_GPIO_0,  PIN_FUNCTION_PWM0, },   /* or PIN_GPIO_8, PIN_GPIO_10, PIN_GPIO_12, PIN_GPIO_14, PIN_GPIO_16, PIN_PWM_0 */
    [WICED_PWM_2]  = {PIN_GPIO_1,  PIN_FUNCTION_PWM1, },   /* or PIN_GPIO_7, PIN_GPIO_9,  PIN_GPIO_11, PIN_GPIO_13, PIN_GPIO_15, PIN_PWM_1 */
    [WICED_PWM_3]  = {PIN_GPIO_8,  PIN_FUNCTION_PWM2, },   /* or PIN_GPIO_0, PIN_GPIO_10, PIN_GPIO_12, PIN_GPIO_14, PIN_GPIO_16, PIN_PWM_2 */
    [WICED_PWM_4]  = {PIN_GPIO_7,  PIN_FUNCTION_PWM3, },   /* or PIN_GPIO_1, PIN_GPIO_9,  PIN_GPIO_11, PIN_GPIO_13, PIN_GPIO_15, PIN_PWM_3 */
    [WICED_PWM_5]  = {PIN_GPIO_10, PIN_FUNCTION_PWM4, },   /* or PIN_GPIO_0, PIN_GPIO_8,  PIN_GPIO_12, PIN_GPIO_14, PIN_GPIO_16, PIN_PWM_4 */
    [WICED_PWM_6]  = {PIN_GPIO_9,  PIN_FUNCTION_PWM5, },   /* or PIN_GPIO_1, PIN_GPIO_7,  PIN_GPIO_11, PIN_GPIO_13, PIN_GPIO_15, PIN_PWM_5 */
};

const platform_spi_t platform_spi_peripherals[] =
{
    [WICED_SPI_1]  =
    {
        .port                    = BCM4390X_SPI_0,
        .gpio_alternate_function = 1,
        .pin_mosi                = &platform_gpio_pins[WICED_GPIO_21],
        .pin_miso                = &platform_gpio_pins[WICED_GPIO_19],
        .pin_clock               = &platform_gpio_pins[WICED_GPIO_20],
    },

    [WICED_SPI_2]  =
    {
        .port                    = BCM4390X_SPI_1,
        .gpio_alternate_function = 1,
        .pin_mosi                = &platform_gpio_pins[WICED_GPIO_40],
        .pin_miso                = &platform_gpio_pins[WICED_GPIO_39],
        .pin_clock               = &platform_gpio_pins[WICED_GPIO_38],
    },
};

/* I2C peripherals. Used by WICED/platform/MCU/wiced_platform_common.c */
const platform_i2c_t platform_i2c_peripherals[] =
{
    [WICED_I2C_1] =
    {
        .port                    = BCM4390X_I2C_0,
        .pin_sda                 = &platform_gpio_pins[WICED_GPIO_48],
        .pin_scl                 = &platform_gpio_pins[WICED_GPIO_49],
        .driver                  = &i2c_gsio_driver,
    },

    [WICED_I2C_2] =
    {
        .port                    = BCM4390X_I2C_1,
        .pin_sda                 = &platform_gpio_pins[WICED_GPIO_50],
        .pin_scl                 = &platform_gpio_pins[WICED_GPIO_51],
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

const platform_i2s_port_info_t i2s_port_info[BCM4390X_I2S_MAX] =
{
    [BCM4390X_I2S_0] =
    {
        .port               = BCM4390X_I2S_0,
        .is_master          = 1,
        .irqn               = I2S0_ExtIRQn,
        .audio_pll_ch       = BCM4390X_AUDIO_PLL_MCLK1,
    },
    [BCM4390X_I2S_1] =
    {
        .port               = BCM4390X_I2S_1,
        .is_master          = 1,
        .irqn               = I2S1_ExtIRQn,
        .audio_pll_ch       = BCM4390X_AUDIO_PLL_MCLK2,
    },
};

const platform_i2s_t i2s_interfaces[WICED_I2S_MAX] =
{
    [WICED_I2S_1] =
    {
        .port_info          = &i2s_port_info[BCM4390X_I2S_0],
        .stream_direction   = PLATFORM_I2S_WRITE,
    },
    [WICED_I2S_2] =
    {
        .port_info          = &i2s_port_info[BCM4390X_I2S_0],
        .stream_direction   = PLATFORM_I2S_READ,
    },
    [WICED_I2S_3] =
    {
        .port_info          = &i2s_port_info[BCM4390X_I2S_1],
        .stream_direction   = PLATFORM_I2S_WRITE,
    },
};

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
#ifndef WICED_DISABLE_STDIO
    /* Initialise UART standard I/O */
    platform_stdio_init( &platform_uart_drivers[STDIO_UART], &platform_uart_peripherals[STDIO_UART], &stdio_config );
#endif
}
