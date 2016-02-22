#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := App_apollo

APOLLO_TX_PACKET_POOL_SIZE ?= 10
APOLLO_RX_PACKET_POOL_SIZE ?= 10

#WICED_ENABLE_TRACEX := 1

#GLOBAL_DEFINES     += CONSOLE_ENABLE_WL

GLOBAL_DEFINES     += APPLICATION_STACK_SIZE=8000
GLOBAL_DEFINES     += WICED_DCT_INCLUDE_BT_CONFIG

$(NAME)_SOURCES    := apollo.c
$(NAME)_SOURCES    += apollo_config.c
$(NAME)_SOURCES    += apollo_debug.c

$(NAME)_COMPONENTS := audio/apollo/audio_render \
                      audio/apollo/apollo_player \
                      audio/apollo/apollo_streamer \
                      audio/apollo/apollocore \
                      audio/apollo/apollo_config_gatt_server \
                      audio/display \
                      utilities/command_console \
                      utilities/command_console/wifi \
                      inputs/button_manager \
                      drivers/power_management

ifneq (,$(findstring BCM943907WAE_1,$(PLATFORM)))
    GLOBAL_DEFINES  += POWER_MANAGEMENT_ON_BCM943907WAE_1
endif

# include display
# GLOBAL_DEFINES += USE_AUDIO_DISPLAY

ifneq (,$(findstring CONSOLE_ENABLE_WL,$(GLOBAL_DEFINES)))
$(NAME)_COMPONENTS += test/wl_tool
endif

APPLICATION_DCT    := apollo_dct.c

WIFI_CONFIG_DCT_H  := wifi_config_dct.h

# Bluetooth MAC address and name are configured via the DCT
BT_CONFIG_DCT_H    := bt_config_dct.h

ifdef WICED_ENABLE_TRACEX
$(info apollo using tracex lib)
GLOBAL_DEFINES     += WICED_TRACEX_BUFFER_DDR_OFFSET=0x0
GLOBAL_DEFINES     += WICED_TRACEX_BUFFER_SIZE=0x200000
endif

GLOBAL_DEFINES     += TX_PACKET_POOL_SIZE=$(APOLLO_TX_PACKET_POOL_SIZE)
GLOBAL_DEFINES     += RX_PACKET_POOL_SIZE=$(APOLLO_RX_PACKET_POOL_SIZE)

GLOBAL_DEFINES     += WICED_USE_AUDIO

GLOBAL_DEFINES     += AUTO_IP_ENABLED

#GLOBAL_DEFINES     += WICED_DISABLE_WATCHDOG

VALID_OSNS_COMBOS  := ThreadX-NetX_Duo
VALID_PLATFORMS    :=
VALID_PLATFORMS    += BCM943909WCD*
VALID_PLATFORMS    += BCM943907*
INVALID_PLATFORMS  += BCM943907AEVAL*
