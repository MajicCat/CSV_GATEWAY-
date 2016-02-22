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
 * Defines peripherals available for use on BCM94WCDPLUS114 board
 */
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
/*
BCM9WCDPLUS114 platform pin definitions ...
+----------------------------------------------------------------------------------------+
|Pin |  Pin Name on    |    Module     | STM32| Peripheral  |    Board     | Peripheral  |
| #  |     Module      |  GPIO Alias   | Port | Available   |  Connection  |   Alias     |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 40 | MICRO_GPIO1     | WICED_GPIO_1  | B  0 | ADC12_IN8   |              |             |
|    |                 |               |      | GPIO        | BUTTON SW1   |             |
|    |                 |               |      | TIM1_CH2_N  |              |             |
|    |                 |               |      | TIM3_CH3    |              |             |
|    |                 |               |      | TIM8_CH2N   |              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 41 | MICRO_GPIO2     | WICED_GPIO_2  | B  1 | ADC12_IN9   |              |             |
|    |                 |               |      | GPIO        | BUTTON SW2   |             |
|    |                 |               |      | TIM3_CH4    |              |             |
|    |                 |               |      | TIM8_CH3N   |              |             |
|    |                 |               |      | TIM1_CH3N   |              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 42 | MICRO_GPIO3     | WICED_GPIO_3  | C  0 | ADC123_IN11 |              |             |
|    |                 |               |      | GPIO        | BT_RST_N     |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 33 | TESTMODE        | WICED_GPIO_4  | A  8 | I2C3_SCL    |              |             |
|    |                 |               |      | GPIO        |              |             |
|    |                 |               |      | TIM1_CH1    |              |             |
|    |                 |               |      | USART1_CK   |              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 43 | MICRO_GPIO5     | WICED_GPIO_5  | C  3 | ADC123_IN13 |              | WICED_ADC_3 |
|    |                 |               |      | GPIO        | THERMISTOR   |             |
|    |                 |               |      | I2S2_SD     |              |             |
|    |                 |               |      | SPI2_MOSI   |              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 44 | MICRO_GPIO6     | WICED_GPIO_6  | C  4 | ADC12_IN14  |              |             |
|    |                 |               |      | GPIO        |              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 45 | MICRO_GPIO7     | WICED_GPIO_7  | B  5 | GPIO        |              |             |
|    |                 |               |      | I2C1_SMBA   |              |             |
|    |                 |               |      | I2S3_SD     |              |             |
|    |                 |               |      | SPI1_MOSI   |              |             |
|    |                 |               |      | SPI3_MOSI   |              |             |
|    |                 |               |      | TIM3_CH2    |              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 46 | MICRO_GPIO8     | WICED_GPIO_8  | C  7 | GPIO        |              |             |
|    |                 |               |      | I2S3_MCK    |              |             |
|    |                 |               |      | TIM8_CH2    |              |             |
|    |                 |               |      | TIM3_CH2    |              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 47 | MICRO_GPIO9     | WICED_GPIO_9  | C 13 | GPIO        |              |             |
|    |                 |               |      | RTC_AF1     |              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 35 | MICRO_I2C1_SCL  | WICED_GPIO_10 | B  6 | GPIO        |              |             |
|    |                 |               |      | TIM4_CH1    |              |             |
|    |                 |               |      | I2C1_SCL    |              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 36 | MICRO_I2C1_SDA  | WICED_GPIO_11 | B  7 | GPIO        |              |             |
|    |                 |               |      | TIM4_CH2    |              |             |
|    |                 |               |      | I2C1_SDA    |              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 48 | MICRO_GPIO12    | WICED_GPIO_12 | C  1 | ADC123_IN11 |              |             |
|    |                 |               |      | GPIO        | BT_REG_EN    |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 49 | MICRO_GPIO13    | WICED_GPIO_13 | C  2 | ADC123_IN12 |              |             |
|    |                 |               |      | GPIO        |              |             |
|    |                 |               |      | SPI2_MISO   |              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 50 | MICRO_GPIO14    | WICED_GPIO_14 | C  5 | ADC12_IN15  |              |             |
|    |                 |               |      | GPIO        |              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 51 | MICRO_JTAG_TRST | WICED_GPIO_15 | B  4 | GPIO        |              |             |
|    |                 |               |      | NJTRST      |              |             |
|    |                 |               |      | SPI1_MISO   |              |             |
|    |                 |               |      | SPI3_MISO   |              |             |
|    |                 |               |      | TIM3_CH1    |              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 52 | MICRO_JTAG_TDO  | WICED_GPIO_16 | B  3 | GPIO        |              |             |
|    |                 |               |      | JTDO        |              |             |
|    |                 |               |      | SPI1_SCK    |              |             |
|    |                 |               |      | SPI3_SCK    |              |             |
|    |                 |               |      | I2S3_SCK    |              |             |
|    |                 |               |      | TIM2_CH2    |              |             |
|    |                 |               |      | TRACESWO    |              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 53 | MICRO_JTAG_TDI  | WICED_GPIO_17 | A 15 | GPIO        |              |             |
|    |                 |               |      | JTDI        |              |             |
|    |                 |               |      | I2S3_WS     |              |             |
|    |                 |               |      | SPI1_NSS    |              |             |
|    |                 |               |      | SPI3_NSS    |              |             |
|    |                 |               |      | TIM2_CH1_ETR|              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 54 | MICRO_JTAG_TMS  | WICED_GPIO_18 | A 13 | GPIO        |              |             |
|    |                 |               |      | JTMS-SWDIO  |              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 55 | MICRO_JTAG_TCK  | WICED_GPIO_19 | A 14 | GPIO        |              |             |
|    |                 |               |      | JTCK-SWCLK  |              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 61 | MICRO_UART1_RTS | WICED_GPIO_20 | A 12 | GPIO        |              |             |
|    |                 |               |      | TIM1_ETR    |              |             |
|    |                 |               |      | USART1_RTS  |              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 62 | MICRO_UART1_CTS | WICED_GPIO_21 | A 11 | GPIO        | LED D1       |             |
|    |                 |               |      | TIM1_CH4    |              | WICED_PWM_2 |
|    |                 |               |      | USART1_CTS  |              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 63 | MICRO_UART1_RXD | WICED_GPIO_22 | A 10 | GPIO        |              |             |
|    |                 |               |      | TIM1_CH3    |              |             |
|    |                 |               |      | USART1_RX   |              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 64 | MICRO_UART1_TXD | WICED_GPIO_23 | A  9 | GPIO        |              |             |
|    |                 |               |      | I2C3_SMBA   |              |             |
|    |                 |               |      | TIM1_CH2    |              |             |
|    |                 |               |      | USART1_TX   |              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 21 | MICRO_SPI1_MISO | WICED_GPIO_24 | A  6 | ADC12_IN6   |              |             |
|    |                 |               |      | GPIO        |              |             |
|    |                 |               |      | SPI1_MISO   |              |             |
|    |                 |               |      | TIM1_BKIN   |              |             |
|    |                 |               |      | TIM3_CH1    |              |             |
|    |                 |               |      | TIM8_BKIN   |              |             |
|    |                 |               |      | TIM13_CH1   |              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 22 | MICRO_SPI1_SCK  | WICED_GPIO_25 | A  5 | ADC12_IN5   |              |             |
|    |                 |               |      | DAC2_OUT    |              |             |
|    |                 |               |      | GPIO        |              |             |
|    |                 |               |      | SPI1_SCK    |              |             |
|    |                 |               |      | TIM2_CH1_ETR|              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 23 | MICRO_SPI1_MOSI | WICED_GPIO_26 | A  7 | ADC12_IN7   |              |             |
|    |                 |               |      | GPIO        | LED D2       |             |
|    |                 |               |      | SPI1_MOSI   |              |             |
|    |                 |               |      | TIM1_CH1N   |              |             |
|    |                 |               |      | TIM3_CH2    |              | WICED_PWM_1 |
|    |                 |               |      | TIM8_CH1N   |              |             |
|    |                 |               |      | TIM14_CH1   |              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 24 | MICRO_SPI1_NSS  | WICED_GPIO_27 | A  4 | ADC12_IN4   |              |             |
|    |                 |               |      | DAC1_OUT    |              |             |
|    |                 |               |      | GPIO        |              |             |
|    |                 |               |      | I2S3_WS     |              |             |
|    |                 |               |      | SPI1_NSS    |              |             |
|    |                 |               |      | SPI3_NSS    |              |             |
|    |                 |               |      | USART2_CK   |              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 29 | MICRO_UART2_RTS | WICED_GPIO_28 | A  1 | GPIO        | BT_UART_CTS  |             |
|    |                 |               |      | TIM2_CH2    |              |             |
|    |                 |               |      | TIM5_CH2    |              |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 30 | MICRO_UART2_CTS | WICED_GPIO_29 | A  0 | GPIO        | BT_UART_RTS  |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 31 | MICRO_UART2_RXD | WICED_GPIO_30 | A  3 | ADC123_IN3  |              |             |
|    |                 |               |      | GPIO        |              |             |
|    |                 |               |      | TIM2_CH4    |              |             |
|    |                 |               |      | TIM5_CH4    |              |             |
|    |                 |               |      | TIM9_CH2    |              |             |
|    |                 |               |      | UART2_RX    | BT_UART_TXD  |             |
|----+-----------------+---------------+------+-------------+--------------+-------------|
| 32 | MICRO_UART2_TXD | WICED_GPIO_31 | A  2 | ADC123_IN2  |              |             |
|    |                 |               |      | GPIO        |              |             |
|    |                 |               |      | TIM2_CH3    |              |             |
|    |                 |               |      | TIM5_CH3    |              |             |
|    |                 |               |      | TIM9_CH1    |              |             |
|    |                 |               |      | USART2_TX   | BT_UART_RXD  |             |
+----------------------------------------------------------------------------------------+
Notes
1. These mappings are defined in <WICED-SDK>/Platform/BCM9WCDPLUS114/platform.c
2. Connections to the Bluetooth module are defined in <WICED-SDK>/Library/bluetooth/include/platform/UART/BCM9WCDPLUS114/wiced_bt_platform.h
2. STM32F2xx Datasheet  -> http://www.st.com/web/en/resource/technical/document/datasheet/CD00237391.pdf
3. STM32F2xx Ref Manual -> http://www.st.com/web/en/resource/technical/document/reference_manual/CD00225773.pdf
*/

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
    WICED_GPIO_1,
    WICED_GPIO_2,
    WICED_GPIO_3,
    WICED_GPIO_4,
    WICED_GPIO_5,
    WICED_GPIO_6,
    WICED_GPIO_7,
    WICED_GPIO_8,
    WICED_GPIO_9,
    WICED_GPIO_10,
    WICED_GPIO_11,
    WICED_GPIO_12,
    WICED_GPIO_13,
    WICED_GPIO_14,
    WICED_GPIO_15,
    WICED_GPIO_16,
    WICED_GPIO_17,
    WICED_GPIO_18,
    WICED_GPIO_19,
    WICED_GPIO_20,
    WICED_GPIO_21,
    WICED_GPIO_22,
    WICED_GPIO_23,
    WICED_GPIO_24,
    WICED_GPIO_25,
    WICED_GPIO_26,
    WICED_GPIO_27,
    WICED_GPIO_28,
    WICED_GPIO_29,
    WICED_GPIO_30,
    WICED_GPIO_31,

    WICED_GPIO_MAX, /* Denotes the total number of GPIO aliases. Not a valid GPIO alias */
    WICED_GPIO_32BIT = 0x7FFFFFFF,
} wiced_gpio_t;

