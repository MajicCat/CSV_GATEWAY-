#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME = LPC18xx_Peripheral_Drivers

GLOBAL_INCLUDES := .
GLOBAL_DEFINES  := WICED_DISABLE_MCU_POWERSAVE \
                   WICED_DISABLE_WATCHDOG

# Include LPC Standard Peripheral Libraries
$(NAME)_COMPONENTS += MCU/LPC18xx/peripherals/libraries

$(NAME)_SOURCES := platform_gpio.c \
                   platform_mcu_powersave.c \
                   platform_uart.c \
                   platform_watchdog.c \
                   platform_spi.c  \
                   platform_spifi.c  \
                   platform_sdio.c
