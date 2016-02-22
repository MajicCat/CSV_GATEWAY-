#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME := App_powersave

$(NAME)_SOURCES := powersave.c

$(NAME)_COMPONENTS += utilities/command_console \
                      utilities/command_console/wps \
                      utilities/command_console/wifi \
                      utilities/command_console/thread \
                      utilities/command_console/ping \
                      utilities/command_console/platform \
                      utilities/command_console/mallinfo \
                      utilities/command_console/ethernet \
                      utilities/command_console/p2p \
                      test/iperf

GLOBAL_DEFINES += STDIO_BUFFER_SIZE=128
GLOBAL_DEFINES += PLATFORM_POWERSAVE_DEFAULT=1 PLATFORM_WLAN_POWERSAVE_STATS=1
GLOBAL_DEFINES += TX_PACKET_POOL_SIZE=10 \
                  RX_PACKET_POOL_SIZE=20 \
                  WICED_TCP_TX_DEPTH_QUEUE=8 \
                  WICED_ETHERNET_DESCNUM_TX=32 \
                  WICED_ETHERNET_DESCNUM_RX=8 \
                  WICED_ETHERNET_RX_PACKET_POOL_SIZE=32+WICED_ETHERNET_DESCNUM_RX \
                  TCP_WINDOW_SIZE=8192
GLOBAL_DEFINES += ADD_NETX_EAPOL_SUPPORT
GLOBAL_DEFINES += WICED_DCT_INCLUDE_P2P_CONFIG
GLOBAL_DEFINES += WPRINT_PLATFORM_PERMISSION

$(NAME)_DEFINES += CONSOLE_INCLUDE_ETHERNET \
                   CONSOLE_INCLUDE_P2P \
                   CONSOLE_ENABLE_IPERF \
                   CONSOLE_ENABLE_THREADS

# Define as 1 if want to include WL commands into application
CONSOLE_ENABLE_WL := 0

VALID_PLATFORMS   := BCM943909* BCM943907* BCM943903*
INVALID_PLATFORMS := BCM943909QT
