#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := Platform_BCM943903WCD1_1

WLAN_CHIP            := 43909
WLAN_CHIP_REVISION   := B0
WLAN_CHIP_FAMILY     := 4390x
APPS_CHIP_REVISION   := B1
HOST_MCU_FAMILY      := BCM4390x
HOST_MCU_VARIANT     := BCM43903
HOST_MCU_PART_NUMBER := BCM43903WLBGA

# BCM943903WCD1_1 includes BCM20707 BT chip
BT_CHIP          := 20707
BT_CHIP_REVISION := A1
BT_CHIP_XTAL_FREQUENCY := 40MHz

PLATFORM_SUPPORTS_BUTTONS := 1

GLOBAL_DEFINES += SFLASH_SUPPORT_MACRONIX_PARTS

WICED_BASE := ../../

$(NAME)_SOURCES := platform.c

GLOBAL_INCLUDES := .
