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
 * GPIO test Application
 *
 * This program inputs into input pins and reads from test pin with
 * rising edge, falling edge, and both edges to check if pin input/output is same
 *
 * Application Instructions
 *   Connect a PC terminal to the serial port of the WICED Eval board,
 *   then build and download the application as described in the WICED
 *   Quick Start Guide
 *
 *   After download, the app tests the GPIOs.  Make sure you have the correct boards
 *   that can help connect to the pins and read from the pins (master/slave).
 *
 */
#include "wiced.h"
#include "gpio.h"
#include "platform_config.h"

#if defined(WICED_BUTTON1) && defined(WICED_BUTTON2)

/******************************************************
 *                      Macros
 ******************************************************/

#define VERIFY( condition )  do { if ( !(condition) ){ WPRINT_APP_INFO(( "Error - function: %s, line : %u\n", __func__, __LINE__ )); return WICED_ERROR; } } while( 0 )

/******************************************************
 *                    Constants
 ******************************************************/

#define TEST_PIN_1  ( WICED_BUTTON1 )
#define TEST_PIN_2  ( WICED_BUTTON2 )

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

static wiced_result_t test_input_output          ( void );
static wiced_result_t test_interrupt_rising_edge ( void );
static wiced_result_t test_interrupt_falling_edge( void );
static wiced_result_t test_interrupt_both_edges  ( void );
static void           gpio_interrupt_handler     ( void* arg );

/******************************************************
 *               Variable Definitions
 ******************************************************/

static volatile wiced_bool_t interrupt_triggered = WICED_FALSE;

/******************************************************
 *               Function Definitions
 ******************************************************/

/*!
 ******************************************************************************
 * Inputs into input and reads from test pin with rising edge, falling edge, and both edges to check if pin input/output is same
 *
 * @return
 */
void gpio( void )
{
    WPRINT_APP_INFO( ( "GPIO Test Application\n") );

    test_input_output();
    test_interrupt_rising_edge();
    test_interrupt_falling_edge();
    test_interrupt_both_edges();
}

static wiced_result_t test_input_output( void )
{
    /* Init TEST_PIN1 : Input and TEST_PIN2 : Output */
    VERIFY( wiced_gpio_init( TEST_PIN_1, INPUT_HIGH_IMPEDANCE ) == WICED_SUCCESS );
    VERIFY( wiced_gpio_init( TEST_PIN_2, OUTPUT_PUSH_PULL ) == WICED_SUCCESS );

    /* Toggle TEST_PIN2 low and check if TEST_PIN1 reads low */
    VERIFY( wiced_gpio_output_low( TEST_PIN_2 ) == WICED_SUCCESS );
    VERIFY( wiced_gpio_input_get( TEST_PIN_1 )  == WICED_FALSE );

    /* Toggle TEST_PIN2 high and check if TEST_PIN1 reads high */
    VERIFY( wiced_gpio_output_high( TEST_PIN_2 ) == WICED_SUCCESS );
    VERIFY( wiced_gpio_input_get( TEST_PIN_1 )   == WICED_TRUE );

    /* Init TEST_PIN2 : Input and TEST_PIN1 : Output */
    VERIFY( wiced_gpio_init( TEST_PIN_2, INPUT_HIGH_IMPEDANCE ) == WICED_SUCCESS );
    VERIFY( wiced_gpio_init( TEST_PIN_1, OUTPUT_PUSH_PULL ) == WICED_SUCCESS );

    /* Toggle TEST_PIN1 low and check if TEST_PIN2 reads low */
    VERIFY( wiced_gpio_output_low( TEST_PIN_1 ) == WICED_SUCCESS );
    VERIFY( wiced_gpio_input_get( TEST_PIN_2 )  == WICED_FALSE );

    /* Toggle TEST_PIN1 high and check if TEST_PIN2 reads high */
    VERIFY( wiced_gpio_output_high( TEST_PIN_1 ) == WICED_SUCCESS );
    VERIFY( wiced_gpio_input_get( TEST_PIN_2 )   == WICED_TRUE );

    WPRINT_APP_INFO( ("test_input_output succeeds\r\n") );
    return WICED_SUCCESS;
}

