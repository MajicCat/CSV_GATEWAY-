#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

SECTOR_COUNT_SCRIPT  := $(TOOLS_ROOT)/text_to_c/sector_count.pl
SECTOR_ADDRESS_SCRIPT  := $(TOOLS_ROOT)/text_to_c/sector_address.pl
SECTOR_NUMBER_SCRIPT  := $(TOOLS_ROOT)/text_to_c/sector_number.pl

APPS_SECTORS_DEFAULT_COUNT	:= 1
CURRENT_SECTOR := $(APPS_START_SECTOR)

APPS_HEADER_DEFINES :=
CURRENT_DEPENDENCY :=
SFLASH_APP_TARGET := waf.sflash_write-NoOS-$(PLATFORM)-$(BUS)

OPENOCD_LOG_FILE ?= build/openocd_log.txt
DOWNLOAD_LOG := >> $(OPENOCD_LOG_FILE)

###############################################################################
# MACRO: BUILD_APPS_RULES
# Creates targets to build a resource file
# the first target converts the text resource file to a C file
# the second target compiles the C resource file into an object file
# $(1) is the name of a resource
# $(2) should be MEM or FILESYSTEM - indication location of resource
define BUILD_APPS_RULES
$(if $($(1)),$(eval $(1)_ENTRY_COUNT := 1),$(eval $(1)_ENTRY_COUNT := 0))
$(if $($(1)),$(eval $(1)_SECTOR_START := $(CURRENT_SECTOR)),$(eval $(1)_SECTOR_START := 0))
$(if $($(1)),$(eval $(1)_SECTOR_COUNT := $(shell $(PERL) $(SECTOR_COUNT_SCRIPT) $($(1)) 0 4096)),$(eval $(1)_SECTOR_COUNT := 0))
$(if $($(1)),$(eval $(1)_SECTOR_ADDRESS := $(shell $(PERL) $(SECTOR_ADDRESS_SCRIPT) $($(1)_SECTOR_START) 4096)),)
$(if $($(1)),$(eval CURRENT_SECTOR := $(shell $(PERL) $(SECTOR_COUNT_SCRIPT) $($(1)) $(CURRENT_SECTOR) 4096)),)
$(if $($(1)),$(eval $(1)_IS_SECURE := $(if $($(1)_SECURE), 1, 0)),	$(eval $(1)_IS_SECURE := 0))
$(eval APPS_HEADER_DEFINES += -D$(1)_ENTRY_COUNT=$($(1)_ENTRY_COUNT))
$(eval APPS_HEADER_DEFINES += -D$(1)_SECTOR_START=$($(1)_SECTOR_START))
$(eval APPS_HEADER_DEFINES += -D$(1)_SECTOR_COUNT=$($(1)_SECTOR_COUNT))
$(eval APPS_HEADER_DEFINES += -D$(1)_IS_SECURE=$($(1)_IS_SECURE))
endef
### end of BUILD_APPS_RULES

###############################################################################
# MACRO: BUILD_APP_DOWNLOAD_DEPENDENCY
define BUILD_APP_DOWNLOAD_DEPENDENCY
$(if $($(1)),$(eval $(1)_DOWNLOAD_DEPENDENCY := $($(1)) $(CURRENT_DEPENDENCY) sflash_write_app display_map_summary $(APPS_LUT_DOWNLOAD_DEP) APPS_LOOKUP_TABLE_RULES  $(LINK_APPS_FILE)),)
$(if $($(1)),$(eval CURRENT_DEPENDENCY += $(1)_DOWNLOAD),)
endef
#### end of BUILD_APP_DOWNLOAD_DEPENDENCY

APPS_DOWNLOADS_DEPENDENCY :=
APPS := FR_APP DCT_IMAGE OTA_APP FILESYSTEM_IMAGE WIFI_FIRMWARE APP0 APP1 APP2
$(foreach APP,$(APPS),$(eval $(if $($(APP)), APPS_HEADER_DEPENDENCY += $($(APP)))))
$(foreach APP,$(APPS),$(eval $(if $($(APP)), APPS_DOWNLOADS_DEPENDENCY += $(APP)_DOWNLOAD)))
$(foreach APP,$(APPS),$(eval $(call BUILD_APP_DOWNLOAD_DEPENDENCY,$(APP))))

