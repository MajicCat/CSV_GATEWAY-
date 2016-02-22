#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

ARM_GNU_ARCH_LIST :=

THUMB_GNU_ARCH_LIST := ARM_CM0 \
                       ARM_CM3 \
                       ARM_CM4 \
                       ARM_CR4


ifneq ($(filter $(HOST_ARCH), $(THUMB_GNU_ARCH_LIST) $(ARM_GNU_ARCH_LIST)),)

ifneq ($(filter $(HOST_ARCH), $(ARM_GNU_ARCH_LIST)),)
HOST_INSTRUCTION_SET := ARM
else
HOST_INSTRUCTION_SET := THUMB
endif

TOOLCHAIN_PREFIX  := arm-none-eabi-

ifeq ($(HOST_OS),Win32)
################
# Windows settings
################


TOOLCHAIN_PATH    := $(TOOLS_ROOT)/ARM_GNU/bin/Win32/
GDBINIT_STRING     = shell start /B $(OPENOCD_FULL_NAME) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD)_gdb_jtag.cfg -l $(OPENOCD_LOG_FILE)
GDB_COMMAND        = cmd /C $(call CONV_SLASHES, $(TOOLCHAIN_PATH))$(TOOLCHAIN_PREFIX)gdb$(EXECUTABLE_SUFFIX)

else  # Win32
ifeq ($(HOST_OS),Linux32)
################
# Linux 32-bit settings
################


TOOLCHAIN_PATH    := $(TOOLS_ROOT)/ARM_GNU/bin/Linux32/
GDBINIT_STRING     = 'shell $(COMMON_TOOLS_PATH)dash -c "trap \\"\\" 2;$(OPENOCD_FULL_NAME) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD)_gdb_jtag.cfg -l $(OPENOCD_LOG_FILE) &"'
GDB_COMMAND        = "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)gdb"

else # Linux32
ifeq ($(HOST_OS),Linux64)
################
# Linux 64-bit settings
################

TOOLCHAIN_PATH    := $(TOOLS_ROOT)/ARM_GNU/bin/Linux64/
GDBINIT_STRING     = 'shell $(COMMON_TOOLS_PATH)dash -c "trap \\"\\" 2;$(OPENOCD_FULL_NAME) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD)_gdb_jtag.cfg -l $(OPENOCD_LOG_FILE) &"'
GDB_COMMAND        = "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)gdb"

else # Linux64
ifeq ($(HOST_OS),OSX)
################
# OSX settings
################

TOOLCHAIN_PATH    := $(TOOLS_ROOT)/ARM_GNU/bin/OSX/
GDBINIT_STRING     = 'shell $(COMMON_TOOLS_PATH)dash -c "trap \\"\\" 2;$(OPENOCD_FULL_NAME) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD)_gdb_jtag.cfg -l $(OPENOCD_LOG_FILE) &"'
GDB_COMMAND        = "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)gdb"

else # OSX

$(error incorrect 'make' used ($(MAKE)) - please use:  (Windows) .\make.exe <target_string>    (OS X, Linux) ./make <target_string>)
endif # OSX
endif # Linux64
endif # Linux32
endif # Win32


# Notes on C++ options:
# The next two CXXFLAGS reduce the size of C++ code by removing unneeded
# features. For example, these flags reduced the size of a console app build
# (with C++ iperf) from 604716kB of flash to 577580kB of flash and 46756kB of
# RAM to 46680kB of RAM.
#
# -fno-rtti
# Disable generation of information about every class with virtual functions for
# use by the C++ runtime type identification features (dynamic_cast and typeid).
# Disabling RTTI eliminates several KB of support code from the C++ runtime
# library (assuming that you don't link with code that uses RTTI).
#
# -fno-exceptions
# Stop generating extra code needed to propagate exceptions, which can produce
# significant data size overhead. Disabling exception handling eliminates
# several KB of support code from the C++ runtime library (assuming that you
# don't link external code that uses exception handling).

CC      := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)gcc$(EXECUTABLE_SUFFIX)" -isystem $(TOOLCHAIN_PATH)../../include -isystem $(TOOLCHAIN_PATH)../../lib/include -isystem $(TOOLCHAIN_PATH)../../lib/include-fixed
CXX     := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)g++$(EXECUTABLE_SUFFIX)"
AS      := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)as$(EXECUTABLE_SUFFIX)"
AR      := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)ar$(EXECUTABLE_SUFFIX)"

