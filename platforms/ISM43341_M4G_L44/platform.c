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
 * Defines board support package for ISM43341-M4G-EVB board
 */
#include "platform.h"
#include "platform_config.h"
#include "platform_init.h"
#include "platform_isr.h"
#include "platform_peripheral.h"
#include "wwd_platform_common.h"
#include "wwd_rtos_isr.h"
#include "wiced_defaults.h"
#include "wiced_platform.h"
#include "platform_bluetooth.h"
#include "platform_nfc.h"

#include "platform_button.h"
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
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/

/*  ***** Please note the WICED_GPIO_X definitions have change from previous ISM43341_M4G_L44 patch files ***** */

/* GPIO pin table. Used by WICED/platform/MCU/wiced_platform_common.c */
const platform_gpio_t platform_gpio_pins[] =
{									//Signal									   L44 Pin
    [WICED_GPIO_1 ] = { GPIOA, 15 },//TDI 											( 6)
    [WICED_GPIO_2 ] = { GPIOB,  3 },//TDO 											( 7)
    [WICED_GPIO_3 ] = { GPIOB,  4 },//TRSTN 										( 8)
    [WICED_GPIO_4 ] = { GPIOA,  9 },//UART1_TX 										(22)
    [WICED_GPIO_5 ] = { GPIOA, 10 },//UART1_RX 										(21)
    [WICED_GPIO_6 ] = { GPIOA, 11 },//UART1_CTS/USB DM								(19)
    [WICED_GPIO_7 ] = { GPIOA, 12 },//UART1_RTS/USB_DP								(18)
    [WICED_GPIO_8 ] = { GPIOB,  5 },//GPIO0 (EVB-Button SW1) 						(23)
    [WICED_GPIO_9 ] = { GPIOB,  6 },//GPIO1 (EVB-Button SW2) 						(24)
    [WICED_GPIO_10] = { GPIOB,  7 },//GPIO2 										(25)
    [WICED_GPIO_11] = { GPIOC,  6 },//GPIO3 (EVB-LED7, Red) 						(26)
    [WICED_GPIO_12] = { GPIOC,  7 },//GPIO4 (EVB-LED6, Green) 						(27)
    [WICED_GPIO_13] = { GPIOA,  3 },//ADC0/SPI1_CMD_Data Ready  (EVB-Thermistor)	(13)
    [WICED_GPIO_14] = { GPIOA,  4 },//ADC1/SPI1_SSN									(12)
    [WICED_GPIO_15] = { GPIOA,  5 },//ADC2/SPI1_SCK									(11)
    [WICED_GPIO_16] = { GPIOA,  6 },//ADC3/SPI1_MISO								(10)
    [WICED_GPIO_17] = { GPIOA,  7 },//ADC4/SPI1_MOSI								( 9)
    [WICED_GPIO_18] = { GPIOA,  0 },//WAKEUP										(16)
    [WICED_GPIO_19] = { GPIOB, 13 },//GPIO13										(32)
    [WICED_GPIO_20] = { GPIOB, 14 },//GPIO14										(31)
    [WICED_GPIO_21] = { GPIOB, 15 },//GPIO15										(30)
 };

/* ADC peripherals. Used WICED/platform/MCU/wiced_platform_common.c */
const platform_adc_t platform_adc_peripherals[] =
{
    [WICED_ADC_1] = {ADC1, ADC_Channel_3, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[WICED_GPIO_13]}, /* PA3, ADC0 */
    [WICED_ADC_2] = {ADC1, ADC_Channel_4, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[WICED_GPIO_14]}, /* PA4, ADC1 */
    [WICED_ADC_3] = {ADC1, ADC_Channel_5, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[WICED_GPIO_15]}, /* PA5, ADC2 */
    [WICED_ADC_4] = {ADC1, ADC_Channel_6, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[WICED_GPIO_16]}, /* PA6, ADC3 */
    [WICED_ADC_5] = {ADC1, ADC_Channel_7, RCC_APB2Periph_ADC1, 1, &platform_gpio_pins[WICED_GPIO_17]}, /* PA7, ADC4 */
};