LINK_APPS_FILE            :=$(OUTPUT_DIR)/APPS$(LINK_OUTPUT_SUFFIX)
STRIPPED_LINK_APPS_FILE   :=$(LINK_APPS_FILE:$(LINK_OUTPUT_SUFFIX)=.stripped$(LINK_OUTPUT_SUFFIX))
FINAL_APPS_FILE           :=$(LINK_APPS_FILE:$(LINK_OUTPUT_SUFFIX)=$(FINAL_OUTPUT_SUFFIX))
MAP_APPS_FILE             :=$(LINK_APPS_FILE:$(LINK_OUTPUT_SUFFIX)=.map)

# include the correct FLASH positioning
#$(info wiced_apps.mk ota2_support=$(OTA2_SUPPORT))
ifeq (1,$(OTA2_SUPPORT))
# OTA Image Support
include platforms/$(subst .,/,$(PLATFORM))/ota2_image_defines.mk

OTA2_IMAGE_FILE	    :=$(OUTPUT_DIR)/OTA2_image_file
OTA2_IMAGE_CONFIG_FILE     :=$(OTA2_IMAGE_FILE).cfg
OTA2_IMAGE_BIN_FILE	:=$(OTA2_IMAGE_FILE).bin
OTA2_IMAGE_FACTORY_RESET_FILE	    :=$(OUTPUT_DIR)/OTA2_factory_reset_file
OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE     :=$(OTA2_IMAGE_FACTORY_RESET_FILE).cfg
OTA2_IMAGE_FACTORY_RESET_BIN_FILE	:=$(OTA2_IMAGE_FACTORY_RESET_FILE).bin
endif
###############################################################################
# MACRO: BUILD_OTA2_APPS_RULES
# Creates targets to build a resource file
# TODO: the first target converts the text resource file to a C file
# the second target compiles the C resource file into an object file
# $(1) is the name of a resource
# $(2) should be MEM or FILESYSTEM - indication location of resource
define BUILD_OTA2_APPS_RULES
	ifeq (,$(SECTOR_SIZE))
		$(info 	WARNING: FLASH SECTOR_SIZE is not defined, using default of 4096)
	SECTOR_SIZE := 4096
	endif
	$(eval APPS_HEADER_DEFINES := )
	$(eval FR_APP_ENTRY_COUNT       := 0 )
	$(eval FR_APP_IS_SECURE	 	    := 0 )
	$(eval FR_APP_SECTOR_ADDRESS    := 0 )
	$(eval FR_APP_SECTOR_START      := 0 )
	$(eval FR_APP_SECTOR_COUNT      := 0 )

	$(eval APPS_HEADER_DEFINES += -DFR_APP_ENTRY_COUNT=$(FR_APP_ENTRY_COUNT) )
	$(eval APPS_HEADER_DEFINES += -DFR_APP_IS_SECURE=$(FR_APP_IS_SECURE) )
	$(eval APPS_HEADER_DEFINES += -DFR_APP_SECTOR_ADDRESS=$(FR_APP_SECTOR_ADDRESS) )
	$(eval APPS_HEADER_DEFINES += -DFR_APP_SECTOR_START=$(FR_APP_SECTOR_START) )
	$(eval APPS_HEADER_DEFINES += -DFR_APP_SECTOR_COUNT=$(FR_APP_SECTOR_COUNT) )

	$(eval DCT_IMAGE_ENTRY_COUNT    := 1 )
	$(eval DCT_IMAGE_IS_SECURE      := 0 )
	$(info OTA2_IMAGE_CURR_DCT_1_AREA_BASE := $(OTA2_IMAGE_CURR_DCT_1_AREA_BASE) )
	$(eval DCT_IMAGE_SECTOR_ADDRESS := $(OTA2_IMAGE_CURR_DCT_1_AREA_BASE) )
	$(info DCT_IMAGE_SECTOR_ADDRESS := $(DCT_IMAGE_SECTOR_ADDRESS) size:= $(SECTOR_SIZE))
	$(eval DCT_IMAGE_SECTOR_START   := $(shell $(PERL) $(SECTOR_NUMBER_SCRIPT) $(DCT_IMAGE_SECTOR_ADDRESS) $(SECTOR_SIZE)) )
	$(eval DCT_IMAGE_SECTOR_COUNT   := $(shell $(PERL) $(SECTOR_COUNT_SCRIPT) $(subst \,/,$(FINAL_DCT_FILE)) 0 $(SECTOR_SIZE)) )

	$(eval APPS_HEADER_DEFINES += -DDCT_IMAGE_ENTRY_COUNT=$(DCT_IMAGE_ENTRY_COUNT) )
	$(eval APPS_HEADER_DEFINES += -DDCT_IMAGE_IS_SECURE=$(DCT_IMAGE_IS_SECURE) )
	$(eval APPS_HEADER_DEFINES += -DDCT_IMAGE_SECTOR_ADDRESS=$(DCT_IMAGE_SECTOR_ADDRESS) )
	$(eval APPS_HEADER_DEFINES += -DDCT_IMAGE_SECTOR_START=$(DCT_IMAGE_SECTOR_START) )
	$(eval APPS_HEADER_DEFINES += -DDCT_IMAGE_SECTOR_COUNT=$(DCT_IMAGE_SECTOR_COUNT) )

	$(eval OTA_APP_ENTRY_COUNT      := 2 )
	$(eval OTA_APP_IS_SECURE		:= 0 )
	$(eval OTA_APP_SECTOR_ADDRESS   := 0 )
	$(eval OTA_APP_SECTOR_START     := 0 )
	$(eval OTA_APP_SECTOR_COUNT     := 0 )

	$(eval APPS_HEADER_DEFINES += -DOTA_APP_ENTRY_COUNT=$(OTA_APP_ENTRY_COUNT) )
	$(eval APPS_HEADER_DEFINES += -DOTA_APP_IS_SECURE=$(OTA_APP_IS_SECURE) )
	$(eval APPS_HEADER_DEFINES += -DOTA_APP_SECTOR_ADDRESS=$(OTA_APP_SECTOR_ADDRESS) )
	$(eval APPS_HEADER_DEFINES += -DOTA_APP_SECTOR_START=$(OTA_APP_SECTOR_START) )
	$(eval APPS_HEADER_DEFINES += -DOTA_APP_SECTOR_COUNT=$(OTA_APP_SECTOR_COUNT) )

	$(eval FILESYSTEM_IMAGE_ENTRY_COUNT    := 3 )
	$(eval FILESYSTEM_IMAGE_IS_SECURE      := 0 )
	$(eval FILESYSTEM_IMAGE_SECTOR_ADDRESS := $(OTA2_IMAGE_CURR_FS_AREA_BASE) )
	$(eval FILESYSTEM_IMAGE_SECTOR_START   := $(shell $(PERL) $(SECTOR_NUMBER_SCRIPT) $(FILESYSTEM_IMAGE_SECTOR_ADDRESS) $(SECTOR_SIZE)) )
	$(eval FILESYSTEM_IMAGE_SECTOR_COUNT   := $(shell $(PERL) $(SECTOR_COUNT_SCRIPT) $(subst \,/,$(FILESYSTEM_IMAGE)) 0 $(SECTOR_SIZE)) )

	$(eval APPS_HEADER_DEFINES += -DFILESYSTEM_IMAGE_ENTRY_COUNT=$(FILESYSTEM_IMAGE_ENTRY_COUNT) )
	$(eval APPS_HEADER_DEFINES += -DFILESYSTEM_IMAGE_IS_SECURE=$(FILESYSTEM_IMAGE_IS_SECURE) )
	$(eval APPS_HEADER_DEFINES += -DFILESYSTEM_IMAGE_SECTOR_ADDRESS=$(FILESYSTEM_IMAGE_SECTOR_ADDRESS) )
	$(eval APPS_HEADER_DEFINES += -DFILESYSTEM_IMAGE_SECTOR_START=$(FILESYSTEM_IMAGE_SECTOR_START) )
	$(eval APPS_HEADER_DEFINES += -DFILESYSTEM_IMAGE_SECTOR_COUNT=$(FILESYSTEM_IMAGE_SECTOR_COUNT) )

	$(eval WIFI_FIRMWARE_ENTRY_COUNT     := 4 )
	$(eval WIFI_FIRMWARE_IS_SECURE       := 0 )
	$(eval WIFI_FIRMWARE_SECTOR_ADDRESS  := 0 )
	$(eval WIFI_FIRMWARE_SECTOR_START    := 0 )
	$(eval WIFI_FIRMWARE_SECTOR_COUNT    := 0 )

	$(eval APPS_HEADER_DEFINES += -DWIFI_FIRMWARE_ENTRY_COUNT=$(WIFI_FIRMWARE_ENTRY_COUNT) )
	$(eval APPS_HEADER_DEFINES += -DWIFI_FIRMWARE_IS_SECURE=$(WIFI_FIRMWARE_IS_SECURE) )
	$(eval APPS_HEADER_DEFINES += -DWIFI_FIRMWARE_SECTOR_ADDRESS=$(WIFI_FIRMWARE_SECTOR_ADDRESS) )
	$(eval APPS_HEADER_DEFINES += -DWIFI_FIRMWARE_SECTOR_START=$(WIFI_FIRMWARE_SECTOR_START) )
	$(eval APPS_HEADER_DEFINES += -DWIFI_FIRMWARE_SECTOR_COUNT=$(WIFI_FIRMWARE_SECTOR_COUNT) )

	$(eval APP0_ENTRY_COUNT	    := 5 )
	$(eval APP0_IS_SECURE	 	:= 0 )
	$(eval APP0_SECTOR_ADDRESS  := $(OTA2_IMAGE_CURR_APP0_AREA_BASE) )
	$(eval APP0_SECTOR_START	:= $(shell $(PERL) $(SECTOR_NUMBER_SCRIPT) $(APP0_SECTOR_ADDRESS) $(SECTOR_SIZE)) )
	$(eval APP0_SECTOR_COUNT	:= $(shell $(PERL) $(SECTOR_COUNT_SCRIPT) $(subst \,/,$(APP0)) 0 $(SECTOR_SIZE)) )

	$(eval APPS_HEADER_DEFINES += -DAPP0_ENTRY_COUNT=$(APP0_ENTRY_COUNT) )
	$(eval APPS_HEADER_DEFINES += -DAPP0_IS_SECURE=$(APP0_IS_SECURE) )
	$(eval APPS_HEADER_DEFINES += -DAPP0_SECTOR_ADDRESS=$(APP0_SECTOR_ADDRESS) )
	$(eval APPS_HEADER_DEFINES += -DAPP0_SECTOR_START=$(APP0_SECTOR_START) )
	$(eval APPS_HEADER_DEFINES += -DAPP0_SECTOR_COUNT=$(APP0_SECTOR_COUNT) )

	$(eval APP1_ENTRY_COUNT	    := 6 )
	$(eval APP1_IS_SECURE	   	:= 0 )
	$(eval APP1_SECTOR_ADDRESS  := 0 )
	$(eval APP1_SECTOR_START	:= 0 )
	$(eval APP1_SECTOR_COUNT	:= 0 )

	$(eval APPS_HEADER_DEFINES += -DAPP1_ENTRY_COUNT=$(APP1_ENTRY_COUNT) )
	$(eval APPS_HEADER_DEFINES += -DAPP1_IS_SECURE=$(APP1_IS_SECURE) )
	$(eval APPS_HEADER_DEFINES += -DAPP1_SECTOR_ADDRESS=$(APP1_SECTOR_ADDRESS) )
	$(eval APPS_HEADER_DEFINES += -DAPP1_SECTOR_START=$(APP1_SECTOR_START) )
	$(eval APPS_HEADER_DEFINES += -DAPP1_SECTOR_COUNT=$(APP1_SECTOR_COUNT) )

	$(eval APP2_ENTRY_COUNT	    := 7 )
	$(eval APP2_IS_SECURE	   	:= 0 )
	$(eval APP2_SECTOR_ADDRESS  := 0 )
	$(eval APP2_SECTOR_START	:= 0 )
	$(eval APP2_SECTOR_COUNT	:= 0 )

	$(eval APPS_HEADER_DEFINES += -DAPP2_ENTRY_COUNT=$(APP2_ENTRY_COUNT) )
	$(eval APPS_HEADER_DEFINES += -DAPP2_IS_SECURE=$(APP2_IS_SECURE) )
	$(eval APPS_HEADER_DEFINES += -DAPP2_SECTOR_ADDRESS=$(APP2_SECTOR_ADDRESS) )
	$(eval APPS_HEADER_DEFINES += -DAPP2_SECTOR_START=$(APP2_SECTOR_START) )
	$(eval APPS_HEADER_DEFINES += -DAPP2_SECTOR_COUNT=$(APP2_SECTOR_COUNT) )

	$(eval APP_LUT_SECTOR_COUNT = $(shell $(PERL) $(SECTOR_COUNT_SCRIPT) $(subst \,/,$(FINAL_APPS_FILE)) 0 $(SECTOR_SIZE)) )

	$(eval APPS_HEADER_DEFINES += -DAPP_LUT_SECTOR_COUNT=$(APP_LUT_SECTOR_COUNT) )
	$(eval APPS_HEADER_DEFINES += -DOTA2_SUPPORT=1 )

