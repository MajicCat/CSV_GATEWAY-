#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := App_OTA2_Bootloader_$(PLATFORM)

# In ota2_bootloader
# SoftAp Support
#   - OTA2_BOOTLOADER_NO_SOFTAP_SUPPORT (After adding SOFTAP support)
#	- OTA2_USE_LAST_KNOWN_GOOD_AREA (when non- 0x00)
#	- CHECK_BATTERY_LEVEL_BEFORE_OTA2_UPGRADE
#		- CHECK_BATTERY_LEVEL_OTA2_UPGRADE_MINIMUM - minimum value to check for before doing an upgrade

OTA2_SUPPORT := 1

$(NAME)_SOURCES    := ./ota2_bootloader.c

$(NAME)_COMPONENTS := filesystems/ota2		\
					  utilities/crc			\
					  utilities/mini_printf


NoOS_START_STACK   := 20000
NoOS_FIQ_STACK     := 0
NoOS_IRQ_STACK     := 256
NoOS_SYS_STACK     := 0

APP_WWD_ONLY       := 1
NO_WIFI_FIRMWARE   := YES
NO_WIFI            := YES


GLOBAL_DEFINES     += WICED_NO_WIFI
GLOBAL_DEFINES     += WICED_DISABLE_MCU_POWERSAVE
GLOBAL_DEFINES     += WICED_DCACHE_WTHROUGH
GLOBAL_DEFINES     += NO_WIFI_FIRMWARE
GLOBAL_DEFINES     += BOOTLOADER
# stack needs to be big enough to handle the CRC32 calculation buffer
GLOBAL_DEFINES     += DCT_CRC32_CALCULATION_SIZE_ON_STACK=4096

# SoftAP is future support
GLOBAL_DEFINES		+= OTA2_BOOTLOADER_NO_SOFTAP_SUPPORT

# Future support
#GLOBAL_DEFINES		+= OTA2_USE_LAST_KNOWN_GOOD_AREA
#GLOBAL_DEFINES		+= CHECK_BATTERY_LEVEL_BEFORE_OTA2_UPGRADE
#GLOBAL_DEFINES		+= CHECK_BATTERY_LEVEL_OTA2_UPGRADE_MINIMUM=?


# When building release, don't print anything out
#GLOBAL_DEFINES     += WICED_DISABLE_STDIO

GLOBAL_INCLUDES    += .

VALID_OSNS_COMBOS  := NoOS
VALID_BUILD_TYPES  := debug release
VALID_PLATFORMS    := BCM943909WCD* BCM943907*
INVALID_PLATFORMS  := BCM943907A*
