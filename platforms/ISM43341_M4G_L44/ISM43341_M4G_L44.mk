#
# Copyright 2014, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := Platform_ISM43341

WLAN_CHIP            := 43341
WLAN_CHIP_REVISION   := B0
WLAN_CHIP_FAMILY     := 4334x
HOST_MCU_FAMILY      := STM32F4xx
#Please note the ISM43341 uses a STM32F405 doesn't have the cryto/hash processor 
HOST_MCU_VARIANT     := STM32F415
HOST_MCU_PART_NUMBER := STM32F405IGH6
BT_CHIP              := 43341
BT_CHIP_REVISION     := B0
BT_MODE              ?= HCI

PLATFORM_SUPPORTS_BUTTONS := 1

INTERNAL_MEMORY_RESOURCES = $(ALL_RESOURCES)

ifndef BUS
BUS := SDIO
endif

EXTRA_TARGET_MAKEFILES +=  $(MAKEFILES_PATH)/standard_platform_targets.mk

VALID_BUSES := SDIO SPI

ifeq ($(BUS),SDIO)
ifeq ($(MULTI_APP_WIFI_FIRMWARE),)
GLOBAL_DEFINES          += WWD_DIRECT_RESOURCES
else
# Setting some internal build parameters
WIFI_FIRMWARE           := $(MULTI_APP_WIFI_FIRMWARE)
WIFI_FIRMWARE_LOCATION 	:= WIFI_FIRMWARE_IN_MULTI_APP
GLOBAL_DEFINES          += WIFI_FIRMWARE_IN_MULTI_APP
endif
endif

# Global includes
GLOBAL_INCLUDES  += .
GLOBAL_INCLUDES  += $(WICED_BASE)/libraries/inputs/gpio_button

# Global defines
# HSE_VALUE = STM32 crystal frequency = 26MHz (needed to make UART work correctly)
GLOBAL_DEFINES += HSE_VALUE=26000000
ifneq ($(APP),console)
GLOBAL_DEFINES += $$(if $$(NO_CRLF_STDIO_REPLACEMENT),,CRLF_STDIO_REPLACEMENT)
endif #console

# Components
$(NAME)_COMPONENTS += drivers/spi_flash \
                      inputs/gpio_button
# Source files
$(NAME)_SOURCES 	:= platform.c 


# WICED APPS
# APP0 and FILESYSTEM_IMAGE are reserved main app and resources file system
# FR_APP :=
# DCT_IMAGE :=
# OTA_APP :=
# FILESYSTEM_IMAGE :=
# WIFI_FIRMWARE :=
# APP0 :=
# APP1 :=
# APP2 :=

# WICED APPS LOOKUP TABLE
APPS_LUT_HEADER_LOC := 0x0000
APPS_START_SECTOR := 1

ifneq ($(APP),bootloader)
ifneq ($(MAIN_COMPONENT_PROCESSING),1)
$(info +-----------------------------------------------------------------------------------------------------+ )
$(info | IMPORTANT NOTES                                                                                     | )
$(info +-----------------------------------------------------------------------------------------------------+ )
$(info | Wi-Fi MAC Address                                                                                   | )
$(info |    The target Wi-Fi MAC address is defined in <WICED-SDK>/generated_mac_address.txt                 | )
$(info |    Ensure each target device has a unique address.                                                  | )
$(info +-----------------------------------------------------------------------------------------------------+ )
$(info | MCU & Wi-Fi Power Save                                                                              | )
$(info |    It is *critical* that applications using WICED Powersave API functions connect an accurate 32kHz | )
$(info |    reference clock to the sleep clock input pin of the WLAN chip. Please read the WICED Powersave   | )
$(info |    Application Note located in the documentation directory if you plan to use powersave features.   | )
$(info +-----------------------------------------------------------------------------------------------------+ )
endif
endif