endef	#### End of BUILD_OTA2_APPS_RULES


.PHONY: DOWNLOAD_APPS_HEADER FILESYSTEM_IMAGE_DOWNLOAD APP0_DOWNLOAD APPS_LOOKUP_TABLE_RULES

APPS_LOOKUP_TABLE_RULES: display_map_summary $(APPS_HEADER_DEPENDENCY)
ifneq (1,$(OTA2_SUPPORT))
	$(foreach APP,$(APPS),$(eval $(call BUILD_APPS_RULES,$(APP))))
else
	$(eval $(call BUILD_OTA2_APPS_RULES))
endif	# OTA2_SUPPORT

$(LINK_APPS_FILE): display_map_summary $(SOURCE_ROOT)WICED/platform/MCU/wiced_apps_lut.c APPS_LOOKUP_TABLE_RULES
	$(QUIET)$(ECHO) Building apps lookup table
	$(QUIET)$(CC) $(CPU_CFLAGS) $(COMPILER_SPECIFIC_COMP_ONLY_FLAG)  $(SOURCE_ROOT)WICED/platform/MCU/wiced_apps_lut.c $(APPS_HEADER_DEFINES) $(WICED_SDK_DEFINES) $(WICED_SDK_INCLUDES) $(COMPILER_SPECIFIC_DEBUG_CFLAGS)  $(call ADD_COMPILER_SPECIFIC_STANDARD_CFLAGS, ) -I$(OUTPUT_DIR) -I$(SOURCE_ROOT). -o $(OUTPUT_DIR)/apps_header.o $(COMPILER_SPECIFIC_STDOUT_REDIRECT)
	$(QUIET)$(LINKER) $(WICED_SDK_LDFLAGS) $(WICED_SDK_DCT_LINK_CMD) $(call COMPILER_SPECIFIC_LINK_MAP,$(MAP_APPS_FILE)) -o $@  $(OUTPUT_DIR)/apps_header.o $(COMPILER_SPECIFIC_STDOUT_REDIRECT)