typedef enum
{
    WICED_SPI_1,
    WICED_SPI_MAX, /* Denotes the total number of SPI port aliases. Not a valid SPI alias */
    WICED_SPI_32BIT = 0x7FFFFFFF,
} wiced_spi_t;

typedef enum
{
    WICED_I2C_1,
    WICED_I2C_MAX, /* Denotes the total number of I2C port aliases. Not a valid I2C alias */
    WICED_I2C_32BIT = 0x7FFFFFFF,
} wiced_i2c_t;

typedef enum
{
    WICED_PWM_1,
    WICED_PWM_2,
    WICED_PWM_3,
    WICED_PWM_4,
    WICED_PWM_5,
    WICED_PWM_6,
    WICED_PWM_7,
    WICED_PWM_8,
    WICED_PWM_9,
    WICED_PWM_MAX, /* Denotes the total number of PWM port aliases. Not a valid PWM alias */
    WICED_PWM_32BIT = 0x7FFFFFFF,
} wiced_pwm_t;

typedef enum
{
    WICED_ADC_1,
    WICED_ADC_2,
    WICED_ADC_3,
    WICED_ADC_MAX, /* Denotes the total number of ADC port aliases. Not a valid ADC alias */
    WICED_ADC_32BIT = 0x7FFFFFFF,
} wiced_adc_t;

