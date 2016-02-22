/*
 * Copyright 2014, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

/** @file
 * Defines peripherals available for use on ISM43341_M4G_L44 board
 */
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif


/*
ISM43341-M4G-L44 platform pin definitions ...
+----------------------------------------------------------------------------------------------------+
|Pin |   Pin Name on           |    Module     | STM32| Peripheral      |    Board     | Peripheral  |
| #  |      Module             |  GPIO Alias   | Port | Available       |  Connection  |   Alias     |
|----+-------------------------+---------------+------+-----------------+--------------+-------------|
| 6  | MODULE_MICRO_JTAG_TDI   | WICED_GPIO_1  | A 15 | JTDI            |              |             |
|    |                         |               |      | SPI3_NSS        |              |             |
|    |                         |               |      | I2S3_WS         |              |             |
|    |                         |               |      | TIM2_CH1_ETR    |              |             |
|    |                         |               |      | SPI1_NSS        |              |             |
|    |                         |               |      | EVENTOUT        |              |             |
|----+-------------------------+---------------+------+-----------------+--------------+-------------|
| 7  | MODULE_MICRO_JTAG_TDO   | WICED_GPIO_2  | B 3  | JTDO            |              |             |
|    |                         |               |      | TRACESWO        |              |             |
|    |                         |               |      | SPI3_SCK        |              |             |
|    |                         |               |      | I2S3_CK         |              |             |
|    |                         |               |      | TIM2_CH2        |              |             |
|    |                         |               |      | SPI1_SCK        |              |             |
|    |                         |               |      | EVENTOUT        |              |             |
|----+-------------------------+---------------+------+-----------------+--------------+-------------|
| 8  |MODULE_MICRO_JTAG_TRSTN  | WICED_GPIO_3  | B 4  | NJTRST          |              |             |
|    |                         |               |      | SPI3_MISO       |              |             |
|    |                         |               |      | TIM3_CH1        |              |             |
|    |                         |               |      | SPI1_MISO       |              |             |
|    |                         |               |      | I2S3ext_SD      |              |             |
|    |                         |               |      | EVENTOUT        |              |             |
|----+-------------------------+---------------+------+-----------------+--------------+-------------|
| 22 | MODULE_MICRO_UART1_TX   | WICED_GPIO_4  | A 9  | USART1_TX       |              |             |
|    |                         |               |      | TIM1_CH2        |              |             |
|    |                         |               |      | I2c3SMBA        |              |             |
|    |                         |               |      | DCMI_D0         |              |             |
|    |                         |               |      | EVENTOUT        |              |             |
|----+-------------------------+---------------+------+-----------------+--------------+-------------|
| 21 | MODULE_MICRO_UART1_RX   | WICED_GPIO_5  | A 10 | USART1_RX       |              |             |
|    |                         |               |      | TIM1_CH3        |              |             |
|    |                         |               |      | OTG_FS_ID       |              |             |
|    |                         |               |      | DCMI_D1         |              |             |
|    |                         |               |      | EVENTOUT        |              |             |
|----+-------------------------+---------------+------+-----------------+--------------+-------------|
| 18 | MODULE_MICRO_UART1_RTS  | WICED_GPIO_6  | A 11 | USART1_CTS      |              |             |
|    |                         |               |      | CAN1_RX         |              |             |
|    |                         |               |      | TIM1_ETR        |              |             |
|    |                         |               |      | OTG_FS_DP       |              |             |
|    |                         |               |      | EVENTOUT        |              |             |
|----+-------------------------+---------------+------+-----------------+--------------+-------------|
| 19 | MODULE_MICRO_UART1_CTS  | WICED_GPIO_7  | A 12 | USART1_RTS      |              |             |
|    |                         |               |      | CAN1_TX         |              |             |
|    |                         |               |      | TIM1_ETR        |              |             |
|    |                         |               |      | OTG_FS_DP       |              |             |
|    |                         |               |      | EVENTOUT        |              |             |
|----+-------------------------+---------------+------+-----------------+--------------+-------------|
| 23 | MODULE_MICRO_GPIO_0     | WICED_GPIO_8  | B 5  | I2C1_SMBA       |              |             |
|    |                         |               |      | CAN2_RX         |              |             |
|    |                         |               |      | OTG_HS_ULPI_D7  |              |             |
|    |                         |               |      | ETH_PPS_OUT     |              |             |
|    |                         |               |      | TIM3_CH2        |              |             |
|    |                         |               |      | SPI1_MOSI       |              |             |
|    |                         |               |      | SPI3_MOSI       |              |             |
|    |                         |               |      | DCMI_D10        |              |             |
|    |                         |               |      | I2S3_SD         |              |             |
|    |                         |               |      | EVENTOUT        |              |             |
|----+-------------------------+---------------+------+-----------------+--------------+-------------|
| 24 | MODULE_MICRO_GPIO_1     | WICED_GPIO_9  | B 6  | I2C1_SCL        |              |             |
|    |                         |               |      | TIM4_CH1        |              |             |
|    |                         |               |      | CAN2_TX         |              |             |
|    |                         |               |      | DCMI_D5         |              |             |
|    |                         |               |      | USART1_TX       |              |             |
|    |                         |               |      | EVENTOUT        |              |             |
|----+-------------------------+---------------+------+-----------------+--------------+-------------|
| 25 | MODULE_MICRO_GPIO_2     | WICED_GPIO_10 | B 7  | I2C1_SDA        |              |             |
|    |                         |               |      | FSMC_NL         |              |             |
|    |                         |               |      | DCMI_VSYNC      |              |             |
|    |                         |               |      | USART1_RX       |              |             |
|    |                         |               |      | TIM4_CH2        |              |             |
|    |                         |               |      | EVENTOUT        |              |             |
|----+-------------------------+---------------+------+-----------------+--------------+-------------|
| 26 | MODULE_MICRO_GPIO_3     | WICED_GPIO_11 | C 6  | I2S2_MCK        |              |             |
|    |                         |               |      | TIM8_CH1        |              |             |
|    |                         |               |      | SDIO_D6         |              |             |
|    |                         |               |      | USART6_TX       |              |             |
|    |                         |               |      | DCMi_D0         |              |             |
|    |                         |               |      | TIM3_CH1        |              |             |
|    |                         |               |      | EVENTOUT        |              |             |
|----+-------------------------+---------------+------+-----------------+--------------+-------------|
| 27 | MODULE_MICRO_GPIO_4     | WICED_GPIO_12 | C 7  | I2S3_MCK        |              |             |
|    |                         |               |      | TIM8_CH2        |              |             |
|    |                         |               |      | SDIO_D7         |              |             |
|    |                         |               |      | USART6_RX       |              |             |
|    |                         |               |      | DCMi_D1         |              |             |
|    |                         |               |      | TIM3_CH2        |              |             |
|    |                         |               |      | EVENTOUT        |              |             |
|----+-------------------------+---------------+------+-----------------+--------------+-------------|
| 13 | MODULE_MICRO_ADC0       | WICED_GPIO_13 | A 3  | USART2_RX       |  THERMISTOR  | WICED_ADC_1 |
|    |                         |               |      | TIM5_CH4        |              |             |
|    |                         |               |      | TIM9_CH2        |              |             |
|    |                         |               |      | TIM2_CH4        |              |             |
|    |                         |               |      | OTG_HS_ULPI_D0  |              |             |
|    |                         |               |      | ETH_MII_COL     |              |             |
|    |                         |               |      | EVENTOUT        |              |             |
|    |                         |               |      | ADC123_IN3      |              |             |
|----+-------------------------+---------------+------+-----------------+--------------+-------------|
| 12 | MODULE_MICRO_ADC1       | WICED_GPIO_14 | A 4  | SPI1_NSS        |              | WICED_ADC_2 |
|    | MODULE_MICRO_SPI_CS     |               |      | SPI3_NSS        |              |             |
|    |                         |               |      | USART2_CK       |              |             |
|    |                         |               |      | DCMI_HSYNC      |              |             |
|    |                         |               |      | OTG_HS_SOF      |              |             |
|    |                         |               |      | I2S3_WS         |              |             |
|    |                         |               |      | EVENTOUT        |              |             |
|    |                         |               |      | ADC12_IN4       |              |             |
|    |                         |               |      | DAC_OUT1        |              |             |
|----+-------------------------+---------------+------+-----------------+--------------+-------------|
| 11 | MODULE_MICRO_ADC2       | WICED_GPIO_15 | A 5  | SPI1_SCK        |              | WICED_ADC_3 |
|    | MODULE_MICRO_SPI_CLK    |               |      | OTG_HS_ULPI_CK  |              |             |
|    |                         |               |      | TIM2_CH1_ETR    |              |             |
|    |                         |               |      | TIM8_CH1N       |              |             |
|    |                         |               |      | EVENTOUT        |              |             |
|    |                         |               |      | ADC12_IN5       |              |             |
|    |                         |               |      | DAC_OUT2        |              |             |
|----+-------------------------+---------------+------+-----------------+--------------+-------------|
| 10 | MODULE_MICRO_ADC3       | WICED_GPIO_16 | A 6  | SPI1_MISO       |              | WICED_ADC_4 |
|    | MODULE_MICRO_SPI_MISO   |               |      | TIM8_BKIN       |              |             |
|    |                         |               |      | TIM13_CH1       |              |             |
|    |                         |               |      | DCMI_PIXCLK     |              |             |
|    |                         |               |      | TIM3_CH1        |              |             |
|    |                         |               |      | TIM1_BKIN       |              |             |
|    |                         |               |      | EVENTOUT        |              |             |
|    |                         |               |      | ADC12_IN6       |              |             |
|----+-------------------------+---------------+------+-----------------+--------------+-------------|
| 9  | MODULE_MICRO_ADC4       | WICED_GPIO_17 | A 7  | SPI1_MOSI       |              | WICED_ADC_5 |
|    | MODULE_MICRO_SPI_MOSI   |               |      | TIM8_CH1N       |              |             |
|    |                         |               |      | TIM14_CH1       |              |             |
|    |                         |               |      | TIM3_CH2        |              |             |
|    |                         |               |      | ETH_MII_RX_DV   |              |             |
|    |                         |               |      | TIM1_CH1N       |              |             |
|    |                         |               |      | ETH_RMII_CRS_DV |              |             |
|    |                         |               |      | EVENTOUT        |              |             |
|    |                         |               |      | ADC12_IN7       |              |             |
|----+-------------------------+---------------+------+-----------------+--------------+-------------|
| 16 | MODULE_MICRO_WAKEUP     | WICED_GPIO_18 | A 0  | USART2_CTS      |              |             |
|    |                         |               |      | UART4_TX        |              |             |
|    |                         |               |      | ETH_MII_CRS     |              |             |
|    |                         |               |      | TIM2_CH1_ETR    |              |             |
|    |                         |               |      | TIM5_CH1        |              |             |
|    |                         |               |      | TIM8_ETR        |              |             |
|    |                         |               |      | EVENTOUT        |              |             |
|    |                         |               |      | ADC123_IN0      |              |             |
|    |                         |               |      | WKUP            |              |             |
|----+-------------------------+---------------+------+-----------------+--------------+-------------|
| 32 | MODULE_MICRO_GPIO13     | WICED_GPIO_19 | B 13 | SPI2_SCK        |              |             |
|    |                         |               |      | I2S2_CK         |              |             |
|    |                         |               |      | USART3_CTS      |              |             |
|    |                         |               |      | TIM1_CH1N       |              |             |
|    |                         |               |      | CAN2)TX         |              |             |
|    |                         |               |      | OTG_HS_ULPI_D6  |              |             |
|    |                         |               |      | ETH_RMII_TXD1   |              |             |
|    |                         |               |      | ETH_MII_TXD1    |              |             |
|    |                         |               |      | EVENTOUT        |              |             |
|    |                         |               |      | OTG_HS_VBUS     |              |             |
|----+-------------------------+---------------+------+-----------------+--------------+-------------|
| 31 | MODULE_MICRO_GPIO14     | WICED_GPIO_20 | B 14 | SPI2_MISO       |              |             |
|    |                         |               |      | TIM1_CH2N       |              |             |
|    |                         |               |      | TIM12_CH1       |              |             |
|    |                         |               |      | OTG_HS_DM       |              |             |
|    |                         |               |      | USART3_RTS      |              |             |
|    |                         |               |      | TIM8_CH2N       |              |             |
|    |                         |               |      | I2S2ext_SD      |              |             |
|    |                         |               |      | EVENTOUT        |              |             |
|----+-------------------------+---------------+------+-----------------+--------------+-------------|
| 30 | MODULE_MICRO_GPIO15     | WICED_GPIO_21 | B 15 | SPI2_M0Si       |              |             |
|    |                         |               |      | I2S2_SD         |              |             |
|    |                         |               |      | TIM1_CH3N       |              |             |
|    |                         |               |      | TIM8_CH3N       |              |             |
|    |                         |               |      | TIM12_CH2       |              |             |
|    |                         |               |      | OTG_HS_DP       |              |             |
|    |                         |               |      | EVENTOUT        |              |             |
|----+-------------------------+---------------+------+-----------------+--------------+-------------|
Notes
1. The mappings in the table above are defined in <WICED-SDK>/platforms/ISM43341_M4G_L44/platform.c
2. STM32F4xx Datasheet  ->
3. STM32F4xx Ref Manual ->
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
    WICED_GPIO_MAX, /* Denotes the total number of GPIO port aliases. Not a valid GPIO alias */
    WICED_GPIO_32BIT = 0x7FFFFFFF
} wiced_gpio_t;