$(STRIPPED_LINK_APPS_FILE): $(LINK_APPS_FILE)
	$(QUIET)$(STRIP) -o $@ $(STRIPFLAGS) $<

$(FINAL_APPS_FILE): $(STRIPPED_LINK_APPS_FILE)
	$(QUIET)$(OBJCOPY) -O binary -R .eh_frame -R .init -R .fini -R .comment -R .ARM.attributes $< $@
#	$(EVAL APP_LUT_SECTOR_COUNT = $(shell $(PERL) $(SECTOR_COUNT_SCRIPT) $(subst \,/,$(FINAL_APPS_FILE)) 0 4096))

# THIS IS A DODGY HACK AS THE DEFINE MACRO IS NOT WORKING!!!
ifneq ($(FR_APP),)
FR_APP_DOWNLOAD:  $(FR_APP_DOWNLOAD_DEPENDENCY)
	$(QUIET)$(ECHO) Downloading FR_APP $(FR_APP) at sector $(FR_APP_SECTOR_START) address $(FR_APP_SECTOR_ADDRESS)...
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(FR_APP) $(FR_APP_SECTOR_ADDRESS) $(SFLASH_APP_PLATFROM_BUS) 0 $(SFLASH_APP_BCM4390)" -c shutdown $(DOWNLOAD_LOG) 2>&1
endif