/* PWM peripherals. Used by WICED/platform/MCU/wiced_platform_common.c */
const platform_pwm_t platform_pwm_peripherals[] =
{
    [WICED_PWM_1]  = {TIM3, 2, RCC_APB1Periph_TIM3,  GPIO_AF_TIM3,  &platform_gpio_pins[WICED_GPIO_8 ]}, /* PB5,  GPIO0,  TIM3_CH2  */
    [WICED_PWM_2]  = {TIM4, 1, RCC_APB1Periph_TIM4,  GPIO_AF_TIM4,  &platform_gpio_pins[WICED_GPIO_9 ]}, /* PB6,  GPIO1,  TIM4_CH1  */
    [WICED_PWM_3]  = {TIM4, 2, RCC_APB1Periph_TIM4,  GPIO_AF_TIM4,  &platform_gpio_pins[WICED_GPIO_10]}, /* PB7,  GPIO2,  TIM4_CH2  */
    [WICED_PWM_4]  = {TIM4, 3, RCC_APB1Periph_TIM4,  GPIO_AF_TIM4,  &platform_gpio_pins[WICED_GPIO_11]}, /* PB8,  GPIO3,  TIM4_CH3  */
    [WICED_PWM_5]  = {TIM4, 4, RCC_APB1Periph_TIM4,  GPIO_AF_TIM4,  &platform_gpio_pins[WICED_GPIO_12]}, /* PB9,  GPIO4 , TIM4_CH4  */
    [WICED_PWM_6]  = {TIM1, 1, RCC_APB2Periph_TIM1,  GPIO_AF_TIM1,  &platform_gpio_pins[WICED_GPIO_19]}, /* PB13, GPIO13, TIM1_CH1N */
    [WICED_PWM_7]  = {TIM12,1, RCC_APB1Periph_TIM12, GPIO_AF_TIM12, &platform_gpio_pins[WICED_GPIO_20]}, /* PB14, GPIO14, TIM12_CH1 */
    [WICED_PWM_8]  = {TIM12,2, RCC_APB1Periph_TIM12, GPIO_AF_TIM12, &platform_gpio_pins[WICED_GPIO_21]}, /* PB15, GPIO15, TIM12_CH2 */
};

/* UART peripherals and runtime drivers. Used by WICED/platform/MCU/wiced_platform_common.c */
const platform_uart_t platform_uart_peripherals[] =
{
    [WICED_UART_1] =
    {
        .port               = USART1,
        .tx_pin             = &platform_gpio_pins[WICED_GPIO_4],
        .rx_pin             = &platform_gpio_pins[WICED_GPIO_5],
        .cts_pin            = &platform_gpio_pins[WICED_GPIO_6],
        .rts_pin            = &platform_gpio_pins[WICED_GPIO_7],
        .tx_dma_config =
        {
            .controller     = DMA2,
            .stream         = DMA2_Stream7,
            .channel        = DMA_Channel_4,
            .irq_vector     = DMA2_Stream7_IRQn,
            .complete_flags = DMA_HISR_TCIF7,
            .error_flags    = ( DMA_HISR_TEIF7 | DMA_HISR_FEIF7 | DMA_HISR_DMEIF7 ),
        },
        .rx_dma_config =
        {
            .controller     = DMA2,
            .stream         = DMA2_Stream2,
            .channel        = DMA_Channel_4,
            .irq_vector     = DMA2_Stream2_IRQn,
            .complete_flags = DMA_LISR_TCIF2,
            .error_flags    = ( DMA_LISR_TEIF2 | DMA_LISR_FEIF2 | DMA_LISR_DMEIF2 ),
        },
    },
    [WICED_UART_2] =
    {
        .port               = USART6,
        .tx_pin             = &platform_gpio_pins[WICED_GPIO_11],
        .rx_pin             = &platform_gpio_pins[WICED_GPIO_12],
        .cts_pin            = NULL,
        .rts_pin            = NULL,
        .tx_dma_config =
        {
            .controller     = DMA2,
            .stream         = DMA2_Stream6,
            .channel        = DMA_Channel_5,
            .irq_vector     = DMA2_Stream6_IRQn,
            .complete_flags = DMA_HISR_TCIF7,
            .error_flags    = ( DMA_HISR_TEIF7 | DMA_HISR_FEIF7 | DMA_HISR_DMEIF7 ),
        },
        .rx_dma_config =
        {
            .controller     = DMA2,
            .stream         = DMA2_Stream1,
            .channel        = DMA_Channel_5,
            .irq_vector     = DMA2_Stream1_IRQn,
            .complete_flags = DMA_LISR_TCIF2,
            .error_flags    = ( DMA_LISR_TEIF2 | DMA_LISR_FEIF2 | DMA_LISR_DMEIF2 ),
        },
    },
};
platform_uart_driver_t platform_uart_drivers[WICED_UART_MAX];

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

