#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := App_Ping_ethernet

$(NAME)_SOURCES  := ping_ethernet.c

VALID_PLATFORMS  := BCM943909* BCM943907*
INVALID_PLATFORMS := BCM943909QT

GLOBAL_DEFINES   := WICED_NO_WIFI
GLOBAL_DEFINES   += NO_WIFI_FIRMWARE

NO_WIFI_FIRMWARE := YES
