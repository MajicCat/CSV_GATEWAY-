#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := App_Bluetooth_Low_Energy_Mesh_Network

$(NAME)_SOURCES    := blemesh_network.c \
                      wiced_bt_cfg.c

$(NAME)_COMPONENTS += daemons/bt_internet_gateway/blemesh

VALID_PLATFORMS    := BCM943909WCD1_3*

WIFI_CONFIG_DCT_H  := wifi_config_dct.h
APPLICATION_DCT    := blemesh_dct.c
