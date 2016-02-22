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
 * I2C test Application
 *
 * Inits and tests i2c with standard, low, and high speeds and
 * reading/writing 04H and 32H sequences
 *
 * Application Instructions
 *   Connect a PC terminal to the serial port of the WICED Eval board,
 *   then build and download the application as described in the WICED
 *   Quick Start Guide
 *
 *   After download, the i2c function calls i2c test on standard speed,
 *   which inits and runs throughthe 04H and 32H sequences by reading the registers.
 *   Then, it runs through the test on low and high speed.
 *
 */
#include "i2c.h"
#include "wiced.h"
#include "platform_map.h"
#include "platform_config.h"

#if defined(WICED_GPIO_AKM_PDN)

#define DONT_PROBE 0

/******************************************************
 *                      Macros
 ******************************************************/

#define I2C_XFER_RETRY_COUNT                (3)
#define I2C_DONT_USE_DMA                    WICED_TRUE

/******************************************************
 *                    Constants
 ******************************************************/

#define AK4954_04H_DEFAULT_REG_VAL  0x34
#define AK4954_05H_DEFAULT_REG_VAL  0x22
#define AK4954_06H_DEFAULT_REG_VAL  0x09
#define AK4954_07H_DEFAULT_REG_VAL  0x14
#define AK4954_08H_DEFAULT_REG_VAL  0x00
#define AK4954_09H_DEFAULT_REG_VAL  0x3A
#define AK4954_0AH_DEFAULT_REG_VAL  0x40
#define AK4954_0BH_DEFAULT_REG_VAL  0x00

const uint8_t AK4954_04H_8_BYTES_VAL[8] =
{
    AK4954_04H_DEFAULT_REG_VAL,
    AK4954_05H_DEFAULT_REG_VAL,
    AK4954_06H_DEFAULT_REG_VAL,
    AK4954_07H_DEFAULT_REG_VAL,
    AK4954_08H_DEFAULT_REG_VAL,
    AK4954_09H_DEFAULT_REG_VAL,
    AK4954_0AH_DEFAULT_REG_VAL,
    AK4954_0BH_DEFAULT_REG_VAL,

};

const static uint8_t AK4954_ZERO_8_BYTES[8];

#define PDN_PIN         WICED_GPIO_AKM_PDN

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct eight_bytes
{
    uint8_t         bytes[8];
}
eight_bytes_t;

typedef struct eight_byte_val
{
    uint8_t         reg;
    eight_bytes_t   eight;
}
eight_byte_val_t;

/******************************************************
 *               Variable Definitions
 ******************************************************/

wiced_i2c_device_t ak4954_control_port_i2c_standard =
{
    .port          = WICED_I2C_1,
    .address       = 0x12,
    .address_width = I2C_ADDRESS_WIDTH_7BIT,
    .speed_mode    = I2C_STANDARD_SPEED_MODE,
};

wiced_i2c_device_t ak4954_control_port_i2c_low =
{
    .port          = WICED_I2C_1,
    .address       = 0x12,
    .address_width = I2C_ADDRESS_WIDTH_7BIT,
    .speed_mode    = I2C_LOW_SPEED_MODE,
};
wiced_i2c_device_t ak4954_control_port_i2c_high =
{
    .port          = WICED_I2C_1,
    .address       = 0x12,
    .address_width = I2C_ADDRESS_WIDTH_7BIT,
    .speed_mode    = I2C_HIGH_SPEED_MODE,
};

wiced_i2c_device_t ak4954_control_port_i2c =
{
    .port          = WICED_I2C_1,
    .address       = 0x12,
    .address_width = I2C_ADDRESS_WIDTH_7BIT,
    .speed_mode    = I2C_STANDARD_SPEED_MODE,
};

eight_bytes_t EIGHT_BYTE_PATTERN = {{0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8}};

