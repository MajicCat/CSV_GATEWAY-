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
 * PWM test application
 *
 * Functionality demonstrated with the output pin of a
 * PWM channel directly connected to a GPIO input pin.
 *
 * This app is configured for BCM943909WCD1_3 board.
 * It can be made to work on other 4390x boards by
 * appropriately configuring the PWM and GPIO pins.
 *
 * PWM and GPIO configuration for BCM943909WCD1_3 board.
 * PWM output channel -> WICED_PWM_1
 * GPIO Input pin -> WICED_GPIO_2
 *
 * Pin mappings from BCM943909WCD1_3 board platform file.
 * WICED_PWM_1 -> PIN_GPIO_0 -> J18 GPIO Header pin 1
 * WICED_GPIO_2 -> PIN_GPIO_1 -> J18 GPIO Header pin 3
 */

#include "wiced.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define PWM_TEST_OUTPUT_CHANNEL      WICED_PWM_1
#define PWM_TEST_INPUT_GPIO          WICED_GPIO_2

#define PWM_TEST_FREQ_HZ_INIT        1000
#define PWM_TEST_FREQ_HZ_STEP        500
#define PWM_TEST_FREQ_HZ_MAX         3000
#define PWM_TEST_DUTY_CYCLE_INIT     25
#define PWM_TEST_DUTY_CYCLE_STEP     25
#define PWM_TEST_DUTY_CYCLE_MAX      100

#define PWM_TEST_SAMPLE_COUNT        100
#define PWM_TEST_SAMPLE_TIME_MS      ( ( ( 1000 / PWM_TEST_FREQ_HZ_INIT ) + 1 ) * PWM_TEST_SAMPLE_COUNT * 10 )
#define PWM_TEST_ERROR_THRESHOLD     5

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

static void pwm_gpio_rising_edge( void* arg );
static void pwm_gpio_falling_edge( void* arg );

/******************************************************
 *               Variable Definitions
 ******************************************************/

static uint32_t pwm_pulse_width_samples[PWM_TEST_SAMPLE_COUNT];
static uint32_t pwm_pulse_width_samples_index;

static uint32_t pwm_total_period_samples[PWM_TEST_SAMPLE_COUNT];
static uint32_t pwm_total_period_samples_index;

/******************************************************
 *               Function Definitions
 ******************************************************/

static void pwm_gpio_falling_edge( void* arg )
{
    uint32_t pulse_width;

    /* Read the time stamp counter to measure pulse width */
    pulse_width = platform_tick_get_time( PLATFORM_TICK_GET_FAST_TIME_STAMP );

    /* Add to moving average array of pulse width samples */
    pwm_pulse_width_samples[pwm_pulse_width_samples_index] = pulse_width;
    pwm_pulse_width_samples_index = ( pwm_pulse_width_samples_index + 1 ) % PWM_TEST_SAMPLE_COUNT;

    /* Switch PWM GPIO IRQ trigger to rising edge */
    wiced_gpio_input_irq_enable( PWM_TEST_INPUT_GPIO, IRQ_TRIGGER_RISING_EDGE, pwm_gpio_rising_edge, NULL );
}

static void pwm_gpio_rising_edge( void* arg )
{
    uint32_t total_period;

    /* Read the time stamp counter to measure total period */
    total_period = platform_tick_get_time( PLATFORM_TICK_GET_AND_RESET_FAST_TIME_STAMP );

    /* Add to moving average array of total period samples */
    pwm_total_period_samples[pwm_total_period_samples_index] = total_period;
    pwm_total_period_samples_index = ( pwm_total_period_samples_index + 1 ) % PWM_TEST_SAMPLE_COUNT;

    /* Switch PWM GPIO IRQ trigger to falling edge */
    wiced_gpio_input_irq_enable( PWM_TEST_INPUT_GPIO, IRQ_TRIGGER_FALLING_EDGE, pwm_gpio_falling_edge, NULL );
}

