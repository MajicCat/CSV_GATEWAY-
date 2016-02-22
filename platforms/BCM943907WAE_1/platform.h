/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */
#pragma once

#include "platform_sleep.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/* UART port used for standard I/O */
#define STDIO_UART  ( WICED_UART_1 )

/******************************************************
 *                   Enumerations
 ******************************************************/


/*
BCM943907WAE_1 platform pin definitions ...
+-------------------------------------------------------------------------------------------------------------------+
| Enum ID       |Pin  |   Pin Name on    |  Module                           |  Conn  |  Peripheral  |  Peripheral  |
|               | #   |   43907WAE_1     |  Alias                            |  Pin   |  Available   |    Alias     |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_1  | C2  | GPIO_0           | GPIO_0_HDR                        |    -   |  GPIO        |              |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_2  | A50 | GPIO_1           | GPIO_1_BT_DEV_WAKE                |    -   |  GPIO        |              |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_3  | B3  | GPIO_7           | GPIO_7_WCPU_BOOT_MODE             |    -   |  GPIO        |              |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_4  | B1  | GPIO_8           | GPIO_8_TAP_SEL                    |    -   |  GPIO        |              |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_5  | A3  | GPIO_9           | GPIO_9_USB_SEL                    |    -   |  GPIO        |              |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_6  | D7  | GPIO_10          | GPIO_10_DEEP_RST_AUTH_RST         |    -   |  GPIO        |              |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_7  | A63 | GPIO_11          | GPIO_11_ACPU_BOOT_MODE            |    -   |  GPIO        |              |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_8  | C14 | GPIO_12          | GPIO_12_CODEC_PDN                 |    -   |  GPIO        |              |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_9  | B25 | GPIO_13          | GPIO_13_SDIO_MODE_HDR             |    -   |  GPIO        |              |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_10 | B24 | GPIO_14          | GPIO_14_BT_HOST_WAKE              |    -   |  GPIO        |              | *
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_11 | B29 | GPIO_15          | GPIO_15_BT_RESET_N                |    -   |  GPIO        |              |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_12 | B40 | GPIO_16          | GPIO_16_IRQB                      |    -   |  GPIO        |              | *
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_13 | C18 | PWM_0            | 3V3_SW_PWM0                       |    -   |  PWM, GPIO   | GPIO_18      |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_14 | B44 | PWM_1            | 2V_SW_PWM1                        |    -   |  PWM, GPIO   | GPIO_19      |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_15 | A54 | PWM_2            | 1V8_LDO_PWM2                      |    -   |  PWM, GPIO   | GPIO_20      |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_16 | A55 | PWM_3            | PWM_3                             |    -   |  PWM, GPIO   | GPIO_21      |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_17 | A49 | PWM_4            | PWM_4                             |    -   |  PWM, GPIO   | GPIO_22      |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_18 | D8  | PWM_5            | PWM_5                             |    -   |  PWM, GPIO   | GPIO_23      |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_19 | A7  | SPI_0_MISO       | SD_MISO_HDR                       |    -   |  SPI, GPIO   | GPIO_24      |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_20 | A6  | SPI_0_CLK        | SD_CLK_HDR                        |    -   |  SPI, GPIO   | GPIO_25      |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_21 | A4  | SPI_0_MOSI       | SD_MOSI_HDR                       |    -   |  SPI, GPIO   | GPIO_26      |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_22 | A5  | SPI_0_CS         | SD_CS_HDR                         |    -   |  SPI, GPIO   | GPIO_27      |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_23 | C15 | GPIO_2           | GPIO_2_JTAG_TCK                   |    -   |  GPIO        |              |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_24 | D5  | GPIO_3           | GPIO_3_JTAG_TMS                   |    -   |  GPIO        |              |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_25 | C3  | GPIO_4           | GPIO_4_JTAG_TDI                   |    -   |  GPIO        |              |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_26 | B4  | GPIO_5           | GPIO_5_JTAG_TDO                   |    -   |  GPIO        |              |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_27 | C4  | GPIO_6           | GPIO_6_JTAG_TRST_L                |    -   |  GPIO        |              |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_28 | A33 | I2C_0_SDA        | I2C_0_SDA                         |    -   |  I2C, GPIO   | GPIO_28      |
|---------------+-----+------------------+-----------------------------------+--------+--------------+--------------|
| WICED_GPIO_29 | A26 | I2C_0_SCL        | I2C_0_SCL                         |    -   |  I2C, GPIO   | GPIO_29      |
+-------------------------------------------------------------------------------------------------------------------+

+--------------------------------------------------------------------------------------------------------------------------------+
| Enum ID                 |Pin  |  Pin Name on  |    Board                                 |  Conn  |  Peripheral |  Peripheral  |
|                         | #   |  43907WAE_1   |  Connection                              |  Pin   |  Available  |    Alias     |
|-------------------------+-----+---------------+------------------------------------------+--------+-------------+--------------|
| WICED_PERIPHERAL_PIN_1  | B37 | RF_SW_CTRL_6  | RF_SW_CTRL_6_UART1_RX_IN                 |  -     |  UART1      |              |
|-------------------------+-----+---------------+------------------------------------------+--------+-------------+--------------|
| WICED_PERIPHERAL_PIN_2  | B43 | RF_SW_CTRL_7  | RF_SW_CTRL_7_UART1_TX_OUT                |  -     |  UART1      |              |
|-------------------------+-----+---------------+------------------------------------------+--------+-------------+--------------|
| WICED_PERIPHERAL_PIN_3  | B14 | UART0_RXD_IN  | UART0_RXD_IN                             |  -     |  UART2      |              |
|-------------------------+-----+---------------+------------------------------------------+--------+-------------+--------------|
| WICED_PERIPHERAL_PIN_4  | B15 | UART0_TXD_OUT | UART0_TXD_OUT                            |  -     |  UART2      |              |
|-------------------------+-----+---------------+------------------------------------------+--------+-------------+--------------|
| WICED_PERIPHERAL_PIN_5  | A20 | UART0_CTS_IN  | UART0_CTS_IN                             |  -     |  UART2      |              |
|-------------------------+-----+---------------+------------------------------------------+--------+-------------+--------------|
| WICED_PERIPHERAL_PIN_6  | A21 | UART0_RTS_OUT | UART0_RTS_OUT                            |  -     |  UART2      |              |
|-------------------------+-----+---------------+------------------------------------------+--------+-------------+--------------|
| WICED_PERIPHERAL_PIN_7  | A61 | RF_SW_CTRL_8  | RF_SW_CTRL_8_BT_SEC_IN                   |  -     |  UART3      |              |
|-------------------------+-----+---------------+------------------------------------------+--------+-------------+--------------|
| WICED_PERIPHERAL_PIN_8  | C21 | RF_SW_CTRL_9  | RF_SW_CTRL_9_BT_SEC_OUT                  |  -     |  UART3      |              |
|-------------------------+-----+---------------+------------------------------------------+--------+-------------+--------------+
*/

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
    WICED_GPIO_MAX, /* Denotes the total number of GPIO port aliases. Not a valid GPIO alias */
} wiced_gpio_t;

