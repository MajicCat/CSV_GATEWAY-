#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME = USB_Host_43909_Library_$(PLATFORM)

GLOBAL_INCLUDES := .

#$(NAME)_DEFINES :=

$(NAME)_SOURCES := platform_usb_host.c

#GLOBAL_DEFINES += WICED_USB_SUPPORT=1

$(eval $(call PLATFORM_LOCAL_DEFINES_INCLUDES_43909, ../..))
