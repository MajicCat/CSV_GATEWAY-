#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := Tiny_Crypto_43909_Library_$(PLATFORM)

$(NAME)_SOURCES := platform_tiny_crypto.c

$(NAME)_CFLAGS  = $(COMPILER_SPECIFIC_PEDANTIC_CFLAGS)

$(eval $(call PLATFORM_LOCAL_DEFINES_INCLUDES_43909, ../../..))