typedef enum
{
    WICED_UART_1,
    WICED_UART_2,
    WICED_UART_MAX, /* Denotes the total number of UART port aliases. Not a valid UART alias */
    WICED_UART_32BIT = 0x7FFFFFFF,
} wiced_uart_t;

/* Logical Button-ids which map to phyiscal buttons on the board */
typedef enum
{
    PLATFORM_BUTTON_1,
    PLATFORM_BUTTON_2,
    PLATFORM_BUTTON_MAX, /* Denotes the total number of Buttons on the board. Not a valid Button Alias */
} platform_button_t;

/******************************************************
 *                    Constants
 ******************************************************/

#define WICED_PLATFORM_BUTTON_COUNT  ( 2 )

/* UART port used for standard I/O */
#define STDIO_UART ( WICED_UART_1 )

/*#define WICED_SPI_FLASH is internal to the module */
#define WICED_PLATFORM_INCLUDES_SPI_FLASH

/* Components connected to external I/Os*/
#define WICED_LED1                 ( WICED_GPIO_21 )
#define WICED_LED2                 ( WICED_GPIO_26 )
#define WICED_LED1_ON_STATE        ( WICED_ACTIVE_LOW )
#define WICED_LED2_ON_STATE        ( WICED_ACTIVE_LOW )

