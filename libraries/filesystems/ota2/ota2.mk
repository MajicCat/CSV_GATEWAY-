#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#
NAME := Lib_OTA2

OTA2_SUPPORT := 1

$(NAME)_SOURCES := ./wiced_ota2_image.c

GLOBAL_INCLUDES += .

$(NAME)_COMPONENTS += utilities/crc


