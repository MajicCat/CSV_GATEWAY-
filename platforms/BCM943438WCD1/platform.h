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
 * Defines peripherals available for use on BCM943438WCD1 board
 */
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
/*
BCM943438WCD1 platform pin definitions ...
+--------------------------------------------------------------------------------------------------------+
| Enum ID       |Pin |   Pin Name on    |    Module     | STM32| Peripheral  |    Board     | Peripheral  |
|               | #  |      Module      |  GPIO Alias   | Port | Available   |  Connection  |   Alias     |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_1  | 26 | MICRO_WKUP       | WICED_GPIO_1  | A  0 | GPIO        |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_2  | 29 | MICRO_ADC_IN1    | WICED_GPIO_2  | A  1 | GPIO        | BUTTON SW2   |             |
|               |    |                  |               |      | TIM2_CH2    |              |             |
|               |    |                  |               |      | TIM5_CH2    |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_3  | 30 | MICRO_ADC_IN2    | WICED_GPIO_3  | A  2 | ADC123_IN2  |              |             |
|               |    |                  |               |      | GPIO        | BUTTON SW1   |             |
|               |    |                  |               |      | TIM2_CH3    |              |             |
|               |    |                  |               |      | TIM5_CH3    |              |             |
|               |    |                  |               |      | TIM9_CH1    |              |             |
|               |    |                  |               |      | USART2_TX   |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_4  | 31 | MICRO_ADC_IN3    | WICED_GPIO_4  | A  3 | ADC123_IN3  |              | WICED_ADC_3 |
|               |    |                  |               |      | GPIO        | THERMISTOR   |             |
|               |    |                  |               |      | TIM2_CH4    |              |             |
|               |    |                  |               |      | TIM5_CH4    |              |             |
|               |    |                  |               |      | TIM9_CH2    |              |             |
|               |    |                  |               |      | UART2_RX    |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_5  | 32 | MICRO_SPI_SSN    | WICED_GPIO_5  | A  4 | ADC12_IN4   |              |             |
|               |    |                  |               |      | DAC1_OUT    |              |             |
|               |    |                  |               |      | GPIO        |              |             |
|               |    |                  |               |      | I2S3_WS     |              |             |
|               |    |                  |               |      | SPI1_NSS    |              |             |
|               |    |                  |               |      | SPI3_NSS    |              |             |
|               |    |                  |               |      | USART2_CK   |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_6  | 33 | MICRO_SPI_SCK    | WICED_GPIO_6  | A  5 | ADC12_IN5   |              |             |
|               |    |                  |               |      | DAC2_OUT    |              |             |
|               |    |                  |               |      | GPIO        |              |             |
|               |    |                  |               |      | SPI1_SCK    |              |             |
|               |    |                  |               |      | TIM2_CH1_ETR|              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_7  | 34 | MICRO_SPI_MISO   | WICED_GPIO_7  | A  6 | ADC12_IN6   |              |             |
|               |    |                  |               |      | GPIO        |              |             |
|               |    |                  |               |      | SPI1_MISO   |              |             |
|               |    |                  |               |      | TIM1_BKIN   |              |             |
|               |    |                  |               |      | TIM3_CH1    |              |             |
|               |    |                  |               |      | TIM8_BKIN   |              |             |
|               |    |                  |               |      | TIM13_CH1   |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_8  | 35 | MICRO_SPI_MOSI   | WICED_GPIO_8  | A  7 | ADC12_IN7   |              |             |
|               |    |                  |               |      | GPIO        |              |             |
|               |    |                  |               |      | SPI1_MOSI   |              |             |
|               |    |                  |               |      | TIM1_CH1N   |              |             |
|               |    |                  |               |      | TIM3_CH2    |              |             |
|               |    |                  |               |      | TIM8_CH1N   |              |             |
|               |    |                  |               |      | TIM14_CH1   |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_9  | 03 | MICRO_UART_TX    | WICED_GPIO_9  | A  9 | GPIO        |              |             |
|               |    |                  |               |      | I2C3_SMBA   |              |             |
|               |    |                  |               |      | TIM1_CH2    |              |             |
|               |    |                  |               |      | USART1_TX   |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_10 | 04 | MICRO_UART_RX    | WICED_GPIO_10 | A 10 | GPIO        |              |             |
|               |    |                  |               |      | TIM1_CH3    |              |             |
|               |    |                  |               |      | USART1_RX   |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_11 | 36 | MICRO_GPIO_0     | WICED_GPIO_11 | B  0 | GPIO        | LED D4       |             |
|               |    |                  |               |      | TIM3_CH3    |              |             |
|               |    |                  |               |      |             |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_12 | 37 | MICRO_I2C2_SCL   | WICED_GPIO_12 | B 10 | GPIO        |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_13 | 07 | MICRO_JTAG_TMS   | WICED_GPIO_13 | A 13 | GPIO        |              |             |
|               |    |                  |               |      | JTMS-SWDIO  |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_14 | 08 | MICRO_JTAG_TCK   | WICED_GPIO_14 | A 14 | GPIO        |              |             |
|               |    |                  |               |      | JTCK-SWCLK  |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_15 | 09 | MICRO_JTAG_TDI   | WICED_GPIO_15 | A 15 | GPIO        |              |             |
|               |    |                  |               |      | JTDI        |              |             |
|               |    |                  |               |      | I2S3_WS     |              |             |
|               |    |                  |               |      | SPI1_NSS    |              |             |
|               |    |                  |               |      | SPI3_NSS    |              |             |
|               |    |                  |               |      | TIM2_CH1_ETR|              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_16 | 10 | MICRO_JTAG_TDO   | WICED_GPIO_16 | B  3 | GPIO        |              |             |
|               |    |                  |               |      | JTDO        |              |             |
|               |    |                  |               |      | SPI1_SCK    |              |             |
|               |    |                  |               |      | SPI3_SCK    |              |             |
|               |    |                  |               |      | I2S3_SCK    |              |             |
|               |    |                  |               |      | TIM2_CH2    |              |             |
|               |    |                  |               |      | TRACESWO    |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_17 | 11 | MICRO_JTAG_TRSTN | WICED_GPIO_17 | B  4 | GPIO        |              |             |
|               |    |                  |               |      | NJTRST      |              |             |
|               |    |                  |               |      | SPI1_MISO   |              |             |
|               |    |                  |               |      | SPI3_MISO   |              |             |
|               |    |                  |               |      | TIM3_CH1    |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_18 | 05 | MICRO_UART_CTS   | WICED_GPIO_18 | A 11 | GPIO        |              |             |
|               |    |                  |               |      | UART_CTS    |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_19 | 06 | MICRO_UART_RTS   | WICED_GPIO_19 | A 12 | GPIO        |              |             |
|               |    |                  |               |      | UART_RTS    |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_20 | 22 | MICRO_GPIO_2     | WICED_GPIO_20 | C  0 | GPIO        |              |             |
|               |    |                  |               |      |             |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_21 | 23 | MICRO_GPIO_3     | WICED_GPIO_21 | C  1 | GPIO        |              |             |
|               |    |                  |               |      |             |              |             |
+---------------+----+------------------+------+---------------+-------------+--------------+-------------+
| WICED_GPIO_22 | 24 | MICRO_GPIO_4     | WICED_GPIO_22 | C  2 | GPIO        |              |             |
|               |    |                  |               |      |             |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_23 | 25 | MICRO_GPIO_5     | WICED_GPIO_23 | C  3 | GPIO        |              |             |
|               |    |                  |               |      |             |              |             |
+---------------+----+------------------+------+---------------+-------------+--------------+-------------+
| WICED_GPIO_24 | 12 | MICRO_I2C1_SCL   | WICED_GPIO_24 | B  6 | GPIO        |              |             |
|               |    |                  |               |      | I2C1_SCL    |              |             |
+---------------+----+------------------+------+---------------+-------------+--------------+-------------+
| WICED_GPIO_25 | 13 | MICRO_I2C1_SDA   | WICED_GPIO_25 | B  7 | GPIO        |              |             |
|               |    |                  |               |      | I2C1_SDA    |              |             |
|---------------+----+------------------+---------------+------+-------------+--------------+-------------|
| WICED_GPIO_26 | 14 | MICRO_GPIO_1     | WICED_GPIO_26 | B  8 | GPIO        |   LED D3     |             |
|               |    |                  |               |      |             |              |             |
+---------------+----+------------------+------+---------------+-------------+--------------+-------------+
| WICED_GPIO_27 | 15 | MICRO_I2C2_SDA   | WICED_GPIO_27 | B  9 | GPIO        |              |             |
|               |    |                  |               |      | I2C2_SDA    |              |             |
+---------------+----+------------------+------+---------------+-------------+--------------+-------------+
Notes
1. These mappings are defined in <WICED-SDK>/platforms/BCM943438WCD1/platform.c
2. stmf411 datasheet found in http://www.st.com/web/en/resource/technical/document/datasheet/DM00115249.pdf
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
    WICED_GPIO_MAX, /* Denotes the total number of GPIO port aliases. Not a valid GPIO alias */
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
    WICED_I2C_MAX,
    WICED_I2C_32BIT = 0x7FFFFFFF,
} wiced_i2c_t;