/* Wi-Fi control pins. Used by WICED/platform/MCU/wwd_platform_common.c
 * SDIO: WWD_PIN_BOOTSTRAP[1:0] = b'X0
 * gSPI: WWD_PIN_BOOTSTRAP[1:0] = b'01
 */
const platform_gpio_t wifi_control_pins[] =
{
    /* Power pin unavailable */
    //[WWD_PIN_POWER      ] = { GPIOx,  x },
    [WWD_PIN_RESET      ] = { GPIOD,  15 },
    [WWD_PIN_32K_CLK    ] = { GPIOA,   8 },
    [WWD_PIN_BOOTSTRAP_0] = { GPIOD,   6 },
    [WWD_PIN_BOOTSTRAP_1] = { GPIOB,   1 },
};

/* Wi-Fi SDIO bus pins. Used by WICED/platform/STM32F4xx/WWD/wwd_SDIO.c */
const platform_gpio_t wifi_sdio_pins[] =
{
    [WWD_PIN_SDIO_OOB_IRQ] = { GPIOB, 11 },
    [WWD_PIN_SDIO_CLK    ] = { GPIOC, 12 },
    [WWD_PIN_SDIO_CMD    ] = { GPIOD,  2 },
    [WWD_PIN_SDIO_D0     ] = { GPIOC,  8 },
    [WWD_PIN_SDIO_D1     ] = { GPIOC,  9 },
    [WWD_PIN_SDIO_D2     ] = { GPIOC, 10 },
    [WWD_PIN_SDIO_D3     ] = { GPIOC, 11 },
};

/* ISM433XX platforms, not usable, not connect to radio */
/* Wi-Fi gSPI bus pins. Used by WICED/platform/STM32F4xx/WWD/wwd_SPI.c */
const platform_gpio_t wifi_spi_pins[] =
{
    [WWD_PIN_SPI_IRQ ] = { GPIOC,  9 },
    [WWD_PIN_SPI_CS  ] = { GPIOC, 11 },
    [WWD_PIN_SPI_CLK ] = { GPIOB, 13 },
    [WWD_PIN_SPI_MOSI] = { GPIOB, 15 },
    [WWD_PIN_SPI_MISO] = { GPIOB, 14 },
};

/* Bluetooth control pins. Used by libraries/bluetooth/internal/bus/UART/bt_bus.c */
static const platform_gpio_t internal_bt_control_pins[] =
{
    /* Power pin unavailable */
    //[WICED_BT_PIN_POWER      ] = { GPIOx,  x },
    [WICED_BT_PIN_RESET      ] = { GPIOD,  5 },
    [WICED_BT_PIN_HOST_WAKE  ] = { GPIOD,  6 },
    [WICED_BT_PIN_DEVICE_WAKE] = { GPIOD,  7 }
};
const platform_gpio_t* wiced_bt_control_pins[] =
{
    /* Power pin unavailable */
    //[WICED_BT_PIN_POWER      ] = &internal_bt_control_pins[WICED_BT_PIN_POWER      ],
    [WICED_BT_PIN_RESET      ] = &internal_bt_control_pins[WICED_BT_PIN_RESET      ],
    [WICED_BT_PIN_HOST_WAKE  ] = &internal_bt_control_pins[WICED_BT_PIN_HOST_WAKE  ],
    [WICED_BT_PIN_DEVICE_WAKE] = &internal_bt_control_pins[WICED_BT_PIN_DEVICE_WAKE],
};

