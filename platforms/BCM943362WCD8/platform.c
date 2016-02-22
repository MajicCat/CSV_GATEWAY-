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
#include "platform_button.h"
#include "gpio_button.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

const platform_wakeup_pin_config_t platform_wakeup_pin_config[] =
{
    [WICED_GPIO_1]  = { WICED_TRUE, 10, IOPORT_SENSE_FALLING },
    [WICED_GPIO_2]  = { WICED_FALSE, 0, 0                    },
    [WICED_GPIO_3]  = { WICED_FALSE, 0, 0                    },
    [WICED_GPIO_4]  = { WICED_TRUE,  9, IOPORT_SENSE_FALLING },
    [WICED_GPIO_5]  = { WICED_TRUE,  7, IOPORT_SENSE_FALLING },
    [WICED_GPIO_6]  = { WICED_TRUE,  8, IOPORT_SENSE_FALLING },
    [WICED_GPIO_7]  = { WICED_FALSE, 0, 0                    },
    [WICED_GPIO_8]  = { WICED_FALSE, 0, 0                    },
    [WICED_GPIO_9]  = { WICED_FALSE, 0, 0                    },
    [WICED_GPIO_10] = { WICED_FALSE, 0, 0                    },
    [WICED_GPIO_11] = { WICED_FALSE, 0, 0                    },
    [WICED_GPIO_12] = { WICED_FALSE, 0, 0                    },
    [WICED_GPIO_13] = { WICED_TRUE,  2, IOPORT_SENSE_FALLING },
    [WICED_GPIO_14] = { WICED_TRUE,  1, IOPORT_SENSE_FALLING },
    [WICED_GPIO_15] = { WICED_FALSE, 0, 0                    },
    [WICED_GPIO_16] = { WICED_TRUE,  3, IOPORT_SENSE_FALLING },
    [WICED_GPIO_17] = { WICED_TRUE,  0, 0                    },
};

const platform_gpio_t platform_gpio_pins[] =
{
    [WICED_GPIO_1]  = { .pin = IOPORT_CREATE_PIN( PIOA, 20 ), .wakeup_pin_config = &platform_wakeup_pin_config[WICED_GPIO_1] },
    [WICED_GPIO_2]  = { .pin = IOPORT_CREATE_PIN( PIOA, 17 ), .wakeup_pin_config = &platform_wakeup_pin_config[WICED_GPIO_2] },
    [WICED_GPIO_3]  = { .pin = IOPORT_CREATE_PIN( PIOA, 18 ), .wakeup_pin_config = &platform_wakeup_pin_config[WICED_GPIO_3] },
    [WICED_GPIO_4]  = { .pin = IOPORT_CREATE_PIN( PIOA, 19 ), .wakeup_pin_config = &platform_wakeup_pin_config[WICED_GPIO_4] },
    [WICED_GPIO_5]  = { .pin = IOPORT_CREATE_PIN( PIOA, 11 ), .wakeup_pin_config = &platform_wakeup_pin_config[WICED_GPIO_5] },
    [WICED_GPIO_6]  = { .pin = IOPORT_CREATE_PIN( PIOA, 14 ), .wakeup_pin_config = &platform_wakeup_pin_config[WICED_GPIO_6] },
    [WICED_GPIO_7]  = { .pin = IOPORT_CREATE_PIN( PIOA, 12 ), .wakeup_pin_config = &platform_wakeup_pin_config[WICED_GPIO_7] },
    [WICED_GPIO_8]  = { .pin = IOPORT_CREATE_PIN( PIOA, 13 ), .wakeup_pin_config = &platform_wakeup_pin_config[WICED_GPIO_8] },
    [WICED_GPIO_9]  = { .pin = IOPORT_CREATE_PIN( PIOA, 22 ), .wakeup_pin_config = &platform_wakeup_pin_config[WICED_GPIO_9] },
    [WICED_GPIO_10] = { .pin = IOPORT_CREATE_PIN( PIOA, 21 ), .wakeup_pin_config = &platform_wakeup_pin_config[WICED_GPIO_10] },
    [WICED_GPIO_11] = { .pin = IOPORT_CREATE_PIN( PIOA, 24 ), .wakeup_pin_config = &platform_wakeup_pin_config[WICED_GPIO_11] },
    [WICED_GPIO_12] = { .pin = IOPORT_CREATE_PIN( PIOA, 25 ), .wakeup_pin_config = &platform_wakeup_pin_config[WICED_GPIO_12] },
    [WICED_GPIO_13] = { .pin = IOPORT_CREATE_PIN( PIOA, 2  ), .wakeup_pin_config = &platform_wakeup_pin_config[WICED_GPIO_13] },
    [WICED_GPIO_14] = { .pin = IOPORT_CREATE_PIN( PIOA, 1  ), .wakeup_pin_config = &platform_wakeup_pin_config[WICED_GPIO_14] },
    [WICED_GPIO_15] = { .pin = IOPORT_CREATE_PIN( PIOA, 3  ), .wakeup_pin_config = &platform_wakeup_pin_config[WICED_GPIO_15] },
    [WICED_GPIO_16] = { .pin = IOPORT_CREATE_PIN( PIOA, 4  ), .wakeup_pin_config = &platform_wakeup_pin_config[WICED_GPIO_16] },
    [WICED_GPIO_17] = { .pin = IOPORT_CREATE_PIN( PIOA, 0  ), .wakeup_pin_config = &platform_wakeup_pin_config[WICED_GPIO_17] }
};

