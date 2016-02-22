#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := diagnostics

#==============================================================================
# Console specific files
#==============================================================================
$(NAME)_SOURCES := diagnostics.c

$(NAME)_SOURCES +=	otp/otp.c \
					audio_loopback/audio_loopback.c \
					cpu_ddr/cpu_ddr.c \
					cpu_ddr/cpu.c \
					cpu_ddr/ddr.c \
					hibernation/hibernation.c \
					hwcrypto_test/hwcrypto_test.c \
					i2c/i2c.c \
					spi/spi.c \
					gpio/gpio.c \
					uart/dut/uart_dut.c \
					usb/usb.c \
					gci/gci.c \
					pwm/pwm.c \
					sdio/sdio.c \
					gmac_loopback/gmac_loopback.c

$(NAME)_COMPONENTS += utilities/command_console

#==============================================================================
# Additional command console modules
#==============================================================================
$(NAME)_COMPONENTS += utilities/command_console/wps \
                      utilities/command_console/wifi \
                      utilities/command_console/thread \
                      utilities/command_console/ping \
                      utilities/command_console/platform \
                      utilities/command_console/fs \
                      utilities/command_console/mallinfo

#==============================================================================
# Includes
#==============================================================================
$(NAME)_INCLUDES := .
#==============================================================================
# Configuration
#==============================================================================

#==============================================================================
# Global defines
#==============================================================================
GLOBAL_DEFINES += STDIO_BUFFER_SIZE=128

GLOBAL_DEFINES += OTP_DEBUG

ifeq ($(RING_BUFFER_ENABLE),1)
GLOBAL_DEFINES  += RING_BUFFER_ENABLE
endif

ifeq ($(FLOW_CONTROL_ENABLE),1)
GLOBAL_DEFINES  += FLOW_CONTROL_ENABLE
endif

ifdef DUT_UART
GLOBAL_DEFINES  += DUT_UART=$(DUT_UART)
endif

VALID_PLATFORMS := BCM943909* BCM943907* BCM943903*

#==============================================================================
# Wl tool inclusion
#==============================================================================
# Platforms & combinations with enough memory to fit WL tool, can declare CONSOLE_ENABLE_WL := 1
#CONSOLE_ENABLE_WL ?= 0

#==============================================================================
# Network stack-specific inclusion
#==============================================================================

GLOBAL_DEFINES += CONSOLE_ENABLE_THREADS

#==========audio=======================

GLOBAL_DEFINES     += WICED_USE_AUDIO

VALID_OSNS_COMBOS  := ThreadX-NetX_Duo
#==========audio=======================

# Uncomment this to compile sdio host driver
WICED_SDIO_SUPPORT := yes