static wiced_result_t test_interrupt_rising_edge( void )
{
    /* Init TEST_PIN1 : GPIO input with interrupt and TEST_PIN2 : Output */
    interrupt_triggered = WICED_FALSE;
    VERIFY( wiced_gpio_init( TEST_PIN_2, OUTPUT_PUSH_PULL ) == WICED_SUCCESS );
    VERIFY( wiced_gpio_output_low( TEST_PIN_2 ) == WICED_SUCCESS );
    VERIFY( wiced_gpio_init( TEST_PIN_1, INPUT_HIGH_IMPEDANCE ) == WICED_SUCCESS );
    VERIFY( wiced_gpio_input_irq_enable( TEST_PIN_1, IRQ_TRIGGER_RISING_EDGE, gpio_interrupt_handler, NULL ) == WICED_SUCCESS );

    /* Toggle TEST_PIN2 high and check if the interrupt triggers on TEST_PIN1 */
    VERIFY( wiced_gpio_output_high( TEST_PIN_2 ) == WICED_SUCCESS );
    VERIFY( interrupt_triggered == WICED_TRUE );

    /* Disable interrupt */
    VERIFY( wiced_gpio_input_irq_disable( TEST_PIN_1 ) == WICED_SUCCESS );

    /* Init TEST_PIN2 : GPIO input with interrupt and TEST_PIN1 : Output */
    interrupt_triggered = WICED_FALSE;
    VERIFY( wiced_gpio_init( TEST_PIN_1, OUTPUT_PUSH_PULL ) == WICED_SUCCESS );
    VERIFY( wiced_gpio_output_low( TEST_PIN_1 ) == WICED_SUCCESS );
    VERIFY( wiced_gpio_init( TEST_PIN_2, INPUT_HIGH_IMPEDANCE ) == WICED_SUCCESS );
    VERIFY( wiced_gpio_input_irq_enable( TEST_PIN_2, IRQ_TRIGGER_RISING_EDGE, gpio_interrupt_handler, NULL ) == WICED_SUCCESS );

    /* Toggle TEST_PIN1 high and check if the interrupt triggers on TEST_PIN2 */
    VERIFY( wiced_gpio_output_high( TEST_PIN_1 ) == WICED_SUCCESS );
    VERIFY( interrupt_triggered == WICED_TRUE );

    /* Disable interrupt */
    VERIFY( wiced_gpio_input_irq_disable( TEST_PIN_2 ) == WICED_SUCCESS );

    WPRINT_APP_INFO( ("test_interrupt_rising_edge succeeds\r\n") );
    return WICED_SUCCESS;
}

static wiced_result_t test_interrupt_falling_edge( void )
{
    /* Init TEST_PIN1 : GPIO input with interrupt and TEST_PIN2 : Output */
    interrupt_triggered = WICED_FALSE;
    VERIFY( wiced_gpio_init( TEST_PIN_2, OUTPUT_PUSH_PULL ) == WICED_SUCCESS );
    VERIFY( wiced_gpio_output_high( TEST_PIN_2 ) == WICED_SUCCESS );
    VERIFY( wiced_gpio_init( TEST_PIN_1, INPUT_HIGH_IMPEDANCE ) == WICED_SUCCESS );
    VERIFY( wiced_gpio_input_irq_enable( TEST_PIN_1, IRQ_TRIGGER_FALLING_EDGE, gpio_interrupt_handler, NULL ) == WICED_SUCCESS );

    /* Toggle TEST_PIN2 low and check if the interrupt triggers on TEST_PIN1 */
    VERIFY( wiced_gpio_output_low( TEST_PIN_2 ) == WICED_SUCCESS );
    VERIFY( interrupt_triggered == WICED_TRUE );

    /* Disable interrupt */
    VERIFY( wiced_gpio_input_irq_disable( TEST_PIN_1 ) == WICED_SUCCESS );

    /* Init TEST_PIN2 : GPIO input with interrupt and TEST_PIN1 : Output */
    interrupt_triggered = WICED_FALSE;
    VERIFY( wiced_gpio_init( TEST_PIN_1, OUTPUT_PUSH_PULL ) == WICED_SUCCESS );
    VERIFY( wiced_gpio_output_high( TEST_PIN_1 ) == WICED_SUCCESS );
    VERIFY( wiced_gpio_init( TEST_PIN_2, INPUT_HIGH_IMPEDANCE ) == WICED_SUCCESS );
    VERIFY( wiced_gpio_input_irq_enable( TEST_PIN_2, IRQ_TRIGGER_FALLING_EDGE, gpio_interrupt_handler, NULL ) == WICED_SUCCESS );

    /* Toggle TEST_PIN1 high and check if the interrupt triggers on TEST_PIN2 */
    VERIFY( wiced_gpio_output_low( TEST_PIN_1 ) == WICED_SUCCESS );
    VERIFY( interrupt_triggered == WICED_TRUE );

    /* Disable interrupt */
    VERIFY( wiced_gpio_input_irq_disable( TEST_PIN_2 ) == WICED_SUCCESS );

    WPRINT_APP_INFO( ("test_interrupt_falling_edge succeeds\r\n") );
    return WICED_SUCCESS;
}

