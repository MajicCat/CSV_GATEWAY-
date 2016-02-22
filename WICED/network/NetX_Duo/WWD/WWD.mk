#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := WWD_NetX_Duo_Interface

GLOBAL_INCLUDES := .

$(NAME)_SOURCES := wwd_buffer.c \
                   wwd_network.c

$(NAME)_CFLAGS  = $(COMPILER_SPECIFIC_PEDANTIC_CFLAGS)

$(NAME)_CHECK_HEADERS := \
                         wwd_buffer.h \
                         wwd_network.h