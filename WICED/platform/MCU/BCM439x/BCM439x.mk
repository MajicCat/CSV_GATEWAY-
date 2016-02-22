#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME = MCU_BCM439x

# Host architecture is ARM Cortex M3
HOST_ARCH := ARM_CM3

FAMILY := BCM439x

# Host MCU alias for OpenOCD
HOST_OPENOCD := $(FAMILY)

VALID_BUSES := SoC.4390

ifndef BUS
BUS := SoC.4390
endif

GLOBAL_INCLUDES := . \
                   .. \
                   ../.. \
                   ../../include \
                   ../../$(HOST_ARCH) \
                   ../../$(HOST_ARCH)/CMSIS \
                   ../../../../libraries/drivers/spi_flash \
                   peripherals \
                   WAF \
                   WWD

# Global defines
GLOBAL_DEFINES += BCMDBG_DUMP
GLOBAL_DEFINES += NETWORK_NOTIFY_RELEASED_PACKETS
#GLOBAL_DEFINES += CPU_CLOCK_SPEED=48
#GLOBAL_DEFINES += PA_CONTROL=12
#GLOBAL_DEFINES += DMA_CHANNELS=1

# Global flags
$(NAME)_DEFINES += DMA_ENABLED
GLOBAL_CFLAGS   += $$(CPU_CFLAGS)    $$(ENDIAN_CFLAGS_LITTLE)
GLOBAL_CXXFLAGS += $$(CPU_CXXFLAGS)  $$(ENDIAN_CXXFLAGS_LITTLE)
GLOBAL_ASMFLAGS += $$(CPU_ASMFLAGS)  $$(ENDIAN_ASMFLAGS_LITTLE)
GLOBAL_LDFLAGS  += $$(CPU_LDFLAGS)   $$(ENDIAN_LDFLAGS_LITTLE)

ifeq ($(TOOLCHAIN_NAME),GCC)
GLOBAL_LDFLAGS  += -nostartfiles
GLOBAL_LDFLAGS  += -Wl,--defsym,__STACKSIZE__=$$($(RTOS)_START_STACK)
GLOBAL_LDFLAGS  += -L ./WICED/platform/MCU/$(FAMILY)/$(TOOLCHAIN_NAME) \
                   -L ./WICED/platform/MCU/$(FAMILY)/$(TOOLCHAIN_NAME)/$(WLAN_CHIP)$(WLAN_CHIP_REVISION)
endif

# Components
$(NAME)_COMPONENTS  += $(TOOLCHAIN_NAME) \
                       MCU/BCM439x/peripherals \
                       MCU/BCM439x/WWD/internal \
                       utilities/ring_buffer

# send spi flash requests through normal wiced driver
#$(NAME)_COMPONENTS += drivers/spi_flash
#GLOBAL_DEFINES += SPI_DRIVER_SFLASH
#GLOBAL_DEFINES += EXPOSED_4390_SFLASH_PINS


ifneq ($(APP),bootloader)
$(NAME)_COMPONENTS  += MCU/BCM439x/peripherals/ROM/$(WLAN_CHIP)$(WLAN_CHIP_REVISION)
endif

$(NAME)_SOURCES := ../../$(HOST_ARCH)/crt0_$(TOOLCHAIN_NAME).c \
                   ../../$(HOST_ARCH)/hardfault_handler.c \
                   ../../$(HOST_ARCH)/host_cm3.c \
                   ../platform_resource.c \
                   ../platform_stdio.c \
                   ../wiced_platform_common.c \
                   ../wiced_apps_common.c \
                   ../wiced_waf_common.c \
                   ../wiced_dct_external_common.c \
                   ../platform_nsclock.c \
                   platform_vector_table.c \
                   platform_init.c \
                   platform_unhandled_isr.c \
                   platform_filesystem.c \
                   platform_mcu_powersave.c \
                   WWD/wwd_platform.c \
                   WWD/BCM439x_packet.c \
                   WAF/waf_platform.c \
                   WWD/wwd_bus.c

