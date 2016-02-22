#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

NAME = SAM4S

# Host architecture is ARM Cortex M3
HOST_ARCH := ARM_CM4

# Host MCU alias for OpenOCD
HOST_OPENOCD := at91sam4sXX

$(NAME)_NEVER_OPTIMISE := 1

GLOBAL_INCLUDES := . \
                   .. \
                   ../.. \
                   ../../include \
                   ../../$(HOST_ARCH) \
                   ../../$(HOST_ARCH)/CMSIS \
                   ../../$(TOOLCHAIN_NAME) \
                   peripherals \
                   WAF

# Global defines
GLOBAL_DEFINES  := USE_STDPERIPH_DRIVER
GLOBAL_DEFINES  += _$(HOST_MCU_PART_NUMBER)_
GLOBAL_DEFINES  += _STM3x_
GLOBAL_DEFINES  += _STM32x_
GLOBAL_DEFINES	+= DCT_HEADER_ALIGN_SIZE=512

# Global flags
GLOBAL_CFLAGS   += $$(CPU_CFLAGS)    $$(ENDIAN_CFLAGS_LITTLE)
GLOBAL_CXXFLAGS += $$(CPU_CXXFLAGS)  $$(ENDIAN_CXXFLAGS_LITTLE)
GLOBAL_ASMFLAGS += $$(CPU_ASMFLAGS)  $$(ENDIAN_ASMFLAGS_LITTLE)
GLOBAL_LDFLAGS  += $$(CPU_LDFLAGS)   $$(ENDIAN_LDFLAGS_LITTLE)

ifeq ($(TOOLCHAIN_NAME),GCC)
GLOBAL_LDFLAGS  += -nostartfiles
GLOBAL_LDFLAGS  += -Wl,--defsym,__STACKSIZE__=$$($(RTOS)_START_STACK)
GLOBAL_LDFLAGS  += -L ./WICED/platform/MCU/$(NAME)/$(TOOLCHAIN_NAME) \
                   -L ./WICED/platform/MCU/$(NAME)/$(TOOLCHAIN_NAME)/$(HOST_MCU_VARIANT)
else
ifeq ($(TOOLCHAIN_NAME),IAR)
GLOBAL_LDFLAGS  += --config_def __STACKSIZE__=$$($(RTOS)_START_STACK)
endif
endif

# Components
$(NAME)_COMPONENTS += $(TOOLCHAIN_NAME)
$(NAME)_COMPONENTS += MCU/SAM4S/peripherals
$(NAME)_COMPONENTS += utilities/ring_buffer

# Source files
$(NAME)_SOURCES := ../../$(HOST_ARCH)/crt0_$(TOOLCHAIN_NAME).c \
                   ../../$(HOST_ARCH)/hardfault_handler.c \
                   ../../$(HOST_ARCH)/host_cm4.c \
                   ../platform_resource.c \
                   ../platform_stdio.c \
                   ../wiced_platform_common.c \
                   ../wwd_platform_separate_mcu.c \
                   ../wwd_resources.c \
                   ../wiced_apps_common.c	\
                   ../wiced_waf_common.c	\
                   ../wiced_dct_internal_common.c \
                   ../platform_nsclock.c \
                   platform_vector_table.c \
                   platform_init.c \
                   platform_unhandled_isr.c \
                   WWD/hsmci_pdc.c \
                   WWD/sdmmc.c	\
                   WAF/waf_platform.c \

#for DCT with crc checking
$(NAME)_COMPONENTS  += utilities/crc

ifdef PLATFORM_SUPPORTS_BUTTONS
$(NAME)_SOURCES += ../platform_button.c
endif

ifndef NO_WIFI
$(NAME)_SOURCES += WWD/wwd_platform.c \
                   WWD/wwd_$(BUS).c
endif

# These need to be forced into the final ELF since they are not referenced otherwise
$(NAME)_LINK_FILES := ../../$(HOST_ARCH)/crt0_$(TOOLCHAIN_NAME).o \
                      ../../$(HOST_ARCH)/hardfault_handler.o \
                      platform_vector_table.o

#$(NAME)_CFLAGS = $(COMPILER_SPECIFIC_PEDANTIC_CFLAGS)

# Add maximum and default watchdog timeouts to definitions. Warning: Do not change MAX_WATCHDOG_TIMEOUT_SECONDS
MAX_WATCHDOG_TIMEOUT_SECONDS = 16
GLOBAL_DEFINES += MAX_WATCHDOG_TIMEOUT_SECONDS=$(MAX_WATCHDOG_TIMEOUT_SECONDS)

# DCT linker script
DCT_LINK_SCRIPT += $(TOOLCHAIN_NAME)/$(HOST_MCU_VARIANT)/dct$(LINK_SCRIPT_SUFFIX)

ifeq ($(APP),bootloader)
####################################################################################
# Building bootloader
####################################################################################

DEFAULT_LINK_SCRIPT += $(TOOLCHAIN_NAME)/bootloader$(LINK_SCRIPT_SUFFIX)
#$(NAME)_SOURCES     += WAF/waf_platform.c
#$(NAME)_LINK_FILES  += WAF/waf_platform.o

else
ifneq ($(filter sflash_write, $(APP)),)
####################################################################################
# Building sflash_write
####################################################################################

PRE_APP_BUILDS      += bootloader
DEFAULT_LINK_SCRIPT := $(TOOLCHAIN_NAME)/app_ram$(LINK_SCRIPT_SUFFIX)
GLOBAL_INCLUDES     += WAF ../../../../../apps/waf/bootloader/
GLOBAL_DEFINES      += __JTAG_FLASH_WRITER_DATA_BUFFER_SIZE__=16384
ifeq ($(TOOLCHAIN_NAME),IAR)
GLOBAL_LDFLAGS      += --config_def __JTAG_FLASH_WRITER_DATA_BUFFER_SIZE__=16384
endif

else
ifeq ($(USES_BOOTLOADER_OTA),1)
####################################################################################
# Building standard application to run with bootloader
####################################################################################

PRE_APP_BUILDS      += bootloader
DEFAULT_LINK_SCRIPT := $(TOOLCHAIN_NAME)/app_with_bootloader$(LINK_SCRIPT_SUFFIX)
GLOBAL_INCLUDES     += WAF ../../../../../apps/waf/bootloader/

else
####################################################################################
# Building a WWD application (standalone app without bootloader and DCT)
####################################################################################

DEFAULT_LINK_SCRIPT := $(TOOLCHAIN_NAME)/app_no_bootloader$(LINK_SCRIPT_SUFFIX)
GLOBAL_DEFINES      += WICED_DISABLE_BOOTLOADER

endif # USES_BOOTLOADER_OTA = 1
endif # APP= sflash_write
endif # APP=bootloader