wiced_result_t pwm_test( void )
{
#if PLATFORM_NO_PWM
    WPRINT_APP_INFO ( ( "PWM is not supported on this platform\r\n" ) );
    return WICED_ERROR;
#else
    int ret;
    wiced_result_t pwm_test_result;
    uint32_t target_frequency;        /* Target Frequency in Hz */
    uint32_t target_duty_cycle;       /* Target Duty Cycle in % */
    uint32_t measured_frequency;      /* Measured Frequency in Hz */
    uint32_t measured_duty_cycle;     /* Measured Duty Cycle in % */
    uint64_t average_total_period;
    uint64_t average_pulse_width;
    uint32_t frequency_error;
    uint32_t duty_cycle_error;

    WPRINT_APP_INFO ( ( "Starting PWM Test on Channel %d ...\r\n", PWM_TEST_OUTPUT_CHANNEL ) );

    if ( ( ret = wiced_gpio_init( PWM_TEST_INPUT_GPIO, INPUT_HIGH_IMPEDANCE ) ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO ( ( "Input GPIO Initialization Failed (%d)\r\n", ret ) );
        return WICED_ERROR;
    }

    pwm_test_result = WICED_SUCCESS;

    for ( target_frequency = PWM_TEST_FREQ_HZ_INIT ; target_frequency < PWM_TEST_FREQ_HZ_MAX ; target_frequency += PWM_TEST_FREQ_HZ_STEP )
    {
        for ( target_duty_cycle = PWM_TEST_DUTY_CYCLE_INIT ; target_duty_cycle < PWM_TEST_DUTY_CYCLE_MAX ; target_duty_cycle += PWM_TEST_DUTY_CYCLE_STEP )
        {
            measured_frequency = 0;
            measured_duty_cycle = 0;
            average_total_period = 0;
            average_pulse_width = 0;
            frequency_error = 0;
            duty_cycle_error = 0;
            pwm_pulse_width_samples_index = 0;
            pwm_total_period_samples_index = 0;

            memset( pwm_pulse_width_samples, 0, PWM_TEST_SAMPLE_COUNT );
            memset( pwm_total_period_samples, 0, PWM_TEST_SAMPLE_COUNT );

            WPRINT_APP_INFO ( ( "Target Frequency = %luHZ, Target Duty Cycle = %lu%% ... ", target_frequency, target_duty_cycle ) );

            if ( ( ret = wiced_pwm_init( PWM_TEST_OUTPUT_CHANNEL, target_frequency, target_duty_cycle ) ) != WICED_SUCCESS )
            {
                pwm_test_result = WICED_ERROR;
                WPRINT_APP_INFO ( ( "PWM Initialization Failed (%d)\r\n", ret ) );
                continue;
            }

            wiced_gpio_input_irq_enable( PWM_TEST_INPUT_GPIO, IRQ_TRIGGER_RISING_EDGE, pwm_gpio_rising_edge, NULL );

            wiced_pwm_start( PWM_TEST_OUTPUT_CHANNEL );

            wiced_rtos_delay_milliseconds( ( PWM_TEST_SAMPLE_TIME_MS ) );

            wiced_pwm_stop( PWM_TEST_OUTPUT_CHANNEL );

            wiced_gpio_input_irq_disable( PWM_TEST_INPUT_GPIO );

            for ( int i = 0 ; i < PWM_TEST_SAMPLE_COUNT ; i++ )
            {
                average_total_period += ( (uint64_t)pwm_total_period_samples[i] );
                average_pulse_width += ( (uint64_t)pwm_pulse_width_samples[i] );
            }

            average_total_period /= PWM_TEST_SAMPLE_COUNT;
            average_pulse_width /= PWM_TEST_SAMPLE_COUNT;

            if ( average_total_period > 0 )
            {
                measured_frequency = ( platform_reference_clock_get_freq( PLATFORM_REFERENCE_CLOCK_CPU ) / ( (uint32_t)average_total_period ) );
                measured_duty_cycle = (uint32_t)( ( average_pulse_width * 100 ) / average_total_period );
            }

            frequency_error = ( ( abs( target_frequency - measured_frequency ) * 100 ) / target_frequency );
            duty_cycle_error = ( ( abs( target_duty_cycle - measured_duty_cycle ) * 100 ) / target_duty_cycle );

            if ( ( frequency_error <= PWM_TEST_ERROR_THRESHOLD ) && ( duty_cycle_error <= PWM_TEST_ERROR_THRESHOLD ) )
            {
                WPRINT_APP_INFO ( ( "Passed (freq error %u%% duty cycle error %u%%)\r\n", (unsigned)frequency_error, (unsigned)duty_cycle_error ) );
            }
            else
            {
                pwm_test_result = WICED_ERROR;
                WPRINT_APP_INFO ( ( "Failed (freq error %u%% duty cycle error %u%%)\r\n", (unsigned)frequency_error, (unsigned)duty_cycle_error ) );
            }
        }
    }

    if ( pwm_test_result == WICED_SUCCESS )
    {
        WPRINT_APP_INFO ( ( "PWM Test Passed\r\n" ) );
    }
    else
    {
        WPRINT_APP_INFO ( ( "PWM Test Failed\r\n" ) );
    }

    return pwm_test_result;
#endif /* PLATFORM_NO_PWM */
}
