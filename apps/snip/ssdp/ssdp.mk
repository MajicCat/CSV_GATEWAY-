#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#
NAME := App_SSDP
$(NAME)_SOURCES := ssdp_app.c \
                   ssdp_app_config.c \

$(NAME)_COMPONENTS := daemons/SSDP \
                      daemons/HTTP_server \
                      utilities/command_console \
                      utilities/command_console/wifi

GLOBAL_DEFINES     += WWD_TEST_NVRAM_OVERRIDE
GLOBAL_DEFINES     += APPLICATION_STACK_SIZE=8000
GLOBAL_DEFINES     += WPRINT_ENABLE_APP_DEBUG
#GLOBAL_DEFINES     += WPRINT_ENABLE_LIB_DEBUG

APPLICATION_DCT    := ssdp_app_dct.c

WIFI_CONFIG_DCT_H  := wifi_config_dct.h

VALID_OSNS_COMBOS  := ThreadX-NetX ThreadX-NetX_Duo
VALID_PLATFORMS    := BCM943909WCD*
