#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#


$(CHIP)_SOURCE_WL += $(CHIP)/wl/exe/wlu_common.c \
					 $(CHIP)/shared/bcmwifi/src/bcmwifi_channels.c \
                     $(CHIP)/wl/ppr/src/wlc_ppr.c \
					 $(CHIP)/wl/exe/wlu_rates_matrix.c