typedef enum
{
    WICED_I2S_NONE,
    WICED_I2S_MAX, /* Denotes the total number of I2S port aliases.  Not a valid I2S alias */
    WICED_I2S_32BIT = 0x7FFFFFFF
} wiced_i2s_t;

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

/* SPI flash is present */
#define WICED_PLATFORM_INCLUDES_SPI_FLASH
#define WICED_SPI_FLASH_CS ( WICED_GPIO_5 )

/* Components connected to external I/Os */
#define WICED_LED1         ( WICED_GPIO_11 )
#define WICED_LED2         ( WICED_GPIO_26 )
#define WICED_BUTTON1      ( WICED_GPIO_3 )
#define WICED_BUTTON2      ( WICED_GPIO_2 )
#define WICED_THERMISTOR   ( WICED_GPIO_4 )

/* I/O connection <-> Peripheral Connections */
#define WICED_LED1_JOINS_PWM        ( WICED_PWM_1 )
#define WICED_LED2_JOINS_PWM        ( WICED_PWM_2 )
#define WICED_THERMISTOR_JOINS_ADC  ( WICED_ADC_3 )

/*  Bootloader OTA/OTA2 LED to flash while "Factory Reset" button held */
#define PLATFORM_FACTORY_RESET_LED_GPIO      ( WICED_LED1 )
#define PLATFORM_FACTORY_RESET_LED_ON_STATE  ( WICED_ACTIVE_HIGH )

/* Bootloader OTA and OTA2 factory reset during settings */
#define PLATFORM_FACTORY_RESET_BUTTON_GPIO      ( WICED_BUTTON1 )
#define PLATFORM_FACTORY_RESET_PRESSED_STATE    (   0  )
#define PLATFORM_FACTORY_RESET_CHECK_PERIOD     (  100 )
#define PLATFORM_FACTORY_RESET_TIMEOUT          ( 5000 )

#ifdef __cplusplus
} /*extern "C" */
#endif