typedef enum
{
    WICED_PERIPHERAL_PIN_1 = WICED_GPIO_MAX,
    WICED_PERIPHERAL_PIN_2,
    WICED_PERIPHERAL_PIN_3,
    WICED_PERIPHERAL_PIN_4,
    WICED_PERIPHERAL_PIN_5,
    WICED_PERIPHERAL_PIN_6,
    WICED_PERIPHERAL_PIN_7,
    WICED_PERIPHERAL_PIN_8,
    WICED_PERIPHERAL_PIN_MAX, /* Denotes the total number of GPIO and peripheral pins. Not a valid pin alias */
} wiced_peripheral_pin_t;

typedef enum
{
    WICED_SPI_1,
    WICED_SPI_2,
    WICED_SPI_MAX, /* Denotes the total number of SPI port aliases. Not a valid SPI alias */
} wiced_spi_t;

typedef enum
{
    WICED_I2C_1,
    WICED_I2C_2,
    WICED_I2C_MAX, /* Denotes the total number of I2C port aliases. Not a valid I2C alias */
} wiced_i2c_t;

typedef enum
{
    WICED_PWM_1,
    WICED_PWM_2,
    WICED_PWM_3,
    WICED_PWM_4,
    WICED_PWM_5,
    WICED_PWM_6,
    WICED_PWM_MAX, /* Denotes the total number of PWM port aliases. Not a valid PWM alias */
} wiced_pwm_t;

typedef enum
{
   WICED_ADC_NONE = 0xff, /* No ADCs */
} wiced_adc_t;