ADD_COMPILER_SPECIFIC_STANDARD_CFLAGS   = $(1) -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11                $(if $(filter yes,$(BROADCOM_INTERNAL) $(TESTER)),-Werror)
ADD_COMPILER_SPECIFIC_STANDARD_CXXFLAGS = $(1) -Wall -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions $(if $(filter yes,$(BROADCOM_INTERNAL) $(TESTER)),-Werror)
ADD_COMPILER_SPECIFIC_STANDARD_ADMFLAGS = $(1)
COMPILER_SPECIFIC_OPTIMIZED_CFLAGS    := -Os
COMPILER_SPECIFIC_UNOPTIMIZED_CFLAGS  := -O0
COMPILER_SPECIFIC_PEDANTIC_CFLAGS  := $(COMPILER_SPECIFIC_STANDARD_CFLAGS) -Werror -Wstrict-prototypes  -W -Wshadow  -Wwrite-strings -pedantic -std=c99 -U__STRICT_ANSI__ -Wconversion -Wextra -Wdeclaration-after-statement -Wconversion -Waddress -Wlogical-op -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wmissing-field-initializers -Wdouble-promotion -Wswitch-enum -Wswitch-default -Wuninitialized -Wunknown-pragmas -Wfloat-equal  -Wundef  -Wshadow # -Wcast-qual -Wtraditional -Wtraditional-conversion
COMPILER_SPECIFIC_ARFLAGS_CREATE   := -rcs
COMPILER_SPECIFIC_ARFLAGS_ADD      := -rcs
COMPILER_SPECIFIC_ARFLAGS_VERBOSE  := -v
COMPILER_SPECIFIC_DEBUG_CFLAGS     := -DDEBUG -ggdb $(COMPILER_SPECIFIC_UNOPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_DEBUG_CXXFLAGS   := -DDEBUG -ggdb $(COMPILER_SPECIFIC_UNOPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_DEBUG_ASFLAGS    := --defsym DEBUG=1 -ggdb
COMPILER_SPECIFIC_DEBUG_LDFLAGS    := -Wl,--gc-sections -Wl,--cref
COMPILER_SPECIFIC_RELEASE_CFLAGS   := -DNDEBUG -ggdb $(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_RELEASE_CXXFLAGS := -DNDEBUG -ggdb $(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_RELEASE_ASFLAGS  := -ggdb
COMPILER_SPECIFIC_RELEASE_LDFLAGS  := -Wl,--gc-sections -Wl,$(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS) -Wl,--cref
COMPILER_SPECIFIC_DEPS_FLAG        := -MD
COMPILER_SPECIFIC_COMP_ONLY_FLAG   := -c
COMPILER_SPECIFIC_LINK_MAP         =  -Wl,-Map,$(1)
COMPILER_SPECIFIC_LINK_FILES       =  -Wl,--start-group $(1) -Wl,--end-group
COMPILER_SPECIFIC_LINK_SCRIPT_DEFINE_OPTION = -Wl$(COMMA)-T
COMPILER_SPECIFIC_LINK_SCRIPT      =  $(addprefix -Wl$(COMMA)-T ,$(1))
LINKER                             := $(CXX) --static -Wl,-static -Wl,--warn-common
LINK_SCRIPT_SUFFIX                 := .ld
TOOLCHAIN_NAME := GCC
OPTIONS_IN_FILE_OPTION    := @

# Chip specific flags for GCC
ifeq ($(HOST_ARCH),ARM_CM4)
CPU_CFLAGS     := -mthumb -mcpu=cortex-m4
CPU_CXXFLAGS   := -mthumb -mcpu=cortex-m4
CPU_ASMFLAGS   := -mcpu=cortex-m4 -mfpu=softvfp
CPU_LDFLAGS    := -mthumb -mcpu=cortex-m4 -Wl,-A,thumb
endif

ifeq ($(HOST_ARCH),ARM_CM3)
CPU_CFLAGS   := -mthumb -mcpu=cortex-m3
CPU_CXXFLAGS := -mthumb -mcpu=cortex-m3
CPU_ASMFLAGS := -mcpu=cortex-m3 -mfpu=softvfp
CPU_LDFLAGS  := -mthumb -mcpu=cortex-m3 -Wl,-A,thumb
endif

ifeq ($(HOST_ARCH),ARM_CM0)
CPU_CFLAGS   := -mthumb -mcpu=cortex-m0
CPU_CXXFLAGS := -mthumb -mcpu=cortex-m0
CPU_ASMFLAGS := -mcpu=cortex-m0 -mthumb
CPU_LDFLAGS  := -mthumb -mcpu=cortex-m0 -Wl,-A,thumb
endif

ifeq ($(HOST_ARCH),ARM_CR4)
CPU_BASE_FLAGS     := -mcpu=cortex-r4 -mthumb-interwork
CPU_COMPILER_FLAGS := $(CPU_BASE_FLAGS) -mthumb -fno-builtin-memcpy
CPU_CFLAGS         := $(CPU_COMPILER_FLAGS)
CPU_CXXFLAGS       := $(CPU_COMPILER_FLAGS)
CPU_ASMFLAGS       := $(CPU_BASE_FLAGS)
CPU_LDFLAGS        := -mthumb $(CPU_BASE_FLAGS) -Wl,-A,thumb -Wl,-z,max-page-size=0x10 -Wl,-z,common-page-size=0x10
endif


ENDIAN_CFLAGS_LITTLE   := -mlittle-endian
ENDIAN_CXXFLAGS_LITTLE := -mlittle-endian
ENDIAN_ASMFLAGS_LITTLE :=
ENDIAN_LDFLAGS_LITTLE  := -mlittle-endian

# $(1) is map file, $(2) is CSV output file
COMPILER_SPECIFIC_MAPFILE_TO_CSV = $(PERL) $(MAPFILE_PARSER) -a $(1) > $(2)

MAPFILE_PARSER            :=$(TOOLS_ROOT)/mapfile_parser/map_parse_gcc.pl

# $(1) is map file, $(2) is CSV output file
COMPILER_SPECIFIC_MAPFILE_DISPLAY_SUMMARY = $(PERL) $(MAPFILE_PARSER) $(1)

OBJDUMP := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)objdump$(EXECUTABLE_SUFFIX)"
OBJCOPY := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)objcopy$(EXECUTABLE_SUFFIX)"
STRIP   := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)strip$(EXECUTABLE_SUFFIX)"
NM      := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)nm$(EXECUTABLE_SUFFIX)"

LINK_OUTPUT_SUFFIX  :=.elf
FINAL_OUTPUT_SUFFIX :=.bin


endif #ifneq ($(filter $(HOST_ARCH), ARM_CM3 ARM_CM4),)