static wiced_result_t test_interrupt_both_edges( void )
{
    /* Init TEST_PIN1 : GPIO input with interrupt and TEST_PIN2 : Output */
    interrupt_triggered = WICED_FALSE;
    VERIFY( wiced_gpio_init( TEST_PIN_2, OUTPUT_PUSH_PULL ) == WICED_SUCCESS );
    VERIFY( wiced_gpio_output_low( TEST_PIN_2 ) == WICED_SUCCESS );
    VERIFY( wiced_gpio_init( TEST_PIN_1, INPUT_HIGH_IMPEDANCE ) == WICED_SUCCESS );
    VERIFY( wiced_gpio_input_irq_enable( TEST_PIN_1, IRQ_TRIGGER_BOTH_EDGES, gpio_interrupt_handler, NULL ) == WICED_SUCCESS );

    /* Toggle TEST_PIN2 high and check if the interrupt triggers on TEST_PIN1 */
    VERIFY( wiced_gpio_output_high( TEST_PIN_2 ) == WICED_SUCCESS );
    VERIFY( interrupt_triggered == WICED_TRUE );

    /* Toggle TEST_PIN2 low and check if the interrupt triggers on TEST_PIN1 */
    interrupt_triggered = WICED_FALSE;
    VERIFY( wiced_gpio_output_low( TEST_PIN_2 ) == WICED_SUCCESS );
    VERIFY( interrupt_triggered == WICED_TRUE );

    /* Disable interrupt */
    VERIFY( wiced_gpio_input_irq_disable( TEST_PIN_1 ) == WICED_SUCCESS );

    /* Init TEST_PIN2 : GPIO input with interrupt and TEST_PIN1 : Output */
    interrupt_triggered = WICED_FALSE;
    VERIFY( wiced_gpio_init( TEST_PIN_1, OUTPUT_PUSH_PULL ) == WICED_SUCCESS );
    VERIFY( wiced_gpio_output_low( TEST_PIN_1 ) == WICED_SUCCESS );
    VERIFY( wiced_gpio_init( TEST_PIN_2, INPUT_HIGH_IMPEDANCE ) == WICED_SUCCESS );
    VERIFY( wiced_gpio_input_irq_enable( TEST_PIN_2, IRQ_TRIGGER_BOTH_EDGES, gpio_interrupt_handler, NULL ) == WICED_SUCCESS );

    /* Toggle TEST_PIN1 high and check if the interrupt triggers on TEST_PIN2 */
    VERIFY( wiced_gpio_output_high( TEST_PIN_1 ) == WICED_SUCCESS );
    VERIFY( interrupt_triggered == WICED_TRUE );

    /* Toggle TEST_PIN1 low and check if the interrupt triggers on TEST_PIN2 */
    interrupt_triggered = WICED_FALSE;
    VERIFY( wiced_gpio_output_low( TEST_PIN_1 ) == WICED_SUCCESS );
    VERIFY( interrupt_triggered == WICED_TRUE );

    /* Disable interrupt */
    VERIFY( wiced_gpio_input_irq_disable( TEST_PIN_2 ) == WICED_SUCCESS );

    WPRINT_APP_INFO( ("test_interrupt_both_edges succeeds\r\n") );
    return WICED_SUCCESS;
}

static void gpio_interrupt_handler( void* arg )
{
    interrupt_triggered = WICED_TRUE;
}

#else

void gpio( void )
{
    WPRINT_APP_INFO( ( "\nGPIO test NOT supported\n" ) );
}

#endif