const platform_uart_t const platform_uart_peripherals[] =
{
    [WICED_UART_1] =
    {
        .uart_id       =    USART_1,
        .peripheral       = USART1,
        .peripheral_id    = ID_USART1,
        .tx_pin           = &platform_gpio_pins[WICED_GPIO_9],
        .rx_pin           = &platform_gpio_pins[WICED_GPIO_10],
        .cts_pin          = NULL, /* flow control isn't supported */
        .rts_pin          = NULL, /* flow control isn't supported */
    }
};

platform_uart_driver_t platform_uart_drivers[WICED_UART_MAX];

const platform_pwm_t const platform_pwm_peripherals[] =
{
    [WICED_PWM_1] = {.unimplemented = 0}
};

const platform_spi_t const platform_spi_peripherals[] =
{
    [WICED_SPI_1] =
    {
        .peripheral           = SPI,
        .peripheral_id        = ID_SPI,
        .clk_pin              = &platform_gpio_pins[WICED_GPIO_6],
        .mosi_pin             = &platform_gpio_pins[WICED_GPIO_8],
        .miso_pin             = &platform_gpio_pins[WICED_GPIO_7],
    }
};


const platform_i2c_t const platform_i2c_peripherals[] =
{
    [WICED_I2C_1] =
    {
        .i2c_block_id      = TWI_0,
        .peripheral        = TWI0,
        .peripheral_id     = ID_TWI0,
        .sda_pin           = &platform_gpio_pins[WICED_GPIO_15], /* PA3 */
        .scl_pin           = &platform_gpio_pins[WICED_GPIO_16], /* PA4 */
    }
};


const platform_adc_t const platform_adc_peripherals[] =
{
    [WICED_ADC_1] =
    {
        .peripheral    = ADC,
        .peripheral_id = ID_ADC,
        .adc_pin       = &platform_gpio_pins[WICED_GPIO_2],
        .adc_clock_hz  = 64000000,
        .channel       = ADC_CHANNEL_0,
        .settling_time = ADC_SETTLING_TIME_3,
        .resolution    = ADC_MR_LOWRES_BITS_12,
        .trigger       = ADC_TRIG_SW,
    },
    [WICED_ADC_2] =
    {
        .peripheral    = ADC,
        .peripheral_id = ID_ADC,
        .adc_pin       = &platform_gpio_pins[WICED_GPIO_3],
        .adc_clock_hz  = 64000000,
        .channel       = ADC_CHANNEL_1,
        .settling_time = ADC_SETTLING_TIME_3,
        .resolution    = ADC_MR_LOWRES_BITS_12,
        .trigger       = ADC_TRIG_SW,
    },
    [WICED_ADC_3] =
    {
        .peripheral    = ADC,
        .peripheral_id = ID_ADC,
        .adc_pin       = &platform_gpio_pins[WICED_GPIO_4],
        .adc_clock_hz  = 64000000,
        .channel       = ADC_CHANNEL_2,
        .settling_time = ADC_SETTLING_TIME_3,
        .resolution    = ADC_MR_LOWRES_BITS_12,
        .trigger       = ADC_TRIG_SW,
    },
};

