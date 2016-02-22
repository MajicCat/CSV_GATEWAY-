#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := App_console

#==============================================================================
# Console specific files
#==============================================================================
$(NAME)_SOURCES := wiced_init.c

$(NAME)_COMPONENTS += utilities/command_console

#==============================================================================
# Additional command console modules
#==============================================================================
$(NAME)_COMPONENTS += utilities/command_console/wps \
                      utilities/command_console/wifi \
                      utilities/command_console/thread \
                      utilities/command_console/ping \
                      utilities/command_console/platform \
                      utilities/command_console/mallinfo

ifneq ($(PLATFORM),$(filter $(PLATFORM), BCM94390WCD1 BCM94390WCD2 BCM94390WCD3 BCM943341WAE BCM943341WCD1 ))
$(NAME)_COMPONENTS           += utilities/command_console/p2p
WICED_USE_WIFI_P2P_INTERFACE := 1
$(NAME)_DEFINES              += CONSOLE_INCLUDE_P2P
GLOBAL_DEFINES               += WICED_DCT_INCLUDE_P2P_CONFIG
endif

BCM94390x_ETHERNET_PLATFORMS := BCM943909* BCM943907*
$(eval BCM94390x_ETHERNET_PLATFORMS := $(call EXPAND_WILDCARD_PLATFORMS,$(BCM94390x_ETHERNET_PLATFORMS)))
ifeq ($(PLATFORM),$(filter $(PLATFORM), $(BCM94390x_ETHERNET_PLATFORMS)))
$(NAME)_COMPONENTS += utilities/command_console/ethernet
$(NAME)_DEFINES += CONSOLE_INCLUDE_ETHERNET
endif

#==============================================================================
# Includes
#==============================================================================
$(NAME)_INCLUDES := .

#==============================================================================
# Configuration
#==============================================================================

#==============================================================================
# Global defines
#==============================================================================
GLOBAL_DEFINES += STDIO_BUFFER_SIZE=128

# Increase packet pool size for particular platforms
ifeq ($(PLATFORM),$(filter $(PLATFORM), BCM943362WCD4 BCM94390WCD2 ))
GLOBAL_DEFINES += TX_PACKET_POOL_SIZE=10 \
                  RX_PACKET_POOL_SIZE=10 \
                  WICED_TCP_TX_DEPTH_QUEUE=8 \
                  TCP_WINDOW_SIZE=8192
else
# Set 43909 specific packet pool settings
BCM94390x_PLATFORMS := BCM943909* BCM943907* BCM943903*
$(eval BCM94390x_PLATFORMS := $(call EXPAND_WILDCARD_PLATFORMS,$(BCM94390x_PLATFORMS)))
ifeq ($(PLATFORM),$(filter $(PLATFORM), $(BCM94390x_PLATFORMS)))
GLOBAL_DEFINES += TX_PACKET_POOL_SIZE=10 \
                  RX_PACKET_POOL_SIZE=20 \
                  WICED_TCP_TX_DEPTH_QUEUE=8 \
                  WICED_ETHERNET_DESCNUM_TX=32 \
                  WICED_ETHERNET_DESCNUM_RX=8 \
                  WICED_ETHERNET_RX_PACKET_POOL_SIZE=32+WICED_ETHERNET_DESCNUM_RX \
                  TCP_WINDOW_SIZE=8192
else
# Otherwise use default values
endif
endif


INVALID_PLATFORMS := BCM943362WCD4_LPCX1769 \
                     BCM943362WCDA \
                     STMDiscovery411_BCM43438

#==============================================================================
# Wl tool inclusion
#==============================================================================
# Platforms & combinations with enough memory to fit WL tool, can declare CONSOLE_ENABLE_WL := 1
CONSOLE_ENABLE_WL ?= 0

#==============================================================================
# Network stack-specific inclusion
#==============================================================================
ifeq ($(NETWORK),NetX)
#$(NAME)_SOURCES += NetX/netdb.c

ifdef CONSOLE_ENABLE_WPS
GLOBAL_DEFINES  += ADD_NETX_EAPOL_SUPPORT
endif
endif

ifeq ($(NETWORK),NetX_Duo)
#$(NAME)_SOURCES += NetX_Duo/netdb.c

ifdef CONSOLE_ENABLE_WPS
GLOBAL_DEFINES  += ADD_NETX_EAPOL_SUPPORT
endif
endif

GLOBAL_DEFINES += CONSOLE_ENABLE_THREADS

#==============================================================================
# iperf inclusion
#==============================================================================
#4390 does not have enough memory for iperf
ifneq (,$(filter $(PLATFORM),BCM94390WCD1 BCM94390WCD2 BCM94390WCD3))
CONSOLE_NO_IPERF :=1
endif

ifndef CONSOLE_NO_IPERF
$(NAME)_COMPONENTS += test/iperf
$(NAME)_DEFINES    += CONSOLE_ENABLE_IPERF
endif
