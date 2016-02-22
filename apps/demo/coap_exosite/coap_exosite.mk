#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := App_coap_exosite

$(NAME)_SOURCES := coap_exosite.c

$(NAME)_COMPONENTS := protocols/COAP

WIFI_CONFIG_DCT_H := wifi_config_dct.h

VALID_PLATFORMS := BCM943341WCD1 \
                   BCM943362WCD4 \
                   BCM943362WCD6 \
                   BCM943362WCD8 \
                   BCM943364WCD1 \
                   BCM94343WWCD1 \
                   BCM943438WCD1 \
                   BCM943909WCD1_3 \
                   BCM943907APS \
                   BCM943907WAE_1