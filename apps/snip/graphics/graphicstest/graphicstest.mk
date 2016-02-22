#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := Graphics_GraphicsTest_Application

# NOTE: repeated start functionality on BCM4390x only value for I2C_1 bus interface
#GLOBAL_DEFINES     += U8G_I2C_USE_REPEAT_START

$(NAME)_SOURCES    := graphicstest.c

$(NAME)_COMPONENTS := graphics/u8g

VALID_PLATFORMS    := BCM943909WCD1_3 BCM943907WAE_1*