ifneq ($(OTA_APP),)
OTA_APP_DOWNLOAD:  $(OTA_APP_DOWNLOAD_DEPENDENCY)
	$(QUIET)$(ECHO) Downloading OTA_APP $(OTA_APP) at sector $(OTA_APP_SECTOR_START) address $(OTA_APP_SECTOR_ADDRESS)...
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(OTA_APP) $(OTA_APP_SECTOR_ADDRESS) $(SFLASH_APP_PLATFROM_BUS) 0 $(SFLASH_APP_BCM4390)" -c shutdown $(DOWNLOAD_LOG) 2>&1
endif

ifneq ($(DCT_IMAGE),)
DCT_IMAGE_DOWNLOAD:  $(DCT_IMAGE_DOWNLOAD_DEPENDENCY)
	$(QUIET)$(ECHO) Downloading DCT_IMAGE $(DCT_IMAGE) at sector $(DCT_IMAGE_SECTOR_START) offset $(DCT_IMAGE_SECTOR_ADDRESS) size $(DCT_IMAGE_SECTOR_COUNT)...
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(DCT_IMAGE) $(DCT_IMAGE_SECTOR_ADDRESS) $(SFLASH_APP_PLATFROM_BUS) 0 $(SFLASH_APP_BCM4390)" -c shutdown $(DOWNLOAD_LOG) 2>&1
endif

