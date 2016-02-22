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
#include "wiced_rtos.h" /* for wiced_mutex_t */
#include "platform_init.h"
#include "platform_peripheral.h"
#include "platform_config.h"
#include "platform_audio.h"
#include "ak4961.h"
#include "platform_external_memory.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

enum ak4961_device_id
{
    AK4961_DEVICE_ID_0          = 0,

    /* Not a device id! */
    AK4961_DEVICE_ID_MAX,
};

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

static ak4961_device_runtime_data_t ak4961_device_runtime_data[AK4961_DEVICE_ID_MAX];

wiced_i2c_device_t ak4961_control_port =
{
    .port               = WICED_I2C_1,
    .address            = 0x12,
    .address_width      = I2C_ADDRESS_WIDTH_7BIT,
    .speed_mode         = I2C_HIGH_SPEED_MODE,
};

ak4961_device_cmn_data_t ak4961 =
{
    .rtd                = &ak4961_device_runtime_data[AK4961_DEVICE_ID_0],
    .i2c_data           = &ak4961_control_port,
    .ck                 = ak4961_pll_slave,
    .pdn                = WICED_GPIO_AKM_PDN,
    .switcher_3v3_ps_enable = WICED_GPIO_AKM_SWITCHER_3V3_PS_ENABLE,
    .switcher_2v_enable = WICED_GPIO_AKM_SWITCHER_2V_ENABLE,
    .ldo_1v8_enable     = WICED_GPIO_AKM_LDO_1V8_ENABLE,
};

wiced_audio_device_interface_t ak4961_dac1_hp_device_interface = AK4961_DAC_AUDIO_DEVICE_INTERFACE_INITIALIZER();

const ak4961_device_route_t ak4961_dac1_hp_device_route = AK4961_DAC1_HP_INITIALIZER(&ak4961_dac1_hp_device_interface);

/* SDTI1A -> DAC1 -> HP */
const ak4961_dac_route_data_t ak4961_dac1_route_data =
{
    .base.id            = AK4961_ROUTE_ID_0,
    .base.device_type   = AK4961_DEVICE_TYPE_PLAYBACK,
    .base.device_route  = &ak4961_dac1_hp_device_route,
    .source_port        = AK4961_SOURCE_PORT_SDTI1A,
    .sync_domain_select = AK4961_SYNC_DOMAIN_SELECT_1,
    .output_left_select = AK4961_DAC_OUTPUT_SELECT_LCH,
    .output_right_select= AK4961_DAC_OUTPUT_SELECT_RCH,
    .amp_gain_default   = AK4961_DAC_HP_AMP_GAIN_0DB_DEFAULT,
    .amp_gain_current   = AK4961_DAC_HP_AMP_GAIN_0DB_DEFAULT,
    .amp_gain_mute      = AK4961_DAC_HP_AMP_GAIN_MUTE,
    .digital_volume     = AK4961_DAC_DIGITAL_VOLUME_0DB_DEFAULT,
};

/* AIN1P <-> MPWR1A */
static const ak4961_adc_analog_input_t ak4961_adc1_analog_input_left =
{
    .adc_input_select   = AK4961_ADC_INPUT_SELECT_AIN1,
    .power_supply       = AK4961_MIC_POWER_SUPPLY_MPWR1A,
    .output_voltage     = AK4961_MIC_POWER_OUTPUT_VOLTAGE_DEFAULT,
    .amp_gain           = AK4961_ADC_MIC_AMP_GAIN_0DB_DEFAULT,
};

#ifdef NOTYET
/* AIN2P <-> MPWR1B */
static const ak4961_adc_analog_input_t ak4961_adc1_analog_input_right =
{
    .adc_input_select   = AK4961_ADC_INPUT_SELECT_AIN2,
    .power_supply       = AK4961_MIC_POWER_SUPPLY_MPWR1B,
    .output_voltage     = AK4961_MIC_POWER_OUTPUT_VOLTAGE_DEFAULT,
    .amp_gain           = AK4961_ADC_MIC_AMP_GAIN_0DB_DEFAULT,
};
#endif

wiced_audio_device_interface_t ak4961_adc1_mic_device_interface = AK4961_ADC_AUDIO_DEVICE_INTERFACE_INITIALIZER();

const ak4961_device_route_t ak4961_adc1_mic_device_route = AK4961_ADC1_MIC_INITIALIZER(&ak4961_adc1_mic_device_interface);