/* Bluetooth UART pins. Used by libraries/bluetooth/internal/bus/UART/bt_bus.c */
static const platform_gpio_t internal_bt_uart_pins[] =
{
    [WICED_BT_PIN_UART_TX ] = { GPIOD,  9 },
    [WICED_BT_PIN_UART_RX ] = { GPIOD,  8 },
    [WICED_BT_PIN_UART_CTS] = { GPIOD, 12 },
    [WICED_BT_PIN_UART_RTS] = { GPIOD, 11 },
};
const platform_gpio_t* wiced_bt_uart_pins[] =
{
    [WICED_BT_PIN_UART_TX ] = &internal_bt_uart_pins[WICED_BT_PIN_UART_TX ],
    [WICED_BT_PIN_UART_RX ] = &internal_bt_uart_pins[WICED_BT_PIN_UART_RX ],
    [WICED_BT_PIN_UART_CTS] = &internal_bt_uart_pins[WICED_BT_PIN_UART_CTS],
    [WICED_BT_PIN_UART_RTS] = &internal_bt_uart_pins[WICED_BT_PIN_UART_RTS],
};

/* Bluetooth UART peripheral and runtime driver. Used by libraries/bluetooth/internal/bus/UART/bt_bus.c */
static const platform_uart_t internal_bt_uart_peripheral =
{
    .port               = USART3,
    .tx_pin             = &internal_bt_uart_pins[WICED_BT_PIN_UART_TX ],
    .rx_pin             = &internal_bt_uart_pins[WICED_BT_PIN_UART_RX ],
    .cts_pin            = &internal_bt_uart_pins[WICED_BT_PIN_UART_CTS],
    .rts_pin            = &internal_bt_uart_pins[WICED_BT_PIN_UART_RTS],
    .tx_dma_config =
    {
        .controller     = DMA1,
        .stream         = DMA1_Stream4,
        .channel        = DMA_Channel_7,
        .irq_vector     = DMA1_Stream4_IRQn,
        .complete_flags = DMA_HISR_TCIF4,
        .error_flags    = ( DMA_HISR_TEIF4 | DMA_HISR_FEIF4 | DMA_HISR_DMEIF4 ),
    },
    .rx_dma_config =
    {
        .controller     = DMA1,
        .stream         = DMA1_Stream1,
        .channel        = DMA_Channel_4,
        .irq_vector     = DMA1_Stream1_IRQn,
        .complete_flags = DMA_LISR_TCIF1,
        .error_flags    = ( DMA_LISR_TEIF1 | DMA_LISR_FEIF1 | DMA_LISR_DMEIF1 ),
    },
};
static platform_uart_driver_t internal_bt_uart_driver;
const platform_uart_t*        wiced_bt_uart_peripheral = &internal_bt_uart_peripheral;
platform_uart_driver_t*       wiced_bt_uart_driver     = &internal_bt_uart_driver;

/* Bluetooth UART configuration. Used by libraries/bluetooth/internal/bus/UART/bt_bus.c */
const platform_uart_config_t wiced_bt_uart_config =
{
    .baud_rate    = 115200,
    .data_width   = DATA_WIDTH_8BIT,
    .parity       = NO_PARITY,
    .stop_bits    = STOP_BITS_1,
    .flow_control = FLOW_CONTROL_DISABLED,
};