ifneq ($(FILESYSTEM_IMAGE),)
FILESYSTEM_IMAGE_DOWNLOAD: $(FILESYSTEM_IMAGE_DOWNLOAD_DEPENDENCY)
	$(QUIET)$(ECHO) Downloading resources filesystem ... $(FILESYSTEM_IMAGE) at sector $(FILESYSTEM_IMAGE_SECTOR_START) size $(FILESYSTEM_IMAGE_SECTOR_COUNT)...
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(FILESYSTEM_IMAGE) $(FILESYSTEM_IMAGE_SECTOR_ADDRESS) $(SFLASH_APP_PLATFROM_BUS) 0 $(SFLASH_APP_BCM4390)" -c shutdown $(DOWNLOAD_LOG) 2>&1
endif

ifneq ($(WIFI_FIRMWARE),)
WIFI_FIRMWARE_DOWNLOAD: $(WIFI_FIRMWARE_DOWNLOAD_DEPENDENCY)
	$(QUIET)$(ECHO) Downloading WIFI_FIRMWARE ... at sector $(WIFI_FIRMWARE_SECTOR_START) size $(WIFI_FIRMWARE_SECTOR_COUNT)...
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(WIFI_FIRMWARE) $(WIFI_FIRMWARE_SECTOR_ADDRESS) $(SFLASH_APP_PLATFROM_BUS) 0 $(SFLASH_APP_BCM4390)" -c shutdown $(DOWNLOAD_LOG) 2>&1
endif

ifneq ($(APP0),)
APP0_DOWNLOAD:  $(APP0_DOWNLOAD_DEPENDENCY)
	$(QUIET)$(ECHO) Downloading APP0 $(APP0) @ sector $(APP0_SECTOR_START) address $(APP0_SECTOR_ADDRESS) size $(APP0_SECTOR_COUNT)...
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(APP0) $(APP0_SECTOR_ADDRESS) $(SFLASH_APP_PLATFROM_BUS) 0 $(SFLASH_APP_BCM4390)" -c shutdown $(DOWNLOAD_LOG) 2>&1
endif

ifneq ($(APP1),)
APP1_DOWNLOAD:  $(APP1_DOWNLOAD_DEPENDENCY)
	$(QUIET)$(ECHO) Downloading APP1 $(APP1) at sector $(APP1_SECTOR_START) address $(APP1_SECTOR_ADDRESS) size: $(APP1_SECTOR_COUNT)...
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(APP1) $(APP1_SECTOR_ADDRESS) $(SFLASH_APP_PLATFROM_BUS) 0 $(SFLASH_APP_BCM4390)" -c shutdown $(DOWNLOAD_LOG) 2>&1
endif

ifneq ($(APP2),)
APP2_DOWNLOAD:  $(APP2_DOWNLOAD_DEPENDENCY)
	$(QUIET)$(ECHO) Downloading APP2 $(APP2) at sector $(APP2_SECTOR_START) address $(APP2_SECTOR_ADDRESS) size: $(APP1_SECTOR_COUNT)...
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(APP2) $(APP2_SECTOR_ADDRESS) $(SFLASH_APP_PLATFROM_BUS) 0 $(SFLASH_APP_BCM4390)" -c shutdown $(DOWNLOAD_LOG) 2>&1
endif

