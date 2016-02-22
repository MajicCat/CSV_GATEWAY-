#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := App_audio_loopback

$(NAME)_SOURCES    := audio_loopback.c

GLOBAL_DEFINES     += WICED_USE_AUDIO

VALID_OSNS_COMBOS  := ThreadX-NetX_Duo

VALID_PLATFORMS    := BCM943909WCD* BCM943907WAE_1* BCM943907APS*
