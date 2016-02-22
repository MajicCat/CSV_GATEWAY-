#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME          := Lib_iperf

#==============================================================================
# Configuration
#==============================================================================
WICED_THREADS          := 0
IPERF_DEBUG            := 0
IPERF_THREAD_STACKSIZE := 512
IPERF_THREAD_PRIORITY  := 1

#==============================================================================
# iperf includes
#==============================================================================
GLOBAL_INCLUDES  := .

$(NAME)_INCLUDES := include


#==============================================================================
# iperf source files
#==============================================================================
$(NAME)_SOURCES := src/Client.cpp \
                   src/Extractor.c \
                   src/Launch.cpp \
                   src/List.cpp \
                   src/Listener.cpp \
                   src/Locale.c \
                   src/PerfSocket.cpp \
                   src/ReportCSV.c \
                   src/ReportDefault.c \
                   src/Reporter.c \
                   src/Server.cpp \
                   src/Settings.cpp \
                   src/SocketAddr.c \
                   src/sockets.c \
                   src/stdio.c \
                   src/tcp_window_size.c \
                   src/debug.c

#==============================================================================
# iperf compatability files
#==============================================================================
$(NAME)_SOURCES += compat/delay.cpp \
                   compat/error.c \
                   compat/gettimeofday.c \
                   compat/inet_ntop.c \
                   compat/inet_pton.c \
                   compat/setitimer.c \
                   compat/signal.c \
                   compat/snprintf.c \
                   compat/string.c \
                   compat/Thread.c


#==============================================================================
# iperf WICED-compatability files
#==============================================================================
$(NAME)_SOURCES += WICED/Condition_$(RTOS).c \
                   WICED/gettimeofday.c \
                   WICED/Thread_$(RTOS).c \
                   WICED/usleep.c \
                   WICED/wiced_cpp.cpp \
                   WICED/netdb.c \
                   WICED/$(NETWORK)/sockets.c \
                   WICED/iperf_test.c

#==============================================================================
# iperf main
#==============================================================================
$(NAME)_SOURCES += src/main.cpp


#==============================================================================
# Local defines
#==============================================================================
GLOBAL_DEFINES  += WICED
$(NAME)_DEFINES += IPERF_THREAD_STACKSIZE=$(IPERF_THREAD_STACKSIZE)
$(NAME)_DEFINES += IPERF_THREAD_PRIORITY=$(IPERF_THREAD_PRIORITY)
# Turn iperf threading on/off
ifeq (0, $(WICED_THREADS))
$(NAME)_DEFINES += NO_THREADS
else
ifeq (FreeRTOS, $(RTOS))
# Needed for mutexes in FreeRTOS
GLOBAL_DEFINES += configUSE_MUTEXES=1
endif
endif

ifeq ($(NETWORK),NetX)
#GLOBAL_DEFINES += NX_EXTENDED_BSD_SOCKET_SUPPORT
endif

ifeq ($(NETWORK),NetX_Duo)
#GLOBAL_DEFINES += NXD_EXTENDED_BSD_SOCKET_SUPPORT
endif

# Turn iperf debugging on/off
ifeq (1, $(IPERF_DEBUG))
$(NAME)_DEFINES += IPERF_DEBUG=1
else
$(NAME)_DEFINES += IPERF_DEBUG=0
endif

#==============================================================================
# Global defines
#==============================================================================
ifeq (LwIP, $(NETWORK))
# Required with LwIP to prevent redefinition of struct timeval
GLOBAL_DEFINES += LWIP_TIMEVAL_PRIVATE=0
GLOBAL_DEFINES += LWIP_SO_RCVBUF=1
endif