#define WICED_PLATFORM_MAX_BUTTON  (2)
#define WICED_BUTTON1              ( WICED_GPIO_1 )
#define WICED_BUTTON2              ( WICED_GPIO_2 )
#define WICED_THERMISTOR           ( WICED_GPIO_5 )
#define WICED_GPIO_AUTH_RST        ( WICED_GPIO_7 )

/* I/O connection <-> Peripheral Connections */
#define WICED_LED1_JOINS_PWM       ( WICED_PWM_2 )
#define WICED_LED2_JOINS_PWM       ( WICED_PWM_1 )
#define WICED_THERMISTOR_JOINS_ADC ( WICED_ADC_3 )

/*  Bootloader OTA/OTA2 LED to flash while "Factory Reset" button held */
#define PLATFORM_FACTORY_RESET_LED_GPIO     ( WICED_LED1 )
#define PLATFORM_FACTORY_RESET_LED_ON_STATE ( WICED_LED1_ON_STATE )

/* Bootloader OTA and OTA2 factory reset during settings */
#define PLATFORM_FACTORY_RESET_BUTTON_GPIO      ( WICED_BUTTON1 )
#define PLATFORM_FACTORY_RESET_PRESSED_STATE    (   0  )
#define PLATFORM_FACTORY_RESET_CHECK_PERIOD     (  100 )
#define PLATFORM_FACTORY_RESET_TIMEOUT          ( 5000 )

#ifdef __cplusplus
} /*extern "C" */
#endif