# If Downloading is required, then the Serial Flash app need to be built
sflash_write_app:
	$(QUIET)$(ECHO) Building Serial Flash Loader App
	$(QUIET)$(MAKE) -r -f $(SOURCE_ROOT)Makefile $(SFLASH_APP_TARGET) -I$(OUTPUT_DIR)  SFLASH= EXTERNAL_WICED_GLOBAL_DEFINES=$(EXTERNAL_WICED_GLOBAL_DEFINES) SUB_BUILD=sflash_app $(SFLASH_REDIRECT)
	$(QUIET)$(ECHO) Finished Building Serial Flash Loader App
	$(QUIET)$(ECHO_BLANK_LINE)

APPS_LUT_DOWNLOAD: sflash_write_app $(FINAL_APPS_FILE) $(APPS_DOWNLOADS_DEPENDENCY) $(APPS_LUT_DOWNLOAD_DEP) APPS_LOOKUP_TABLE_RULES
	$(QUIET)$(ECHO) Downloading apps lookup table in wiced_apps.mk ... $(FINAL_APPS_FILE) @ $(APPS_LUT_HEADER_LOC) size $(APP_LUT_SECTOR_COUNT)
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(FINAL_APPS_FILE) $(APPS_LUT_HEADER_LOC) $(SFLASH_APP_PLATFROM_BUS) 0 $(SFLASH_APP_BCM4390)" -c shutdown $(DOWNLOAD_LOG) 2>&1

# TODO: add compression to individual components before building OTA Image - here or in the builder?
# If OTA Image is required, then build the OTA Image maker

ota2_image: $(FINAL_APPS_FILE) APPS_LUT_DOWNLOAD
	$(QUIET)$(ECHO) Building OTA Image Info File $(OTA2_IMAGE_CONFIG_FILE)
	$(QUIET)$(call WRITE_FILE_CREATE, $(OTA2_IMAGE_CONFIG_FILE) ,FACTORY_RESET=0x00)
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_CONFIG_FILE) ,MAJOR_VERSION=0x00)
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_CONFIG_FILE) ,MINOR_VERSION=0x00)
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_CONFIG_FILE) ,PLATFORM_NAME=$(PLATFORM))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_CONFIG_FILE) ,APPS_LUT_LOC=$(APPS_LUT_HEADER_LOC))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_CONFIG_FILE) ,APPS_LUT_FILE=$(call CONV_SLASHES,$(FINAL_APPS_FILE)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_CONFIG_FILE) ,FR_APP_LOC=$(FR_APP_SECTOR_ADDRESS))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_CONFIG_FILE) ,FR_APP_FILE=$(call CONV_SLASHES,$(FR_APP)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_CONFIG_FILE) ,DCT_LOC=$(SFLASH_DCT_LOC))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_CONFIG_FILE) ,DCT_FILE=$(call CONV_SLASHES,$(FINAL_DCT_FILE)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_CONFIG_FILE) ,OTA_APP_LOC=$(OTA_APP_SECTOR_ADDRESS))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_CONFIG_FILE) ,OTA_APP_FILE=$(call CONV_SLASHES,$(OTA_APP)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_CONFIG_FILE) ,FILESYSTEM_LOC=$(FILESYSTEM_IMAGE_SECTOR_ADDRESS))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_CONFIG_FILE) ,FILESYSTEM_FILE=$(call CONV_SLASHES,$(FS_IMAGE)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_CONFIG_FILE) ,WIFI_FIRMWARE_LOC=$(WIFI_FIRMWARE_SECTOR_ADDRESS))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_CONFIG_FILE) ,WIFI_FIRMWARE_FILE=$(call CONV_SLASHES,$(WIFI_FIRMWARE)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_CONFIG_FILE) ,APPLICATION_0_LOC=$(APP0_SECTOR_ADDRESS))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_CONFIG_FILE) ,APPLICATION_0_FILE=$(call CONV_SLASHES,$(APP0)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_CONFIG_FILE) ,APPLICATION_1_LOC=$(APP1_SECTOR_ADDRESS))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_CONFIG_FILE) ,APPLICATION_1_FILE=$(call CONV_SLASHES,$(APP1)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_CONFIG_FILE) ,APPLICATION_2_LOC=$(APP2_SECTOR_ADDRESS))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_CONFIG_FILE) ,APPLICATION_2_FILE=$(call CONV_SLASHES,$(APP2)))
	$(QUIET)$(ECHO) Building OTA2 Image $(OTA2_IMAGE_BIN_FILE)
	$(COMMON_TOOLS_PATH)mk_wiced_ota2_image32 $(OTA2_IMAGE_CONFIG_FILE) $(OTA2_IMAGE_BIN_FILE) -v $(VERBOSE)
	$(QUIET)$(ECHO) Building OTA2 Image Done

