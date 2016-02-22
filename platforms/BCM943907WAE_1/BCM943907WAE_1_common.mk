#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := Platform_BCM943907WAE_1

WLAN_CHIP            := 43909
WLAN_CHIP_FAMILY     := 4390x
HOST_MCU_FAMILY      := BCM4390x
HOST_MCU_VARIANT     := BCM43907
HOST_MCU_PART_NUMBER := BCM43907WLCSP

# BCM943907WAE_1 includes BCM20707 BT chip
BT_CHIP          := 20707
BT_CHIP_REVISION := A1
BT_CHIP_XTAL_FREQUENCY := 40MHz

# BCM943907WAE_1 does not have Ethernet or DDR
PLATFORM_NO_GMAC := 1
PLATFORM_NO_DDR := 1
PLATFORM_NO_USB_HOST := 1

PLATFORM_SUPPORTS_BUTTONS := 1

# BCM943907WAE_1 includes Maxim17040 and Maxim8971 chips
GLOBAL_DEFINES     += POWER_MANAGEMENT_SUPPORT_MAXIM_CHIPS
$(NAME)_COMPONENTS += drivers/power_management

GLOBAL_DEFINES += SFLASH_SUPPORT_MACRONIX_PARTS

WICED_BASE := ../../../
PLATFORM_SOURCES := $(WICED_BASE)/platforms/$(PLATFORM_DIRECTORY)/../

$(NAME)_SOURCES := ../platform.c

GLOBAL_INCLUDES := .  \
                   ..

ifeq (,$(APP_WWD_ONLY)$(NS_WWD_ONLY)$(RTOS_WWD_ONLY))
$(NAME)_SOURCES += ../platform_audio.c \
                   ../wiced_audio.c
$(NAME)_COMPONENTS += drivers/audio/AK4961
endif  # (,$(APP_WWD_ONLY)$(NS_WWD_ONLY)$(RTOS_WWD_ONLY))
