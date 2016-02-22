#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := App_Apps_Sleep_test

$(NAME)_SOURCES  := apps_sleep.c

VALID_PLATFORMS  := BCM943909* BCM943907* BCM943903*
INVALID_PLATFORMS := BCM943909QT

GLOBAL_DEFINES   := WICED_NO_WIFI
GLOBAL_DEFINES   += NO_WIFI_FIRMWARE
GLOBAL_DEFINES   += PLATFORM_POWERSAVE_DEFAULT=1
GLOBAL_DEFINES   += PLATFORM_MCU_POWERSAVE_MODE_INIT=PLATFORM_MCU_POWERSAVE_MODE_DEEP_SLEEP

NO_WIFI_FIRMWARE := YES