/*BT chip specific configuration information*/
const platform_bluetooth_config_t wiced_bt_config =
{
    .patchram_download_mode      = PATCHRAM_DOWNLOAD_MODE_MINIDRV_CMD,
    .patchram_download_baud_rate = 115200,
    .featured_baud_rate          = 115200
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

/* NFC control pins. Used by libraries/bluetooth/internal/bus/UART/bt_bus.c */
static const platform_gpio_t internal_nfc_control_pins[] =
{
    /* Reset pin unavailable */
    [WICED_NFC_PIN_POWER       ] = { GPIOD,  1 },
    [WICED_NFC_PIN_WAKE        ] = { GPIOC,  0 },
    [WICED_NFC_PIN_TRANS_SELECT] = { GPIOG,  0 },
    [WICED_NFC_PIN_IRQ_REQ     ] = { GPIOC,  2 }
};
const platform_gpio_t* wiced_nfc_control_pins[] =
{
    /* Reset pin is REG_PU */
    [WICED_NFC_PIN_POWER       ] = &internal_nfc_control_pins[WICED_NFC_PIN_POWER       ],
    [WICED_NFC_PIN_WAKE        ] = &internal_nfc_control_pins[WICED_NFC_PIN_WAKE        ],
    [WICED_NFC_PIN_TRANS_SELECT] = &internal_nfc_control_pins[WICED_NFC_PIN_TRANS_SELECT],
    [WICED_NFC_PIN_IRQ_REQ     ] = &internal_nfc_control_pins[WICED_NFC_PIN_IRQ_REQ     ],
};


/* NFC I2C pins. Used by libraries/bluetooth/internal/bus/i2c/nfc_bus.c */
static const platform_gpio_t internal_nfc_i2c_pins[] =
{
    [WICED_NFC_I2C_SDA ] = { GPIOB,  9 },
    [WICED_NFC_I2C_SCL ] = { GPIOB,  8 },
};
const platform_gpio_t* wiced_nfc_i2c_pins[] =
{
    [WICED_NFC_I2C_SDA ] = &internal_nfc_i2c_pins[WICED_NFC_I2C_SDA ],
    [WICED_NFC_I2C_SCL ] = &internal_nfc_i2c_pins[WICED_NFC_I2C_SCL ],
};
/* I2C peripherals. Used by WICED/platform/MCU/wiced_platform_common.c */
const platform_i2c_t platform_i2c_peripherals[] =
{
    [WICED_I2C_1] =
    {
        .port                    = I2C2,
        .pin_scl                 = &platform_gpio_pins[WICED_GPIO_9 ], //GPIO1
        .pin_sda                 = &platform_gpio_pins[WICED_GPIO_10], //GPIO2
        .peripheral_clock_reg    = RCC_APB1Periph_I2C2,
        .tx_dma                  = DMA1,
        .tx_dma_peripheral_clock = RCC_AHB1Periph_DMA1,
        .tx_dma_stream           = DMA1_Stream7,
        .rx_dma_stream           = DMA1_Stream0,
        .tx_dma_stream_id        = 7,
        .rx_dma_stream_id        = 0,
        .tx_dma_channel          = DMA_Channel_1,
        .rx_dma_channel          = DMA_Channel_1,
        .gpio_af                 = GPIO_AF_I2C2
    },
    [WICED_I2C_2] =
    {
        .port                    = I2C1,
        .pin_scl                 = &internal_nfc_i2c_pins[WICED_NFC_I2C_SCL],
        .pin_sda                 = &internal_nfc_i2c_pins[WICED_NFC_I2C_SDA],
        .peripheral_clock_reg    = RCC_APB1Periph_I2C1,
        .tx_dma                  = DMA1,
        .tx_dma_peripheral_clock = RCC_AHB1Periph_DMA1,
        .tx_dma_stream           = DMA1_Stream7,
        .rx_dma_stream           = DMA1_Stream0,
        .tx_dma_stream_id        = 7,
        .rx_dma_stream_id        = 0,
        .tx_dma_channel          = DMA_Channel_1,
        .rx_dma_channel          = DMA_Channel_1,
        .gpio_af                 = GPIO_AF_I2C1
    }
};


/* SPI flash. Exposed to the applications through include/wiced_platform.h */
#if defined ( WICED_PLATFORM_INCLUDES_SPI_FLASH )
const wiced_spi_device_t wiced_spi_flash =
{
    .port        = WICED_SPI_1,
    .chip_select = WICED_SPI_FLASH_CS,
    .speed       = 5000000,
    .mode        = (SPI_CLOCK_RISING_EDGE | SPI_CLOCK_IDLE_HIGH | SPI_NO_DMA | SPI_MSB_FIRST),
    .bits        = 8
};
#endif

/* SPI peripherals.  Used for Serial FLASH */
const platform_spi_t platform_spi_peripherals[] =
{
    [WICED_SPI_1]  =
    {
        .port                  = SPI1,
        .gpio_af               = GPIO_AF_SPI1,
        .peripheral_clock_reg  = RCC_APB2Periph_SPI1,
        .peripheral_clock_func = RCC_APB2PeriphClockCmd,
        .pin_mosi              = &platform_gpio_pins[WICED_SPI_FLASH_MOSI],
        .pin_miso              = &platform_gpio_pins[WICED_SPI_FLASH_MISO],
        .pin_clock             = &platform_gpio_pins[WICED_SPI_FLASH_CLK],
        .tx_dma =
        {
            .controller        = DMA2,
            .stream            = DMA2_Stream5,
            .channel           = DMA_Channel_3,
            .irq_vector        = DMA2_Stream5_IRQn,
            .complete_flags    = DMA_HISR_TCIF5,
            .error_flags       = ( DMA_HISR_TEIF5 | DMA_HISR_FEIF5 | DMA_HISR_DMEIF5 ),
        },
        .rx_dma =
        {
            .controller        = DMA2,
            .stream            = DMA2_Stream0,
            .channel           = DMA_Channel_3,
            .irq_vector        = DMA2_Stream0_IRQn,
            .complete_flags    = DMA_LISR_TCIF0,
            .error_flags       = ( DMA_LISR_TEIF0 | DMA_LISR_FEIF0 | DMA_LISR_DMEIF0 ),
        },
    },
    [WICED_SPI_2]  =
    {
        .port                  = SPI3,
        .gpio_af               = GPIO_AF_SPI3,
        .peripheral_clock_reg  = RCC_APB1Periph_SPI3,
        .peripheral_clock_func = RCC_APB1PeriphClockCmd,
        .pin_mosi              = &platform_gpio_pins[WICED_GPIO_8],	//GPIO0
        .pin_miso              = &platform_gpio_pins[WICED_GPIO_3], //TRSTN
        .pin_clock             = &platform_gpio_pins[WICED_GPIO_2], //TDO
        .tx_dma =
        {
            .controller        = DMA1,
            .stream            = DMA1_Stream5,
            .channel           = DMA_Channel_0,
            .irq_vector        = DMA1_Stream5_IRQn,
            .complete_flags    = DMA_HISR_TCIF5,
            .error_flags       = ( DMA_HISR_TEIF5 | DMA_HISR_FEIF5 | DMA_HISR_DMEIF5 ),
        },
        .rx_dma =
        {
            .controller        = DMA1,
            .stream            = DMA1_Stream0,
            .channel           = DMA_Channel_0,
            .irq_vector        = DMA1_Stream0_IRQn,
            .complete_flags    = DMA_LISR_TCIF0,
            .error_flags       = ( DMA_LISR_TEIF0 | DMA_LISR_FEIF0 | DMA_LISR_DMEIF0 ),
        },
    }

};


/******************************************************
 *               Function Definitions
 ******************************************************/

void platform_init_peripheral_irq_priorities( void )
{
    /* Interrupt priority setup. Called by WICED/platform/MCU/STM32F4xx/platform_init.c */
    NVIC_SetPriority( RTC_WKUP_IRQn    ,  1 ); /* RTC Wake-up event   */
    NVIC_SetPriority( SDIO_IRQn        ,  2 ); /* WLAN SDIO           */
    NVIC_SetPriority( DMA2_Stream3_IRQn,  3 ); /* WLAN SDIO DMA       */
    NVIC_SetPriority( DMA1_Stream3_IRQn,  3 ); /* WLAN gSPI DMA       */
    NVIC_SetPriority( USART1_IRQn      ,  6 ); /* WICED_UART_1        */
    NVIC_SetPriority( USART3_IRQn      ,  6 ); /* Bluetooth UART      */
    NVIC_SetPriority( DMA2_Stream7_IRQn,  7 ); /* WICED_UART_1 TX DMA */
    NVIC_SetPriority( DMA2_Stream2_IRQn,  7 ); /* WICED_UART_1 RX DMA */
    NVIC_SetPriority( DMA1_Stream4_IRQn,  7 ); /* Bluetooth TX DMA    */
    NVIC_SetPriority( DMA1_Stream1_IRQn,  7 ); /* Bluetooth RX DMA    */
    NVIC_SetPriority( DMA1_Stream5_IRQn,  7 ); /* WICED_SPI_2_TX      */
    NVIC_SetPriority( DMA1_Stream0_IRQn,  7 ); /* WICED_SPI_2_RX      */
    NVIC_SetPriority( DMA2_Stream6_IRQn,  7 ); /* WICED_UART_2 TX DMA */
    NVIC_SetPriority( DMA2_Stream6_IRQn,  7 ); /* WICED_UART_2 RX DMA */
    NVIC_SetPriority( EXTI0_IRQn       , 14 ); /* GPIO                */
    NVIC_SetPriority( EXTI1_IRQn       , 14 ); /* GPIO                */
    NVIC_SetPriority( EXTI2_IRQn       , 14 ); /* GPIO                */
    NVIC_SetPriority( EXTI3_IRQn       , 14 ); /* GPIO                */
    NVIC_SetPriority( EXTI4_IRQn       , 14 ); /* GPIO                */
    NVIC_SetPriority( EXTI9_5_IRQn     , 14 ); /* GPIO                */
    NVIC_SetPriority( EXTI15_10_IRQn   , 14 ); /* GPIO                */
}

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

uint32_t  platform_get_factory_reset_button_time ( uint32_t max_time )
{
    uint32_t button_press_timer = 0;
    int led_state = 0;

    /* Initialise input */
     platform_gpio_init( &platform_gpio_pins[ PLATFORM_FACTORY_RESET_BUTTON_GPIO ], INPUT_PULL_UP );

     while ( (PLATFORM_FACTORY_RESET_PRESSED_STATE == platform_gpio_input_get(&platform_gpio_pins[ PLATFORM_FACTORY_RESET_BUTTON_GPIO ])) )
    {
         /* How long is the "Factory Reset" button being pressed. */
         host_rtos_delay_milliseconds( PLATFORM_FACTORY_RESET_CHECK_PERIOD );

         /* Toggle LED every PLATFORM_FACTORY_RESET_CHECK_PERIOD  */
        if ( led_state == 0 )
        {
            platform_gpio_output_high( &platform_gpio_pins[ PLATFORM_FACTORY_RESET_LED_GPIO ] );
            led_state = 1;
        }
        else
        {
            platform_gpio_output_low( &platform_gpio_pins[ PLATFORM_FACTORY_RESET_LED_GPIO ] );
            led_state = 0;
        }

        button_press_timer += PLATFORM_FACTORY_RESET_CHECK_PERIOD;
        if ((max_time > 0) && (button_press_timer >= max_time))
        {
            break;
        }
    }

     /* turn off the LED */
     if (PLATFORM_FACTORY_RESET_LED_ON_STATE == WICED_ACTIVE_HIGH)
     {
         platform_gpio_output_low( &platform_gpio_pins[ PLATFORM_FACTORY_RESET_LED_GPIO ] );
     }
     else
     {
         platform_gpio_output_high( &platform_gpio_pins[ PLATFORM_FACTORY_RESET_LED_GPIO ] );
     }

    return button_press_timer;
}

/******************************************************
 *           Interrupt Handler Definitions
 ******************************************************/

WWD_RTOS_DEFINE_ISR( usart1_irq )
{
    platform_uart_irq( &platform_uart_drivers[WICED_UART_1] );
}

WWD_RTOS_DEFINE_ISR( usart1_tx_dma_irq )
{
    platform_uart_tx_dma_irq( &platform_uart_drivers[WICED_UART_1] );
}

WWD_RTOS_DEFINE_ISR( usart1_rx_dma_irq )
{
    platform_uart_rx_dma_irq( &platform_uart_drivers[WICED_UART_1] );
}

WWD_RTOS_DEFINE_ISR( bt_uart_irq )
{
    platform_uart_irq( wiced_bt_uart_driver );
}

WWD_RTOS_DEFINE_ISR( bt_uart_tx_dma_irq )
{
    platform_uart_tx_dma_irq( wiced_bt_uart_driver );
}

WWD_RTOS_DEFINE_ISR( bt_uart_rx_dma_irq )
{
    platform_uart_rx_dma_irq( wiced_bt_uart_driver );
}

/******************************************************
 *            Interrupt Handlers Mapping
 ******************************************************/

/* These DMA assignments can be found STM32F4xx datasheet DMA section */
WWD_RTOS_MAP_ISR( usart1_irq         , USART1_irq       )
WWD_RTOS_MAP_ISR( usart1_tx_dma_irq  , DMA2_Stream7_irq )
WWD_RTOS_MAP_ISR( usart1_rx_dma_irq  , DMA2_Stream2_irq )
WWD_RTOS_MAP_ISR( bt_uart_irq        , USART3_irq       )
WWD_RTOS_MAP_ISR( bt_uart_tx_dma_irq , DMA1_Stream4_irq )
WWD_RTOS_MAP_ISR( bt_uart_rx_dma_irq , DMA1_Stream1_irq )