typedef enum
{
    WICED_UART_1,   /* ChipCommon Slow UART */
    WICED_UART_2,   /* ChipCommon Fast UART */
    WICED_UART_3,   /* GCI UART */
    WICED_UART_MAX, /* Denotes the total number of UART port aliases. Not a valid UART alias */
} wiced_uart_t;

typedef enum
{
    WICED_I2S_1,
    WICED_I2S_2,
    WICED_I2S_3,
    WICED_I2S_MAX, /* Denotes the total number of I2S port aliases. Not a valid I2S alias */
} wiced_i2s_t;

/* Logical Button-ids which map to phyiscal buttons on the board */
typedef enum
{
    PLATFORM_BUTTON_1,
    PLATFORM_BUTTON_2,
    PLATFORM_BUTTON_3,
    PLATFORM_BUTTON_4,
    PLATFORM_BUTTON_5,
    PLATFORM_BUTTON_6,
    PLATFORM_BUTTON_MAX, /* Denotes the total number of Buttons on the board. Not a valid Button alias */
} platform_button_t;

#define WICED_PLATFORM_BUTTON_COUNT  ( 6 )

/*No external GPIO LEDS are available*/
#define GPIO_LED_NOT_SUPPORTED

/* Components connected to external I/Os*/
/* Note: The BCM943907WAE_1 hardware does not have physical LEDs or buttons so the #defines are mapped to PWM 0,1,2 on the expansion header */
#define WICED_LED1       (WICED_GPIO_18)
#define WICED_LED2       (WICED_GPIO_19)
#define WICED_BUTTON1    (WICED_GPIO_20)
//#define WICED_BUTTON2    (WICED_GPIO_2)
//#define WICED_THERMISTOR (WICED_GPIO_4)
#define WICED_GPIO_AUTH_RST  ( WICED_GPIO_6 )
#define WICED_GPIO_AKM_PDN                      (WICED_GPIO_8)
#define WICED_GPIO_AKM_SWITCHER_3V3_PS_ENABLE   (WICED_GPIO_13)
#define WICED_GPIO_AKM_SWITCHER_2V_ENABLE       (WICED_GPIO_14)
#define WICED_GPIO_AKM_LDO_1V8_ENABLE           (WICED_GPIO_15)

/* I/O connection <-> Peripheral Connections */
//#define WICED_LED1_JOINS_PWM       (WICED_PWM_1)
//#define WICED_LED2_JOINS_PWM       (WICED_PWM_2)
//#define WICED_THERMISTOR_JOINS_ADC (WICED_ADC_3)

#define AUTH_IC_I2C_PORT (WICED_I2C_1)

/* common audio device defines */
#define PLATFORM_DEFAULT_AUDIO_INPUT    AUDIO_DEVICE_ID_AK4961_ADC_LINE
#define PLATFORM_AUDIO_NUM_INPUTS       2
#define PLATFORM_DEFAULT_AUDIO_OUTPUT   AUDIO_DEVICE_ID_AK4961_DAC_LINE
#define PLATFORM_AUDIO_NUM_OUTPUTS      1

#define WICED_BUTTON_MULTI_FUNC         ( WICED_GPIO_1 )
#define WICED_BUTTON_PAUSE_PLAY         ( WICED_GPIO_20 )
#define WICED_BUTTON_VOLUME_UP          ( WICED_GPIO_5 )
#define WICED_BUTTON_VOLUME_DOWN        ( WICED_GPIO_9 )
#define WICED_BUTTON_BACK               ( WICED_GPIO_18 )
#define WICED_BUTTON_FORWARD            ( WICED_GPIO_3 )


/*  Bootloader OTA/OTA2 LED to flash while "Factory Reset" button held */
#define PLATFORM_FACTORY_RESET_LED_GPIO      ( WICED_LED1 )
#define PLATFORM_FACTORY_RESET_LED_ON_STATE  ( WICED_ACTIVE_HIGH )

/* Bootloader OTA and OTA2 factory reset during settings */
#define PLATFORM_FACTORY_RESET_BUTTON_GPIO      ( WICED_BUTTON_BACK )
#define PLATFORM_FACTORY_RESET_PRESSED_STATE    (   0  )
#define PLATFORM_FACTORY_RESET_CHECK_PERIOD     (  100 )
#define PLATFORM_FACTORY_RESET_TIMEOUT          ( 5000 )

#ifdef __cplusplus
} /*extern "C" */
#endif
