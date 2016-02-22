#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := App_Bluetooth_HPS_Server

$(NAME)_SOURCES    := hps_server.c \
                      wiced_bt_config.c

$(NAME)_INCLUDES   := .

$(NAME)_COMPONENTS := daemons/bt_internet_gateway/bt_http_proxy_server

WIFI_CONFIG_DCT_H  := wifi_config_dct.h

VALID_PLATFORMS    := BCM9WCDPLUS114 \
                      BCM943341WCD1 \
                      BCM943909WCD* \
                      BCM9WCD1AUDIO \
                      BCM943438WLPTH_2 \
                      BCM94343WWCDA_ext