/* AIN1 -> ADC1 -> SDTO1A */
const ak4961_adc_route_data_t ak4961_adc1_route_data =
{
    .base.id            = AK4961_ROUTE_ID_1,
    .base.device_type   = AK4961_DEVICE_TYPE_CAPTURE,
    .base.device_route  = &ak4961_adc1_mic_device_route,
    .sink_port          = AK4961_SINK_PORT_SDTO1A,
    .sync_domain_select = AK4961_SYNC_DOMAIN_SELECT_1,
    .type.analog        = {
        .input_left     = &ak4961_adc1_analog_input_left,
        .input_right    = NULL,
    },
};

wiced_audio_device_interface_t ak4961_adc1_dmic1_device_interface = AK4961_ADC_AUDIO_DEVICE_INTERFACE_INITIALIZER();

const ak4961_device_route_t ak4961_adc1_dmic1_device_route = AK4961_ADC1_DMIC1_INITIALIZER(&ak4961_adc1_dmic1_device_interface);

/* DMIC1 -> ADC1 -> SDTO1A */
const ak4961_adc_route_data_t ak4961_adc1_dmic1_route_data =
{
    .base.id            = AK4961_ROUTE_ID_2,
    .base.device_type   = AK4961_DEVICE_TYPE_CAPTURE,
    .base.device_route  = &ak4961_adc1_dmic1_device_route,
    .sink_port          = AK4961_SINK_PORT_SDTO1A,
    .sync_domain_select = AK4961_SYNC_DOMAIN_SELECT_1,
    .type.digital       = {
        .lch_enabled    = 1,
        .rch_enabled    = 1,
        .polarity       = AK4961_DMIC_POLARITY_DEFAULT,
    },
};

ak4961_device_data_t ak4961_dac =
{
     .route             = &ak4961_dac1_route_data,
     .cmn               = &ak4961,
     .data_port         = WICED_I2S_1,
};

ak4961_device_data_t ak4961_adc =
{
     .route             = &ak4961_adc1_route_data,
     .cmn               = &ak4961,
     .data_port         = WICED_I2S_2,
};

ak4961_device_data_t ak4961_adc_dmic1 =
{
    .route             = &ak4961_adc1_dmic1_route_data,
    .cmn               = &ak4961,
    .data_port         = WICED_I2S_3,
};

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/

/* platform audio device defines */
#define AK4961_ADC_DESCRIPTION              "4 conductor 3.5mm @ J3"
#define AK4961_DAC_DESCRIPTION              "4 conductor 3.5mm @ J3"
#define AK4961_ADC_DIGITAL_MIC_DESCRIPTION  "digital mic @ MIC 1"

/* defined here, specific to this platform, for platform_audio_device_info.c */
const platform_audio_device_info_t  platform_audio_input_devices[ PLATFORM_AUDIO_NUM_INPUTS ]  =
{
    AUDIO_DEVICE_ID_AK4961_ADC_LINE_INFO,
    AUDIO_DEVICE_ID_AK4961_ADC_DIGITAL_MIC_INFO
};
const platform_audio_device_info_t  platform_audio_output_devices[ PLATFORM_AUDIO_NUM_OUTPUTS ] =
{
    AUDIO_DEVICE_ID_AK4961_DAC_LINE_INFO
};

/******************************************************
 *               Function Definitions
 ******************************************************/

wiced_result_t platform_init_audio( void )
{
    /* Register audio device */
    ak4961_device_register( &ak4961_dac, AUDIO_DEVICE_ID_AK4961_DAC_LINE );

    ak4961_device_register( &ak4961_adc, AUDIO_DEVICE_ID_AK4961_ADC_LINE );

    ak4961_device_register( &ak4961_adc_dmic1, AUDIO_DEVICE_ID_AK4961_ADC_DIGITAL_MIC );

    return WICED_SUCCESS;
}

wiced_result_t platform_deinit_audio( void )
{
    return WICED_UNSUPPORTED;
}

wiced_result_t ak4961_platform_configure( ak4961_device_data_t* device_data, uint32_t mclk, uint32_t fs, uint8_t width )
{
    UNUSED_PARAMETER( device_data );
    UNUSED_PARAMETER( mclk );
    UNUSED_PARAMETER( device_data );
    UNUSED_PARAMETER( width );

    return WICED_SUCCESS;
}
