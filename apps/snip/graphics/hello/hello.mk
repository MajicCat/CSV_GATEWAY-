#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := Graphics_Hello_Application

$(NAME)_SOURCES    := hello.c

$(NAME)_COMPONENTS := graphics/u8g

VALID_PLATFORMS    := BCM943909WCD1_3 BCM943907WAE_1*
