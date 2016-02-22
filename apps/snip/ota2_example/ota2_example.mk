#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := App_OTA2_example

# todo: things to support
#
# In ota2_bootloader
# SoftAp Support
#   - OTA2_BOOTLOADER_NO_SOFTAP_SUPPORT (After adding SOFTAP support)
#	- OTA2_USE_LAST_KNOWN_GOOD_AREA (when non- 0x00)
#	- CHECK_BATTERY_LEVEL_BEFORE_OTA2_UPGRADE
#		- CHECK_BATTERY_LEVEL_OTA2_UPGRADE_MINIMUM - minimum value to check for before doing an upgrade
#
# Background Downloading Service
#
# We are executing from RAM

#WICED_ENABLE_TRACEX := 1

APPLICATION_DCT    := ota2_test_dct.c

GLOBAL_DEFINES     += WICED_DISABLE_WATCHDOG
GLOBAL_DEFINES     += WPRINT_ENABLE_APP_DEBUG
GLOBAL_DEFINES     += WPRINT_ENABLE_LIB_DEBUG
GLOBAL_DEFINES     += PLATFORM_NO_DDR=1
GLOBAL_DEFINES     += APPLICATION_STACK_SIZE=16000
# stack needs to be big enough to handle the CRC32 calculation buffer
GLOBAL_DEFINES     += DCT_CRC32_CALCULATION_SIZE_ON_STACK=4096

$(NAME)_SOURCES    := ota2_test.c				\
					  ota2_test_config.c		\
					  ota2_test_dct.c

$(NAME)_COMPONENTS := utilities/command_console 	 \
                      utilities/command_console/wifi \
                      utilities/mini_printf 		 \
                      daemons/ota2_service		 	 \
                      filesystems/wicedfs			 \
					  protocols/DNS 				 \
					  protocols/HTTP

#GLOBAL_DEFINES     += CONSOLE_ENABLE_WL
ifneq (,$(findstring CONSOLE_ENABLE_WL,$(GLOBAL_DEFINES)))
# wl commands which dump a lot of data require big buffers.
GLOBAL_DEFINES   += WICED_PAYLOAD_MTU=8320
$(NAME)_COMPONENTS += test/wl_tool
endif

WIFI_CONFIG_DCT_H  := wifi_config_dct.h

ifdef WICED_ENABLE_TRACEX
$(info using tracex lib)
GLOBAL_DEFINES     += WICED_TRACEX_BUFFER_DDR_OFFSET=0x0
GLOBAL_DEFINES     += WICED_TRACEX_BUFFER_SIZE=0x200000
endif

ifeq ($(PLATFORM),$(filter $(PLATFORM), BCM943909WCD1 BCM943909WCD1_3.B0 BCM943909WCD1_3.B1 ))
GLOBAL_DEFINES     += WWD_TEST_NVRAM_OVERRIDE
GLOBAL_DEFINES += TX_PACKET_POOL_SIZE=5 \
                  RX_PACKET_POOL_SIZE=20 \
                  PBUF_POOL_TX_SIZE=8 \
                  PBUF_POOL_RX_SIZE=8 \
                  WICED_ETHERNET_DESCNUM_TX=32 \
                  WICED_ETHERNET_DESCNUM_RX=8 \
                  WICED_ETHERNET_RX_PACKET_POOL_SIZE=32+WICED_ETHERNET_DESCNUM_RX
endif

VALID_OSNS_COMBOS  := ThreadX-NetX ThreadX-NetX_Duo
VALID_PLATFORMS    := BCM943909WCD* BCM943907* FELIX_PROTO1_ext
INVALID_PLATFORMS  := BCM943907A*