ota2_download: ota2_image
	$(QUIET)$(ECHO) Downloading OTA2 Image $(OTA2_IMAGE_BIN_FILE) at $(OTA2_IMAGE_STAGING_AREA_BASE) ...
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(OTA2_IMAGE_BIN_FILE) $(OTA2_IMAGE_STAGING_AREA_BASE) $(SFLASH_APP_PLATFROM_BUS) 0 $(SFLASH_APP_BCM4390)" -c shutdown $(DOWNLOAD_LOG) 2>&1
	$(QUIET)$(ECHO) Downloading OTA2 Image Done

ota2_factory_image: APPS_LUT_DOWNLOAD
	$(QUIET)$(ECHO) Building OTA Factory Reset Image Info File $(OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE)
	$(QUIET)$(call WRITE_FILE_CREATE, $(OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE) ,FACTORY_RESET=0x01)
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE) ,MAJOR_VERSION=0x00)
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE) ,MINOR_VERSION=0x00)
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE) ,PLATFORM_NAME=$(PLATFORM))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE) ,APPS_LUT_LOC=$(APPS_LUT_HEADER_LOC))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE) ,APPS_LUT_FILE=$(call CONV_SLASHES,$(FINAL_APPS_FILE)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE) ,FR_APP_LOC=$(FR_APP_SECTOR_ADDRESS))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE) ,FR_APP_FILE=$(call CONV_SLASHES,$(FR_APP)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE) ,DCT_LOC=$(SFLASH_DCT_LOC))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE) ,DCT_FILE=$(call CONV_SLASHES,$(FINAL_DCT_FILE)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE) ,OTA_APP_LOC=$(OTA_APP_SECTOR_ADDRESS))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE) ,OTA_APP_FILE=$(call CONV_SLASHES,$(OTA_APP)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE) ,FILESYSTEM_LOC=$(FILESYSTEM_IMAGE_SECTOR_ADDRESS))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE) ,FILESYSTEM_FILE=$(call CONV_SLASHES,$(FS_IMAGE)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE) ,WIFI_FIRMWARE_LOC=$(WIFI_FIRMWARE_SECTOR_ADDRESS))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE) ,WIFI_FIRMWARE_FILE=$(call CONV_SLASHES,$(WIFI_FIRMWARE)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE) ,APPLICATION_0_LOC=$(APP0_SECTOR_ADDRESS))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE) ,APPLICATION_0_FILE=$(call CONV_SLASHES,$(APP0)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE) ,APPLICATION_1_LOC=$(APP1_SECTOR_ADDRESS))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE) ,APPLICATION_1_FILE=$(call CONV_SLASHES,$(APP1)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE) ,APPLICATION_2_LOC=$(APP2_SECTOR_ADDRESS))
	$(QUIET)$(call WRITE_FILE_APPEND, $(OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE) ,APPLICATION_2_FILE=$(call CONV_SLASHES,$(APP2)))
	$(QUIET)$(ECHO) Building OTA2 Factory Reset Image $(OTA2_IMAGE_FACTORY_RESET_BIN_FILE)
	$(COMMON_TOOLS_PATH)mk_wiced_ota2_image32 $(OTA2_IMAGE_FACTORY_RESET_CONFIG_FILE) $(OTA2_IMAGE_FACTORY_RESET_BIN_FILE) -v $(VERBOSE)
	$(QUIET)$(ECHO) Building OTA2 Factory Reset Image Done

ota2_factory_download: ota2_download ota2_factory_image
	$(QUIET)$(ECHO) Downloading OTA2 Factory Reset Image $(OTA2_IMAGE_FACTORY_RESET_BIN_FILE) at $(OTA2_IMAGE_FACTORY_RESET_AREA_BASE) ...
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(OTA2_IMAGE_FACTORY_RESET_BIN_FILE) $(OTA2_IMAGE_FACTORY_RESET_AREA_BASE) $(SFLASH_APP_PLATFROM_BUS) 0 $(SFLASH_APP_BCM4390)" -c shutdown $(DOWNLOAD_LOG) 2>&1
	$(QUIET)$(ECHO) Downloading OTA2 Factory Reset Image Done
