#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME = SAM4S_Peripheral_Drivers

GLOBAL_INCLUDES := .

# Include STM32F2xx Standard Peripheral Libraries
$(NAME)_COMPONENTS += MCU/SAM4S/peripherals/libraries/ASF

$(NAME)_SOURCES := platform_adc.c \
                   platform_gpio.c \
                   platform_i2c.c \
                   platform_mcu_powersave.c \
                   platform_spi.c \
                   platform_pwm.c \
                   platform_uart.c \
                   platform_watchdog.c