wiced_result_t i2c_reg_write(wiced_i2c_device_t *device, const uint8_t bytes[], uint16_t byte_count)
{
    wiced_result_t result;
    wiced_i2c_message_t msg[1];

    result = wiced_i2c_init_tx_message(msg, (uint8_t *)bytes, byte_count, I2C_XFER_RETRY_COUNT, I2C_DONT_USE_DMA);
    if (result == WICED_SUCCESS)
    {
        result = wiced_i2c_transfer(device, msg, 1);
    }
    return result;
}


/* AK4954 Random Address Read. */
wiced_result_t i2c_reg_read(wiced_i2c_device_t *device, uint8_t reg, uint8_t bytes[], uint16_t byte_count)
{
    wiced_result_t result;
    wiced_i2c_message_t msg[1];

    result = WICED_SUCCESS;

    /* GSIO doesn't support combined I2C messages, so capitalize on the AKM's
     * register counter being reset with a single write.
     */

    /* Reset AKM's register counter. */
    if (result == WICED_SUCCESS)
    {
        result = i2c_reg_write(device, &reg, 1);
    }

    /* Initialize RX message. */
    if (result == WICED_SUCCESS)
    {
        result = wiced_i2c_init_rx_message(msg, bytes, byte_count, I2C_XFER_RETRY_COUNT, I2C_DONT_USE_DMA);
    }

    /* Transfer. */
    if (result == WICED_SUCCESS)
    {
        result = wiced_i2c_transfer(device, msg, 1);
    }

    return result;
}

wiced_result_t power_up_akm4954( void )
{
    wiced_result_t result;
    static const uint8_t dummy[2];

    /* Set PDN pin. */
    wiced_gpio_output_high(PDN_PIN);

    /* Wait at least 1us before issuing dummy command. */
    wiced_rtos_delay_milliseconds(1);

    /* Write dummy command. */
    result = i2c_reg_write(&ak4954_control_port_i2c, dummy, sizeof dummy);
    wiced_assert("i2c_reg_write dummy", WICED_SUCCESS == result);

    return result;
}

wiced_result_t power_dn_akm4954( void )
{
    /* Clr PDN pin. */
    wiced_gpio_output_low(PDN_PIN);

    return WICED_SUCCESS;
}

wiced_result_t test_negative_probe( void )
{
    wiced_result_t      result;
    wiced_i2c_device_t  bad_control_port =
    {
        .port          = WICED_I2C_1,
        .address       = 0xA5,
        .address_width = I2C_ADDRESS_WIDTH_7BIT,
        .speed_mode    = I2C_STANDARD_SPEED_MODE, //I2C_LOW_SPEED_MODE,
    };

    result = wiced_i2c_init(&bad_control_port);
    if (result == WICED_SUCCESS)
    {
        wiced_bool_t device_is_available;

        device_is_available = wiced_i2c_probe_device(&bad_control_port, I2C_XFER_RETRY_COUNT);
        if (device_is_available == WICED_TRUE)
        {
            printf("found nonexistent device!\n");
            result = WICED_ERROR;
        }

        wiced_i2c_deinit(&bad_control_port);
    }

    return result;
}