#if defined ( WICED_PLATFORM_INCLUDES_SPI_FLASH )
const wiced_spi_device_t wiced_spi_flash =
{
    .port        = WICED_SPI_1,
    .chip_select = WICED_SPI_FLASH_CS,
    .speed       = 1000000,
    .mode        = ( SPI_CLOCK_RISING_EDGE | SPI_CLOCK_IDLE_HIGH | SPI_NO_DMA | SPI_MSB_FIRST ),
    .bits        = 8
};
#endif /* WICED_PLATFORM_INCLUDES_SPI_FLASH */

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


const platform_gpio_t wifi_control_pins[] =
{
    [WWD_PIN_POWER      ] = { .pin = IOPORT_CREATE_PIN( PIOB, 13 ) },
    [WWD_PIN_RESET      ] = { .pin = IOPORT_CREATE_PIN( PIOB,  3 ) },
    [WWD_PIN_32K_CLK    ] = { .pin = IOPORT_CREATE_PIN( PIOB,  0 ) },
    [WWD_PIN_BOOTSTRAP_0] = { .pin = IOPORT_CREATE_PIN( PIOB,  1 ) },
    [WWD_PIN_BOOTSTRAP_1] = { .pin = IOPORT_CREATE_PIN( PIOB,  2 ) },
};

const platform_wakeup_pin_config_t sdio_oob_irq_wakeup_pin_config =
{
    .is_wakeup_pin     = WICED_TRUE,
    .wakeup_pin_number = 12,
    .trigger           = IOPORT_SENSE_RISING,
};

const platform_gpio_t wifi_sdio_pins[] =
{
    [WWD_PIN_SDIO_OOB_IRQ] = { .pin = IOPORT_CREATE_PIN( PIOB,  2 ), .wakeup_pin_config = &sdio_oob_irq_wakeup_pin_config },
    [WWD_PIN_SDIO_CLK    ] = { .pin = IOPORT_CREATE_PIN( PIOA, 29 ) },
    [WWD_PIN_SDIO_CMD    ] = { .pin = IOPORT_CREATE_PIN( PIOA, 28 ) },
    [WWD_PIN_SDIO_D0     ] = { .pin = IOPORT_CREATE_PIN( PIOA, 30 ) },
    [WWD_PIN_SDIO_D1     ] = { .pin = IOPORT_CREATE_PIN( PIOA, 31 ) },
    [WWD_PIN_SDIO_D2     ] = { .pin = IOPORT_CREATE_PIN( PIOA, 26 ) },
    [WWD_PIN_SDIO_D3     ] = { .pin = IOPORT_CREATE_PIN( PIOA, 27 ) }
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

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

void platform_init_peripheral_irq_priorities( void )
{
    /* Interrupt priority setup. Called by WICED/platform/MCU/SAM4S/platform_init.c */
    NVIC_SetPriority( USART1_IRQn      ,  1 );
    NVIC_SetPriority( TWI0_IRQn        ,  14 );
    NVIC_SetPriority( PIOA_IRQn        ,  14 );
    NVIC_SetPriority( PIOB_IRQn        ,  14 );
    NVIC_SetPriority( PIOC_IRQn        ,  14 );
    NVIC_SetPriority( RTT_IRQn         ,  0 );
    NVIC_SetPriority( HSMCI_IRQn       ,  2 );
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

WWD_RTOS_MAP_ISR( usart1_irq, USART1_irq )