typedef enum
{
    WICED_SPI_1, 	/* SPI available */
    WICED_SPI_2,
    WICED_SPI_MAX,  /* Denotes the total number of SPI port aliases. Not a valid SPI alias */
    WICED_SPI_32BIT = 0x7FFFFFFF,
} wiced_spi_t;

typedef enum
{
    WICED_I2C_1, 	/* I2C available */
    WICED_I2C_2,
    WICED_I2C_MAX,  /* Denotes the total number of I2C port aliases. Not a valid I2C alias */
    WICED_I2C_32BIT = 0x7FFFFFFF,
} wiced_i2c_t;

typedef enum
{
    WICED_I2S_NONE, /* I2S unavailable */
    WICED_I2S_MAX,  /* Denotes the total number of I2S port aliases. Not a valid I2S alias */
    WICED_I2S_32BIT = 0x7FFFFFFF,
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
    WICED_PWM_MAX,  /* Denotes the total number of PWM port aliases. Not a valid PWM alias */
    WICED_PWM_32BIT = 0x7FFFFFFF,
} wiced_pwm_t;

typedef enum
{
	WICED_ADC_1,
	WICED_ADC_2,
	WICED_ADC_3,
	WICED_ADC_4,
	WICED_ADC_5,
    WICED_ADC_MAX,  /* Denotes the total number of ADC port aliases. Not a valid ADC alias */
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
#define STDIO_UART                       ( WICED_UART_1 )

/* SPI flash is present */
#define WICED_PLATFORM_INCLUDES_SPI_FLASH

/* SPI Flash. Implemented by Inventek Systems ... */
#ifdef WICED_PLATFORM_INCLUDES_SPI_FLASH
#define WICED_SPI_FLASH_CS            	( WICED_GPIO_14 ) 		//ADC1
#define WICED_SPI_FLASH_MOSI          	( WICED_GPIO_17 )		//ADC4
#define WICED_SPI_FLASH_MISO          	( WICED_GPIO_16 )		//ADC3
#define WICED_SPI_FLASH_CLK           	( WICED_GPIO_15 )		//ADC2
#endif

/* Components connected to external I/Os */
#define WICED_LED1                     	( WICED_GPIO_11 )		//GPIO3
#define WICED_LED2                      ( WICED_GPIO_12 )		//GPIO4
#define WICED_LED1_ON_STATE             ( WICED_ACTIVE_HIGH )
#define WICED_LED2_ON_STATE             ( WICED_ACTIVE_HIGH )
#define WICED_BUTTON1                   ( WICED_GPIO_8  )		//GPIO0
#define WICED_BUTTON2                   ( WICED_GPIO_9  )		//GPIO1
#define WICED_THERMISTOR                ( WICED_GPIO_13 )		//ADC0

#define WICED_GPIO_AUTH_RST             ( WICED_GPIO_21 )		//GPIO15
#define WICED_GPIO_AUTH_SCL             ( WICED_GPIO_9  )		//GPIO1
#define WICED_GPIO_AUTH_SDA             ( WICED_GPIO_10 )		//GPIO2

/* Authentication Chip <-> I2C Peripheral */
#define WICED_I2C_AUTH                  ( WICED_I2C_1 )
#define WICED_I2C_AUTH_DMA              ( I2C_DEVICE_USE_DMA )

/* I/O connection <-> Peripheral Connections */
/* #define WICED_LED1_JOINS_PWM         ( WICED_PWM_1 ) */
/* #define WICED_LED2_JOINS_PWM         ( WICED_PWM_2 ) */
/* #define WICED_THERMISTOR_JOINS_ADC   ( WICED_ADC_3 ) */

/* Bootloader OTA/OTA2 LED to flash while "Factory Reset" button held           */
 #define PLATFORM_FACTORY_RESET_LED_GPIO        	( WICED_LED1 )
 #define PLATFORM_FACTORY_RESET_LED_ON_STATE        ( WICED_LED1_ON_STATE )

/* Bootloader OTA/OTA2 "Factory Reset" button */
 #define PLATFORM_FACTORY_RESET_BUTTON_GPIO      	( WICED_BUTTON1 )
 #define PLATFORM_FACTORY_RESET_PRESSED_STATE    	(   0  )
 #define PLATFORM_FACTORY_RESET_CHECK_PERIOD     	(  100 )
 #define PLATFORM_FACTORY_RESET_TIMEOUT          	( 5000 )


#ifdef __cplusplus
} /*extern "C" */
#endif
