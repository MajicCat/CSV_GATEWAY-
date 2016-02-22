#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := App_HTTP_Server_Sent_Events

$(NAME)_SOURCES    := http_server_sent_events.c

$(NAME)_RESOURCES  := apps/http_server_sent_events/main.html \
                      images/brcmlogo.png \
                      images/brcmlogo_line.png \
                      images/favicon.ico

GLOBAL_DEFINES     :=

WIFI_CONFIG_DCT_H  := wifi_config_dct.h

$(NAME)_COMPONENTS := daemons/HTTP_server \
                      protocols/SNTP
