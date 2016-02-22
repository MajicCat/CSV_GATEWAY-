#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := App_flac_test

#WICED_ENABLE_TRACEX := 1
GLOBAL_DEFINES     += WICED_DISABLE_WATCHDOG
GLOBAL_DEFINES     += WICED_DISABLE_TLS

GLOBAL_DEFINES		+= WPRINT_ENABLE_APP_DEBUG
GLOBAL_DEFINES		+= WPRINT_ENABLE_LIB_DEBUG

# ENABLE for ethernet support
#$(NAME)_DEFINES   += MFG_TEST_ENABLE_ETHERNET_SUPPORT

APPLICATION_DCT    := flac_app_dct.c

GLOBAL_DEFINES     += WWD_TEST_NVRAM_OVERRIDE
GLOBAL_DEFINES     += APPLICATION_STACK_SIZE=8000

$(NAME)_SOURCES    := flac_test.c	\
					  flac_config.c		\
					  flac_app_dct.c		\
					  wiced_flac_interface.c

$(NAME)_COMPONENTS := audio/apollo/audio_render \
                      audio/apollo/apollocore \
                      utilities/command_console \
                      utilities/command_console/wifi \
                      filesystems/wicedfs \
					  audio/codec/FLAC \
					  protocols/DNS \
					  protocols/HTTP

$(NAME)_RESOURCES  := apps/flac/left_right_48k_16bit_2ch.flac


MULTI_APP_WIFI_FIRMWARE   := resources/firmware/$(WLAN_CHIP)/$(WLAN_CHIP)$(WLAN_CHIP_REVISION)-mfgtest.bin


#GLOBAL_DEFINES     += CONSOLE_ENABLE_WL
ifneq (,$(findstring CONSOLE_ENABLE_WL,$(GLOBAL_DEFINES)))
# wl commands which dump a lot of data require big buffers.
GLOBAL_DEFINES   += WICED_PAYLOAD_MTU=8320
$(NAME)_COMPONENTS += test/wl_tool
endif

WIFI_CONFIG_DCT_H  := wifi_config_dct.h

ifdef WICED_ENABLE_TRACEX
$(info apollo_audio using tracex lib)
GLOBAL_DEFINES     += WICED_TRACEX_BUFFER_DDR_OFFSET=0x0
GLOBAL_DEFINES     += WICED_TRACEX_BUFFER_SIZE=0x200000
endif

ifeq ($(PLATFORM),$(filter $(PLATFORM),BCM943909WCD1_3.B0 BCM943909WCD1_3.B1 ))
GLOBAL_DEFINES += TX_PACKET_POOL_SIZE=5 \
                  RX_PACKET_POOL_SIZE=20 \
                  PBUF_POOL_TX_SIZE=8 \
                  PBUF_POOL_RX_SIZE=8 \
                  WICED_ETHERNET_DESCNUM_TX=32 \
                  WICED_ETHERNET_DESCNUM_RX=8 \
                  WICED_ETHERNET_RX_PACKET_POOL_SIZE=32+WICED_ETHERNET_DESCNUM_RX
endif


GLOBAL_DEFINES     += WICED_USE_AUDIO
GLOBAL_DEFINES     += WICED_USE_AUDIO_FLAC


VALID_OSNS_COMBOS  := ThreadX-NetX_Duo
VALID_PLATFORMS    := BCM943909WCD* BCM943907*
INVALID_PLATFORMS  := BCM943907AEVAL*