#for DCT with crc checking
$(NAME)_COMPONENTS  += utilities/crc

ifdef NO_WIFI
$(NAME)_COMPONENTS += WICED/WWD
ifndef NETWORK
NETWORK := NoNS
$(NAME)_COMPONENTS += WICED/network/NoNS
endif
endif

$(NAME)_INCLUDES += WWD/internal/include
$(NAME)_DEFINES += _HNDRTE_

# These need to be forced into the final ELF since they are not referenced otherwise
$(NAME)_LINK_FILES := ../../$(HOST_ARCH)/crt0_$(TOOLCHAIN_NAME).o \
                      ../../$(HOST_ARCH)/hardfault_handler.o \
                      platform_vector_table.o

$(NAME)_CFLAGS  = $(COMPILER_SPECIFIC_PEDANTIC_CFLAGS) -Wno-pedantic -Wno-strict-prototypes -Wno-missing-prototypes

# Add maximum and default watchdog timeouts to definitions. Warning: Do not change MAX_WATCHDOG_TIMEOUT_SECONDS
MAX_WATCHDOG_TIMEOUT_SECONDS = 22
GLOBAL_DEFINES += MAX_WATCHDOG_TIMEOUT_SECONDS=$(MAX_WATCHDOG_TIMEOUT_SECONDS)

# DCT linker script
DCT_LINK_SCRIPT += $(TOOLCHAIN_NAME)/dct$(LINK_SCRIPT_SUFFIX)

ifeq ($(APP),bootloader)
####################################################################################
# Building bootloader
####################################################################################
DEFAULT_LINK_SCRIPT += $(TOOLCHAIN_NAME)/bootloader$(LINK_SCRIPT_SUFFIX)
GLOBAL_DEFINES      += bootloader_ota

else
ifneq ($(filter sflash_write, $(APP)),)
####################################################################################
# Building sflash_write
####################################################################################

PRE_APP_BUILDS      += bootloader
DEFAULT_LINK_SCRIPT := $(TOOLCHAIN_NAME)/app_no_bootloader$(LINK_SCRIPT_SUFFIX)
GLOBAL_INCLUDES     += WAF/  ../../../../../apps/waf/bootloader/
GLOBAL_DEFINES      += WICED_DISABLE_BOOTLOADER
GLOBAL_DEFINES      += __JTAG_FLASH_WRITER_DATA_BUFFER_SIZE__=16384
ifeq ($(TOOLCHAIN_NAME),IAR)
GLOBAL_LDFLAGS      += --config_def __JTAG_FLASH_WRITER_DATA_BUFFER_SIZE__=16384
endif
$(NAME)_SOURCES     += platform_isr.c
$(NAME)_LINK_FILES  += platform_isr.o

else
ifeq ($(USES_BOOTLOADER_OTA),1)
####################################################################################
# Building standard application to run with bootloader
####################################################################################

PRE_APP_BUILDS      += bootloader
DEFAULT_LINK_SCRIPT := $(TOOLCHAIN_NAME)/app_with_bootloader$(LINK_SCRIPT_SUFFIX)
GLOBAL_INCLUDES     += WAF/  ../../../../../apps/waf/bootloader/
$(NAME)_SOURCES     += platform_isr.c
$(NAME)_LINK_FILES  += platform_isr.o

else
####################################################################################
# Building a WWD application (standalone app without bootloader and DCT)
####################################################################################
DEFAULT_LINK_SCRIPT := $(TOOLCHAIN_NAME)/app_no_bootloader$(LINK_SCRIPT_SUFFIX)
GLOBAL_DEFINES      += WICED_DISABLE_BOOTLOADER
$(NAME)_SOURCES     += platform_isr.c
$(NAME)_LINK_FILES  += platform_isr.o

endif # USES_BOOTLOADER_OTA = 1
endif # APP= sflash_write
endif # APP=bootloader
