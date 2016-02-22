#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

#This app only produces meaningful
#output on BCM943907WAE_1 boards

NAME := Power_Management_Application

$(NAME)_SOURCES    := power_management.c

$(NAME)_COMPONENTS := drivers/power_management

VALID_PLATFORMS    := BCM943907WAE_1*
