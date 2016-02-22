#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := App_coap_app

$(NAME)_SOURCES := coap_app.c

$(NAME)_COMPONENTS := protocols/COAP

WIFI_CONFIG_DCT_H := wifi_config_dct.h

INVALID_PLATFORMS    := BCM943909* BCM943907*