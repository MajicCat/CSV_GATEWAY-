#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := App_Bluetooth_Audio

$(NAME)_SOURCES    := bluetooth_audio.c \
                      bluetooth_audio_player.c \
                      bluetooth_audio_nv.c \
                      bluetooth_audio_decoder.c

$(NAME)_COMPONENTS := libraries/drivers/bluetooth \
                      libraries/audio/codec/codec_framework \
                      libraries/audio/codec/sbc_if

BT_CONFIG_DCT_H := bt_config_dct.h

GLOBAL_DEFINES := BUILDCFG \
                  WICED_USE_AUDIO \
                  WICED_NO_WIFI \
                  NO_WIFI_FIRMWARE \
                  TX_PACKET_POOL_SIZE=3 \
                  RX_PACKET_POOL_SIZE=32 \
                  WICED_DCT_INCLUDE_BT_CONFIG \
                  USE_MEM_POOL

ifneq (,$(findstring USE_MEM_POOL,$(GLOBAL_DEFINES)))
$(NAME)_SOURCES   += mem_pool/mem_pool.c
$(NAME)_INCLUDES  := ./mem_pool
#GLOBAL_DEFINES    += MEM_POOL_DEBUG
endif

# Define ENABLE_BT_PROTOCOL_TRACES to enable Bluetooth protocol/profile level
# traces.
#GLOBAL_DEFINES     += ENABLE_BT_PROTOCOL_TRACES

VALID_OSNS_COMBOS  := ThreadX-NetX_Duo

VALID_PLATFORMS    := BCM9WCD1AUDIO BCM943909* BCM943907WAE_1* BCM943907APS*
INVALID_PLATFORMS  := BCM943909QT

NO_WIFI_FIRMWARE   := YES