int i2c_test( void )
{
    uint8_t bytes[8];
    eight_byte_val_t write_val =
    {
        .reg   = 0x32,
        .eight = EIGHT_BYTE_PATTERN,
    };

    /* Negative probe. */
    if (!DONT_PROBE && (test_negative_probe() != WICED_SUCCESS))
    {
        printf("Negative probe failed\n");
        return -1;
    }

    /* Initialize I2C port. */
    if (wiced_i2c_init(&ak4954_control_port_i2c) != WICED_SUCCESS)
    {
        printf( "Init I2C port failed\n" );
        return -2;
    }

    if (wiced_gpio_init(PDN_PIN, OUTPUT_PUSH_PULL) != WICED_SUCCESS)
    {
        printf("GPIO init failed\n");
        return -3;
    }

    /* Power-up AKM.
     * Do this after I2C initialization to bring SDA to tri-state before
     * AKM notices something is wrong. :)
     */
    if (power_up_akm4954() != WICED_SUCCESS)
    {
        printf("power_up_akm4954 failed\n");
        return -4;
    }

    /* Probe the AKM. */
    if (!DONT_PROBE && (wiced_i2c_probe_device(&ak4954_control_port_i2c, I2C_XFER_RETRY_COUNT) == WICED_FALSE))
    {
        printf("device not available\n");
        return -5;
    }

    /* Check default values starting @04H (size 8-bytes). */
    if (i2c_reg_read(&ak4954_control_port_i2c, 0x4, bytes, sizeof(bytes)) != WICED_SUCCESS)
    {
        printf("i2c_reg_read default 04H failed\n");
        return -6;
    }
    if (memcmp(bytes, AK4954_04H_8_BYTES_VAL, sizeof(bytes)) != 0)
    {
        printf("Incorrect default value for 8-bytes @ 04H\n");
        return -7;
    }

    /* Check default values starting @32H (size 8-bytes). */
    if (i2c_reg_read(&ak4954_control_port_i2c, 0x32, bytes, sizeof(bytes)) != WICED_SUCCESS)
    {
        printf("i2c_reg_read default 32H failed\n");
        return -8;
    }
    if (memcmp(bytes, AK4954_ZERO_8_BYTES, sizeof(bytes)) != 0)
    {
        printf( "i2c_reg_read default 32H failed\n" );
        return -9;
    }

    /* Write 8-byte sequence @32H. */
    if (i2c_reg_write(&ak4954_control_port_i2c, (uint8_t *)&write_val, sizeof(write_val)) != WICED_SUCCESS)
    {
        printf( "Write 8-byte 32H sequence failed\n" );
        return -10;
    }

    /* Read-back 8-byte sequence @32H. */
    if (i2c_reg_read(&ak4954_control_port_i2c, 0x32, bytes, sizeof(bytes)) != WICED_SUCCESS)
    {
        printf("read 8-byte 32H sequence failed\n");
        return -11;
    }
    if (memcmp(bytes, &EIGHT_BYTE_PATTERN, sizeof(bytes)) != 0)
    {
        printf("Incorrect value for 8-bytes @ 32H\n");
        return -12;
    }

    /* Power-down AKM. */
    if (power_dn_akm4954() != WICED_SUCCESS)
    {
        printf("power_dn_akm4954 failed\n");
        return -4;
    }

    ak4954_control_port_i2c = ak4954_control_port_i2c_low;

    return 0;
}
/*!
 ******************************************************************************
 * Inits and tests i2c with standard, low, and high speeds and reading/writing 04H and 32H sequences
 *
 * @return  0 for success, otherwise error
 */
int i2c( void )
{
    int ret_code = 0;

    printf("\n*** I2C TEST\n");

    printf("\nI2C standard speed test");
    ak4954_control_port_i2c = ak4954_control_port_i2c_standard;
    if ( i2c_test() != 0 )
    {
        ret_code = -1;
        printf(" FAILED\n");
    }
    else
    {
        printf(" SUCCEED\n");
    }

    printf("\nI2C low speed test");
    ak4954_control_port_i2c = ak4954_control_port_i2c_low;
    if ( i2c_test() != 0 )
    {
        ret_code = -2;
        printf(" FAILED\n");
    }
    else
    {
        printf(" SUCCEED\n");
    }

    printf("\nI2C high speed test");
    ak4954_control_port_i2c = ak4954_control_port_i2c_high;
    if ( i2c_test() != 0 )
    {
        ret_code = -3;
        printf(" FAILED\n");
    }
    else
    {
        printf(" SUCCEED\n");
    }

    printf("\n*** I2C TEST: %s\n", (ret_code == 0) ? "SUCCEED" : "FAILED");

    return ret_code;
}

#else

int i2c( void )
{
    printf( "\nI2C test NOT supported\n" );
    return 0;
}

#endif
