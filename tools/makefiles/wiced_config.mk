#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#

include $(MAKEFILES_PATH)/wiced_toolchain_common.mk
CONFIG_FILE_DIR := build/$(CLEANED_BUILD_STRING)
CONFIG_FILE := $(CONFIG_FILE_DIR)/config.mk

COMPONENT_DIRECTORIES := platforms \
                         WICED/platform \
                         WICED/network \
                         apps \
                         WICED/RTOS \
                         WICED/security \
                         libraries \
                         .


WICED_COMPONENT_URL :=
FETCH_COMPONENT := $(PERL) $(MAKEFILES_PATH)/fetch_component.pl



WICED_SDK_VERSION ?= 3.5.2

##################################
# Macros
##################################

# $(1) is component
GET_BARE_LOCATION =$(patsubst $(call ESCAPE_BACKSLASHES,$(SOURCE_ROOT))%,%,$(strip $($(1)_LOCATION)))

#####################################################################################
# Macro PROCESS_COMPONENT
# $(1) is the list of components left to process. $(COMP) is set as the first element in the list
define PROCESS_COMPONENT
$(eval COMP := $(word 1,$(1)))
$(eval COMP_LOCATION := $(subst .,/,$(COMP)))
$(eval COMP_MAKEFILE_NAME := $(notdir $(COMP_LOCATION)))
# Find the component makefile in directory list
$(eval TEMP_MAKEFILE := $(strip $(wildcard $(foreach dir, $(addprefix $(SOURCE_ROOT),$(COMPONENT_DIRECTORIES)), $(dir)/$(COMP_LOCATION)/$(COMP_MAKEFILE_NAME).mk))))

# Check if component makefile was found - if not try downloading it and re-doing the makefile search
$(if $(TEMP_MAKEFILE),,$(call DOWNLOAD_COMPONENT,$(COMP)) $(eval TEMP_MAKEFILE := $(strip $(wildcard $(foreach dir, $(addprefix $(SOURCE_ROOT),$(COMPONENT_DIRECTORIES)), $(dir)/$(COMP_LOCATION)/$(COMP_MAKEFILE_NAME).mk)))))
$(if $(TEMP_MAKEFILE),,\
	 $(info Unknown component: $(COMP) - directory or makefile for component not found. Ensure the $(COMP_LOCATION) directory contains $(COMP_MAKEFILE_NAME).mk) \
	 $(info Below is a list of valid local components (Some are internal): ) \
	 $(call FIND_VALID_COMPONENTS, VALID_COMPONENT_LIST,$(COMPONENT_DIRECTORIES)) \
     $(foreach comp,$(VALID_COMPONENT_LIST),$(info $(comp))) \
     $(info Below is a list of valid components from the internet: ) \
     $(info $(call DOWNLOAD_COMPONENT_LIST)) \
     $(error Unknown component: $(COMP) - directory or makefile for component not found. Ensure the $(COMP_LOCATION) directory contains $(COMP_MAKEFILE_NAME).mk))
$(if $(filter 1,$(words $(TEMP_MAKEFILE))),,$(error More than one component with the name "$(COMP)". See $(TEMP_MAKEFILE)))

# Clear all the temporary variables
$(eval GLOBAL_INCLUDES:=)
$(eval GLOBAL_LINK_SCRIPT:=)
$(eval DEFAULT_LINK_SCRIPT:=)
$(eval DCT_LINK_SCRIPT:=)
$(eval GLOBAL_DEFINES:=)
$(eval GLOBAL_CFLAGS:=)
$(eval GLOBAL_CXXFLAGS:=)
$(eval GLOBAL_ASMFLAGS:=)
$(eval GLOBAL_LDFLAGS:=)
$(eval GLOBAL_CERTIFICATES:=)
$(eval WIFI_CONFIG_DCT_H:=)
$(eval BT_CONFIG_DCT_H:=)
$(eval APPLICATION_DCT:=)
$(eval CERTIFICATE:=)
$(eval PRIVATE_KEY:=)
$(eval CHIP_SPECIFIC_SCRIPT:=)
$(eval CONVERTER_OUTPUT_FILE:=)
$(eval FINAL_OUTPUT_FILE:=)
$(eval OLD_CURDIR := $(CURDIR))
$(eval CURDIR := $(CURDIR)$(dir $(TEMP_MAKEFILE)))

# Cache the last valid RTOS/NS combination for iterative filtering.
$(eval TEMP_VALID_OSNS_COMBOS := $(VALID_OSNS_COMBOS))

# Include the component makefile - This defines the NAME variable
$(eval include $(TEMP_MAKEFILE))

# Filter the valid RTOS/NS combination to the least-common set.
$(eval VALID_OSNS_COMBOS :=\
  $(if $(VALID_OSNS_COMBOS),\
    $(filter $(VALID_OSNS_COMBOS),$(TEMP_VALID_OSNS_COMBOS)),\
    $(TEMP_VALID_OSNS_COMBOS)\
  )\
)

$(eval $(NAME)_MAKEFILE :=$(TEMP_MAKEFILE))

# Expand the list of resources to point to the full location (either component local or the common resources directory)
$(eval $(NAME)_RESOURCES_EXPANDED := $(foreach res,$($(NAME)_RESOURCES),$(word 1,$(wildcard $(addsuffix $(res),$(CURDIR) $(SOURCE_ROOT)resources/)))))

$(eval CURDIR := $(OLD_CURDIR))

$(eval $(NAME)_LOCATION ?= $(dir $(TEMP_MAKEFILE)))
$(eval $(NAME)_MAKEFILE := $(TEMP_MAKEFILE))
WICED_SDK_MAKEFILES     += $($(NAME)_MAKEFILE)

# Set debug/release specific options
$(eval $(NAME)_BUILD_TYPE := $(BUILD_TYPE))
$(eval $(NAME)_BUILD_TYPE := $(if $($(NAME)_NEVER_OPTIMISE),  debug,   $($(NAME)_BUILD_TYPE)))
$(eval $(NAME)_BUILD_TYPE := $(if $($(NAME)_ALWAYS_OPTIMISE), release, $($(NAME)_BUILD_TYPE)))

$(NAME)_ASMFLAGS += $(if $(findstring debug,$($(NAME)_BUILD_TYPE)), $(COMPILER_SPECIFIC_DEBUG_ASFLAGS),  $(COMPILER_SPECIFIC_RELEASE_ASFLAGS))
$(NAME)_LDFLAGS  += $(if $(findstring debug,$($(NAME)_BUILD_TYPE)), $(COMPILER_SPECIFIC_DEBUG_LDFLAGS),  $(COMPILER_SPECIFIC_RELEASE_LDFLAGS))

$(NAME)_OPTIM_CFLAGS   ?= $(if $(findstring debug,$($(NAME)_BUILD_TYPE)), $(COMPILER_SPECIFIC_DEBUG_CFLAGS),   $(COMPILER_SPECIFIC_RELEASE_CFLAGS))
$(NAME)_OPTIM_CXXFLAGS ?= $(if $(findstring debug,$($(NAME)_BUILD_TYPE)), $(COMPILER_SPECIFIC_DEBUG_CXXFLAGS), $(COMPILER_SPECIFIC_RELEASE_CXXFLAGS))

WICED_SDK_INCLUDES           +=$(addprefix -I$($(NAME)_LOCATION),$(GLOBAL_INCLUDES))
WICED_SDK_LINK_SCRIPT        +=$(if $(GLOBAL_LINK_SCRIPT),$(GLOBAL_LINK_SCRIPT),)
WICED_SDK_DEFAULT_LINK_SCRIPT+=$(if $(DEFAULT_LINK_SCRIPT),$(addprefix $($(NAME)_LOCATION),$(DEFAULT_LINK_SCRIPT)),)
WICED_SDK_DEFINES            +=$(GLOBAL_DEFINES)
WICED_SDK_CFLAGS             +=$(GLOBAL_CFLAGS)
WICED_SDK_CXXFLAGS           +=$(GLOBAL_CXXFLAGS)
WICED_SDK_ASMFLAGS           +=$(GLOBAL_ASMFLAGS)
WICED_SDK_LDFLAGS            +=$(GLOBAL_LDFLAGS)
WICED_SDK_CERTIFICATE        +=$(if $(CERTIFICATE),$(CERTIFICATE))
WICED_SDK_PRIVATE_KEY        +=$(if $(PRIVATE_KEY),$(PRIVATE_KEY))
WICED_SDK_DCT_LINK_SCRIPT    +=$(if $(DCT_LINK_SCRIPT),$(addprefix $($(NAME)_LOCATION),$(DCT_LINK_SCRIPT)),)
WICED_SDK_WIFI_CONFIG_DCT_H  +=$(if $(WIFI_CONFIG_DCT_H),$(addprefix $($(NAME)_LOCATION),$(WIFI_CONFIG_DCT_H)),)
WICED_SDK_BT_CONFIG_DCT_H    +=$(if $(BT_CONFIG_DCT_H),$(addprefix $($(NAME)_LOCATION),$(BT_CONFIG_DCT_H)),)
WICED_SDK_APPLICATION_DCT    +=$(if $(APPLICATION_DCT),$(addprefix $($(NAME)_LOCATION),$(APPLICATION_DCT)),)
# when wifi_config_dct.h file exists in the application directory, add
# add its directory to includes and add a  WIFI_CONFIG_APPLICATION_DEFINED define
WICED_SDK_INCLUDES            +=$(if $(WIFI_CONFIG_DCT_H),-I$($(NAME)_LOCATION),)
WICED_SDK_DEFINES             += $(if $(WIFI_CONFIG_DCT_H),WIFI_CONFIG_APPLICATION_DEFINED,)
WICED_SDK_DEFINES             += SFLASH_APPS_HEADER_LOC=$(APPS_LUT_HEADER_LOC)

WICED_SDK_INCLUDES            +=$(if $(BT_CONFIG_DCT_H),-I$($(NAME)_LOCATION),)
WICED_SDK_DEFINES             += $(if $(BT_CONFIG_DCT_H),BT_CONFIG_APPLICATION_DEFINED,)
WICED_SDK_CHIP_SPECIFIC_SCRIPT += $(CHIP_SPECIFIC_SCRIPT)
WICED_SDK_CONVERTER_OUTPUT_FILE += $(CONVERTER_OUTPUT_FILE)
WICED_SDK_FINAL_OUTPUT_FILE += $(FINAL_OUTPUT_FILE)

$(eval PROCESSED_COMPONENTS += $(NAME))
$(eval PROCESSED_COMPONENTS_LOCS += $(COMP))
$(eval COMPONENTS += $($(NAME)_COMPONENTS))
$(if $(strip $(filter-out $(PROCESSED_COMPONENTS_LOCS),$(COMPONENTS))),$(eval $(call PROCESS_COMPONENT,$(filter-out $(PROCESSED_COMPONENTS_LOCS),$(COMPONENTS)))),)
#$(foreach component,$($(NAME)_COMPONENTS),$(if $(filter $(component),$(PROCESSED_COMPONENTS_LOCS)),,$(eval $(call PROCESS_COMPONENT,$(component)))))
endef

##################################
# Start of processing
##################################

# Separate the build string into components
COMPONENTS := $(subst -, ,$(MAKECMDGOALS))

BUS_LIST        := SPI \
                   SDIO \
                   SoC.4390 \
                   SoC.43909

BUILD_TYPE_LIST := debug \
                   release

IMAGE_TYPE_LIST := rom \
                   ram

# Extract out: the bus option, the debug/release option, OTA option, and the lint option
BUS                 := $(if $(filter $(BUS_LIST),$(COMPONENTS)),$(firstword $(filter $(BUS_LIST),$(COMPONENTS))))
BUILD_TYPE          := $(if $(filter $(BUILD_TYPE_LIST),$(COMPONENTS)),$(firstword $(filter $(BUILD_TYPE_LIST),$(COMPONENTS))),release)
IMAGE_TYPE          := $(if $(filter $(IMAGE_TYPE_LIST),$(COMPONENTS)),$(firstword $(filter $(IMAGE_TYPE_LIST),$(COMPONENTS))),ram)
USES_BOOTLOADER_OTA := $(if $(filter ota,$(COMPONENTS)),$(if $(filter ota2,$(COMPONENTS)),,1))
RUN_LINT            := $(filter lint,$(COMPONENTS))
COMPONENTS          := $(filter-out $(BUS_LIST) $(BUILD_TYPE_LIST) $(IMAGE_TYPE_LIST), $(COMPONENTS))

# Set debug/release specific options
ifeq ($(BUILD_TYPE),debug)
WICED_SDK_LDFLAGS  += $(COMPILER_SPECIFIC_DEBUG_LDFLAGS)
else
WICED_SDK_LDFLAGS  += $(COMPILER_SPECIFIC_RELEASE_LDFLAGS)
endif

# include the correct FLASH positioning
ifeq (1,$(OTA2_SUPPORT))
GLOBAL_DEFINES	+= OTA2_SUPPORT=1
PLATFORM   :=$(strip $(foreach comp,$(subst .,/,$(COMPONENTS)),$(if $(wildcard $(SOURCE_ROOT)platforms/$(comp)),$(comp),)))
include platforms/$(subst .,/,$(PLATFORM))/ota2_image_defines.mk
endif

# $(1) = component name
define DOWNLOAD_COMPONENT
	$(info Component $(1) not found - attempting to find on WICED website) \
	$(info $(shell $(FETCH_COMPONENT)  $(1) $(WICED_SDK_VERSION) $(WICED_COMPONENT_URL)))
endef


define DOWNLOAD_COMPONENT_LIST
	$(shell $(FETCH_COMPONENT)  * $(WICED_SDK_VERSION) $(WICED_COMPONENT_URL))
endef

include $(MAKEFILES_PATH)/downloaded_components.mk

USED_DOWNLOADED_COMPONENTS := $(filter $(COMPONENTS), $(DOWNLOADED_COMPONENTS))

ifneq ($(USED_DOWNLOADED_COMPONENTS),)
   $(info ***************************************************** )
   $(info ***************************************************** )
   $(info * WARNING : Using downloaded third-pardy components   )
   $(info *           These components have not been tested by  )
   $(info *           Broadcom. Refer all support questions to  )
   $(info *           the component manufacturer                )
   $(foreach comp, $(USED_DOWNLOADED_COMPONENTS), \
   $(info *         : $(comp)  from  $($(comp)_MANUFACTURER)) \
   $(info *           $($(comp)_DESCRIPTION)) $(info * ) )
   $(info ***************************************************** )
   $(info ***************************************************** )
endif



# Check if there are any unknown components; output error if so.
$(foreach comp, $(COMPONENTS), $(if $(wildcard $(foreach dir, $(addprefix $(SOURCE_ROOT),$(COMPONENT_DIRECTORIES)), $(dir)/$(subst .,/,$(comp)) ) ),,$(error Unknown component: $(comp))))
#$(foreach comp, $(COMPONENTS), $(if $(wildcard $(foreach dir, $(addprefix $(SOURCE_ROOT),$(COMPONENT_DIRECTORIES)), $(dir)/$(comp) ) ),,$(call DOWNLOAD_COMPONENT,$(comp))))
$(foreach comp, $(COMPONENTS), $(if $(wildcard $(foreach dir, $(addprefix $(SOURCE_ROOT),$(COMPONENT_DIRECTORIES)), $(dir)/$(subst .,/,$(comp)) ) ),,\
	 $(info Unknown component: $(comp) - directory or makefile for component not found. Ensure the $(COMP) directory contains $(notdir $(subst .,/,$(comp))).mk) \
	 $(info Below is a list of valid local components (Some are internal): ) \
	 $(call FIND_VALID_COMPONENTS, VALID_COMPONENT_LIST,$(COMPONENT_DIRECTORIES)) \
     $(foreach comp1,$(VALID_COMPONENT_LIST),$(info $(comp1))) \
     $(info Below is a list of valid components from the internet: ) \
     $(info $(call DOWNLOAD_COMPONENT_LIST)) \
     $(error Unknown component: $(comp) - directory or makefile for component not found. Ensure the $(COMP) directory contains $(notdir $(comp)).mk)))


# Find the matching network, platform, RTOS and application from the build string components
NETWORK_FULL    :=$(strip $(foreach comp,$(subst .,/,$(COMPONENTS)),$(if $(wildcard $(SOURCE_ROOT)WICED/network/$(comp)),$(comp),)))
RTOS_FULL       :=$(strip $(foreach comp,$(subst .,/,$(COMPONENTS)),$(if $(wildcard $(SOURCE_ROOT)WICED/RTOS/$(comp)),$(comp),)))
PLATFORM_FULL   :=$(strip $(foreach comp,$(subst .,/,$(COMPONENTS)),$(if $(wildcard $(SOURCE_ROOT)platforms/$(comp)),$(comp),)))
APP_FULL        :=$(strip $(foreach comp,$(subst .,/,$(COMPONENTS)),$(if $(wildcard $(SOURCE_ROOT)apps/$(comp)),$(comp),)))
USB_FULL        :=$(strip $(foreach comp,$(subst .,/,$(COMPONENTS)),$(if $(wildcard $(SOURCE_ROOT)libraries/drivers/USB/$(comp)),$(comp),)))

NETWORK  :=$(notdir $(NETWORK_FULL))
RTOS     :=$(notdir $(RTOS_FULL))
PLATFORM    :=$(subst /,.,$(PLATFORM_FULL))
APP      :=$(notdir $(APP_FULL))
USB         :=$(notdir $(USB_FULL))

PLATFORM_DIRECTORY :=$(PLATFORM_FULL)

# Define default RTOS
ifndef RTOS
RTOS:=ThreadX
COMPONENTS += ThreadX
endif

# Check if APP is wwd; if so, build app with WWD only, no DCT, no bootloader
ifneq (,$(findstring wwd,$(APP_FULL)))
APP_WWD_ONLY        := 1
USES_BOOTLOADER_OTA := 0
NODCT               := 1
endif

EXTRA_CFLAGS :=    -DWICED_VERSION=$(SLASH_QUOTE_START)$(WICED_SDK_VERSION)$(SLASH_QUOTE_END) \
                   -DBUS=$(SLASH_QUOTE_START)$$(BUS)$(SLASH_QUOTE_END) \
                   -I$(OUTPUT_DIR)/resources/  \
                   -DPLATFORM=$(SLASH_QUOTE_START)$$(PLATFORM)$(SLASH_QUOTE_END)
ifeq (1,$(OTA2_SUPPORT))

include platforms/$(PLATFORM_FULL)/ota2_image_defines.mk

OTA2_IMAGE_CFLAGS := -DOTA2_SUPPORT=1
OTA2_IMAGE_CFLAGS += -DOTA2_IMAGE_FLASH_BASE=$(OTA2_IMAGE_FLASH_BASE)
OTA2_IMAGE_CFLAGS += -DOTA2_IMAGE_FACTORY_RESET_AREA_BASE=$(OTA2_IMAGE_FACTORY_RESET_AREA_BASE)
OTA2_IMAGE_CFLAGS += -DOTA2_IMAGE_APP_DCT_SAVE_AREA_BASE=$(OTA2_IMAGE_APP_DCT_SAVE_AREA_BASE)
OTA2_IMAGE_CFLAGS += -DOTA2_IMAGE_CURRENT_AREA_BASE=$(OTA2_IMAGE_CURRENT_AREA_BASE)
OTA2_IMAGE_CFLAGS += -DOTA2_IMAGE_CURR_LUT_AREA_BASE=$(OTA2_IMAGE_CURR_LUT_AREA_BASE)
OTA2_IMAGE_CFLAGS += -DOTA2_IMAGE_CURR_DCT_1_AREA_BASE=$(OTA2_IMAGE_CURR_DCT_1_AREA_BASE)
OTA2_IMAGE_CFLAGS += -DOTA2_IMAGE_CURR_FS_AREA_BASE=$(OTA2_IMAGE_CURR_FS_AREA_BASE)
OTA2_IMAGE_CFLAGS += -DOTA2_IMAGE_CURR_APP0_AREA_BASE=$(OTA2_IMAGE_CURR_APP0_AREA_BASE)
ifneq (,$(OTA2_IMAGE_LAST_KNOWN_GOOD_AREA_BASE))
OTA2_IMAGE_CFLAGS += -DOTA2_IMAGE_LAST_KNOWN_GOOD_AREA_BASE=$(OTA2_IMAGE_LAST_KNOWN_GOOD_AREA_BASE)
endif
OTA2_IMAGE_CFLAGS += -DOTA2_IMAGE_STAGING_AREA_BASE=$(OTA2_IMAGE_STAGING_AREA_BASE)
OTA2_IMAGE_CFLAGS += -DOTA2_IMAGE_STAGING_AREA_SIZE=$(OTA2_IMAGE_STAGING_AREA_SIZE)

#add the OTA2 flags to the WICED SDK flags

EXTRA_CFLAGS += $(OTA2_IMAGE_CFLAGS)
endif

# Load platform makefile to make variables like WLAN_CHIP, HOST_OPENOCD & HOST_ARCH available to all makefiles
$(eval CURDIR := $(SOURCE_ROOT)platforms/$(PLATFORM_DIRECTORY)/)
include $(SOURCE_ROOT)platforms/$(PLATFORM_DIRECTORY)/$(notdir $(PLATFORM_DIRECTORY)).mk
$(eval CURDIR := $(SOURCE_ROOT)WICED/platform/MCU/$(HOST_MCU_FAMILY)/)
include $(SOURCE_ROOT)WICED/platform/MCU/$(HOST_MCU_FAMILY)/$(HOST_MCU_FAMILY).mk
MAIN_COMPONENT_PROCESSING :=1


# Define default USB stack (if USB support is enabled)
ifneq ($(WICED_USB_SUPPORT),)
ifndef USB
ifeq ($(RTOS),ThreadX)
ifeq ($(BUS),SoC.43909)
USB:=USBX
COMPONENTS += drivers/USB/USBX
endif
endif
endif
endif

# Now we know the target architecture - include all toolchain makefiles and check one of them can handle the architecture
CC :=

ifneq ($(filter $(HOST_ARCH),ARM_CM3 ARM_CM4 ARM_CR4),)

ifeq ($(IAR),1)
include $(MAKEFILES_PATH)/wiced_toolchain_IAR.mk
else
include $(MAKEFILES_PATH)/wiced_toolchain_ARM_GNU.mk
endif

else # ifneq ($(filter $(HOST_ARCH),ARM_CM3 ARM_CM4 ARM_CR4),)
ifneq ($(filter $(HOST_ARCH),MIPS),)
include $(MAKEFILES_PATH)/wiced_toolchain_Win32_MIPS.mk
endif # ifneq ($(filter $(HOST_ARCH),MIPS),)
endif # ifneq ($(filter $(HOST_ARCH),ARM_CM3 ARM_CM4 ARM_CR4),)

ifndef CC
$(error No matching toolchain found for architecture $(HOST_ARCH))
endif

# Process all the components + WICED
COMPONENTS += WICED
#$(info processing components: $(COMPONENTS))

CURDIR :=

$(eval $(call PROCESS_COMPONENT, $(COMPONENTS)))

# Add some default values
WICED_SDK_INCLUDES += -I$(SOURCE_ROOT)WICED/WWD/internal/chips/$(WLAN_CHIP_FAMILY) -I$(SOURCE_ROOT)libraries -I$(SOURCE_ROOT)include
WICED_SDK_WIFI_CONFIG_DCT_H:=$(if $(strip $(WICED_SDK_WIFI_CONFIG_DCT_H)),$(strip $(WICED_SDK_WIFI_CONFIG_DCT_H)),$(SOURCE_ROOT)include/default_wifi_config_dct.h)
WICED_SDK_BT_CONFIG_DCT_H:=$(if $(strip $(WICED_SDK_BT_CONFIG_DCT_H)),$(strip $(WICED_SDK_BT_CONFIG_DCT_H)),$(SOURCE_ROOT)include/default_bt_config_dct.h)
WICED_SDK_DEFINES += WICED_SDK_WIFI_CONFIG_DCT_H=$(SLASH_QUOTE_START)$(WICED_SDK_WIFI_CONFIG_DCT_H)$(SLASH_QUOTE_END)
WICED_SDK_DEFINES += WICED_SDK_BT_CONFIG_DCT_H=$(SLASH_QUOTE_START)$(WICED_SDK_BT_CONFIG_DCT_H)$(SLASH_QUOTE_END)
WICED_SDK_DEFINES += $(EXTERNAL_WICED_GLOBAL_DEFINES)

ALL_RESOURCES := $(sort $(foreach comp,$(PROCESSED_COMPONENTS),$($(comp)_RESOURCES_EXPANDED)))

# Make sure the user has specified a component from each category
$(if $(RTOS),,$(error No RTOS specified. Options are: $(notdir $(wildcard WICED/RTOS/*))))
$(if $(PLATFORM),,$(error No platform specified. Options are: $(notdir $(wildcard platforms/*))))
$(if $(APP),,$(error No application specified. Options are: $(notdir $(wildcard apps/*))))
$(if $(BUS),,$(error No bus specified. Options are: SDIO SPI))

# Make sure a WLAN_CHIP, WLAN_CHIP_REVISION, WLAN_CHIP_FAMILY and HOST_OPENOCD have been defined
$(if $(WLAN_CHIP),,$(error No WLAN_CHIP has been defined))
$(if $(WLAN_CHIP_REVISION),,$(error No WLAN_CHIP_REVISION has been defined))
$(if $(WLAN_CHIP_FAMILY),,$(error No WLAN_CHIP_FAMILY has been defined))
$(if $(HOST_OPENOCD),,$(error No HOST_OPENOCD has been defined))

$(eval VALID_PLATFORMS := $(call EXPAND_WILDCARD_PLATFORMS,$(VALID_PLATFORMS)))
$(eval INVALID_PLATFORMS := $(call EXPAND_WILDCARD_PLATFORMS,$(INVALID_PLATFORMS)))

# Check for valid platform, OSNS combination, build type, image type and bus
$(eval $(if $(VALID_PLATFORMS), $(if $(filter $(VALID_PLATFORMS),$(PLATFORM)),,$(error $(APP) application does not support $(PLATFORM) platform)),))
$(eval $(if $(INVALID_PLATFORMS), $(if $(filter $(INVALID_PLATFORMS),$(PLATFORM)),$(error $(APP) application does not support $(PLATFORM) platform)),))
$(eval $(if $(VALID_OSNS_COMBOS), $(if $(filter $(VALID_OSNS_COMBOS),$(RTOS) $(RTOS)-$(NETWORK)),,$(error $(APP) application does not support $(RTOS)-$(NETWORK) combination)),))
$(eval $(if $(VALID_BUILD_TYPES), $(if $(filter $(VALID_BUILD_TYPES),$(BUILD_TYPE)),,$(error $(APP) application does not support $(BUILD_TYPE) build)),))
$(eval $(if $(VALID_BUSES), $(if $(filter $(VALID_BUSES),$(BUS)),,$(error $(PLATFORM) platform does not support $(BUS) bus type)),))
$(eval $(if $(VALID_IMAGE_TYPES), $(if $(filter $(VALID_IMAGE_TYPES),$(IMAGE_TYPE)),,$(error $(APP) application does not support $(IMAGE_TYPE) build)),))

REMOVE_FIRST = $(wordlist 2,$(words $(1)),$(1))


EXTRA_TARGET_MAKEFILES :=$(call unique,$(EXTRA_TARGET_MAKEFILES))
$(foreach makefile_name,$(EXTRA_TARGET_MAKEFILES),$(eval include $(makefile_name)))

$(CONFIG_FILE_DIR):
	$(QUIET)$(call MKDIR, $@)

# Summarize all the information into the config file


# Fill out full CFLAGS - done here to allow late expansion of macros
$(foreach comp,$(PROCESSED_COMPONENTS), $(eval $(comp)_CFLAGS_ALL := $(call ADD_COMPILER_SPECIFIC_STANDARD_CFLAGS,$($(comp)_OPTIM_CFLAGS))) )
$(foreach comp,$(PROCESSED_COMPONENTS), $(eval $(comp)_CFLAGS_ALL += $(EXTRA_CFLAGS)) )
$(foreach comp,$(PROCESSED_COMPONENTS), $(eval $(comp)_CFLAGS_ALL += $($(comp)_CFLAGS)) )

$(foreach comp,$(PROCESSED_COMPONENTS), $(eval $(comp)_CXXFLAGS_ALL := $(call ADD_COMPILER_SPECIFIC_STANDARD_CXXFLAGS,$($(comp)_OPTIM_CXXFLAGS))) )
$(foreach comp,$(PROCESSED_COMPONENTS), $(eval $(comp)_CXXFLAGS_ALL += $(EXTRA_CFLAGS)) )
$(foreach comp,$(PROCESSED_COMPONENTS), $(eval $(comp)_CXXFLAGS_ALL += $($(comp)_CXXFLAGS)) )


WICED_SDK_PREBUILT_LIBRARIES +=$(foreach comp,$(PROCESSED_COMPONENTS), $(addprefix $($(comp)_LOCATION),$($(comp)_PREBUILT_LIBRARY)))
WICED_SDK_LINK_FILES         +=$(foreach comp,$(PROCESSED_COMPONENTS), $(addprefix $$(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(comp)),$($(comp)_LINK_FILES)))
WICED_SDK_UNIT_TEST_SOURCES  +=$(foreach comp,$(PROCESSED_COMPONENTS), $(addprefix $($(comp)_LOCATION),$($(comp)_UNIT_TEST_SOURCES)))

ifeq ($(ADD_UNIT_TESTS_TO_LINK_FILES),1)
WICED_SDK_LINK_FILES         += $(patsubst %.cpp,%.o,$(patsubst %.cc,%.o,$(patsubst %.c,%.o, $(foreach comp,$(PROCESSED_COMPONENTS), $(addprefix $$(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(comp)),$($(comp)_UNIT_TEST_SOURCES))) )))
endif

.PHONY: $(MAKECMDGOALS)
$(MAKECMDGOALS): $(CONFIG_FILE) $(TOOLCHAIN_HOOK_TARGETS)

$(CONFIG_FILE): $(WICED_SDK_MAKEFILES) | $(CONFIG_FILE_DIR)
	$(QUIET)$(call WRITE_FILE_CREATE, $(CONFIG_FILE) ,WICED_SDK_MAKEFILES           += $(WICED_SDK_MAKEFILES))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,TOOLCHAIN_NAME            := $(TOOLCHAIN_NAME))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WICED_SDK_LDFLAGS             += $(strip $(WICED_SDK_LDFLAGS)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,RESOURCE_CFLAGS           += $(strip $(WICED_SDK_CFLAGS)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WICED_SDK_LINK_SCRIPT         += $(strip $(if $(strip $(WICED_SDK_LINK_SCRIPT)),$(WICED_SDK_LINK_SCRIPT),$(WICED_SDK_DEFAULT_LINK_SCRIPT))))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WICED_SDK_LINK_SCRIPT_CMD     += $(call COMPILER_SPECIFIC_LINK_SCRIPT,$(strip $(if $(strip $(WICED_SDK_LINK_SCRIPT)),$(WICED_SDK_LINK_SCRIPT),$(WICED_SDK_DEFAULT_LINK_SCRIPT)))))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WICED_SDK_PREBUILT_LIBRARIES  += $(strip $(WICED_SDK_PREBUILT_LIBRARIES)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WICED_SDK_CERTIFICATES        += $(strip $(WICED_SDK_CERTIFICATES)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WICED_SDK_PRE_APP_BUILDS      += $(strip $(PRE_APP_BUILDS)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WICED_SDK_DCT_LINK_SCRIPT     += $(strip $(WICED_SDK_DCT_LINK_SCRIPT)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WICED_SDK_DCT_LINK_CMD        += $(strip $(addprefix $(COMPILER_SPECIFIC_LINK_SCRIPT_DEFINE_OPTION) ,$(WICED_SDK_DCT_LINK_SCRIPT))))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WICED_SDK_APPLICATION_DCT     += $(strip $(WICED_SDK_APPLICATION_DCT)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WICED_SDK_WIFI_CONFIG_DCT_H   += $(strip $(WICED_SDK_WIFI_CONFIG_DCT_H)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WICED_SDK_BT_CONFIG_DCT_H     += $(strip $(WICED_SDK_BT_CONFIG_DCT_H)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WICED_SDK_LINK_FILES          += $(WICED_SDK_LINK_FILES))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WICED_SDK_INCLUDES            += $(call unique,$(WICED_SDK_INCLUDES)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WICED_SDK_DEFINES             += $(call unique,$(strip $(addprefix -D,$(WICED_SDK_DEFINES)))))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,COMPONENTS                := $(PROCESSED_COMPONENTS))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,BUS                       := $(BUS))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,IMAGE_TYPE                := $(IMAGE_TYPE))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,NETWORK_FULL              := $(NETWORK_FULL))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,RTOS_FULL                 := $(RTOS_FULL))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,PLATFORM_DIRECTORY             := $(PLATFORM_DIRECTORY))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,APP_FULL                  := $(APP_FULL))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,NETWORK                   := $(NETWORK))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,RTOS                      := $(RTOS))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,PLATFORM                  := $(PLATFORM))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,USB                       := $(USB))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,APP                       := $(APP))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,HOST_OPENOCD                := $(HOST_OPENOCD))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,HOST_ARCH                 := $(HOST_ARCH))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WICED_SDK_CERTIFICATE         :=$(call unique, $(WICED_SDK_CERTIFICATE)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WICED_SDK_PRIVATE_KEY         := $(call unique,$(WICED_SDK_PRIVATE_KEY)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,NO_BUILD_BOOTLOADER           := $(NO_BUILD_BOOTLOADER))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,NO_BOOTLOADER_REQUIRED         := $(NO_BOOTLOADER_REQUIRED))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_LOCATION         := $($(comp)_LOCATION)))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_SOURCES          += $($(comp)_SOURCES)))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_CHECK_HEADERS    += $($(comp)_CHECK_HEADERS)))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_INCLUDES         := $(addprefix -I$($(comp)_LOCATION),$($(comp)_INCLUDES))))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_DEFINES          := $(addprefix -D,$($(comp)_DEFINES))))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_CFLAGS           := $(WICED_SDK_CFLAGS) $($(comp)_CFLAGS_ALL)))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_CXXFLAGS         := $(WICED_SDK_CXXFLAGS) $($(comp)_CXXFLAGS_ALL)))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_ASMFLAGS         := $(WICED_SDK_ASMFLAGS) $($(comp)_ASMFLAGS)))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_RESOURCES        := $($(comp)_RESOURCES_EXPANDED)))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_MAKEFILE         := $($(comp)_MAKEFILE)))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_PRE_BUILD_TARGETS:= $($(comp)_PRE_BUILD_TARGETS)))
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,$(comp)_PREBUILT_LIBRARY := $(addprefix $($(comp)_LOCATION),$($(comp)_PREBUILT_LIBRARY))))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WICED_SDK_UNIT_TEST_SOURCES   := $(WICED_SDK_UNIT_TEST_SOURCES))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,APP_WWD_ONLY              := $(APP_WWD_ONLY))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,USES_BOOTLOADER_OTA       := $(USES_BOOTLOADER_OTA))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,NODCT                     := $(NODCT))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,ALL_RESOURCES             := $(call unique,$(ALL_RESOURCES)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,INTERNAL_MEMORY_RESOURCES := $(call unique,$(INTERNAL_MEMORY_RESOURCES)))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,EXTRA_TARGET_MAKEFILES := $(EXTRA_TARGET_MAKEFILES))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,APPS_LUT_HEADER_LOC := $(APPS_LUT_HEADER_LOC))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,APPS_START_SECTOR := $(APPS_START_SECTOR) )
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,FR_APP := $(FR_APP))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,OTA_APP := $(OTA_APP))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,DCT_IMAGE := $(DCT_IMAGE))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,FILESYSTEM_IMAGE := $(FILESYSTEM_IMAGE) )
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WIFI_FIRMWARE := $(WIFI_FIRMWARE) )
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,APP0 := $(APP0) )
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,APP1 := $(APP1) )
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,APP2 := $(APP2) )
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,FR_APP_SECURE := $(FR_APP_SECURE))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,OTA_APP_SECURE := $(OTA_APP_SECURE))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WICED_ROM_SYMBOL_LIST_FILE := $(WICED_ROM_SYMBOL_LIST_FILE))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WICED_SDK_CHIP_SPECIFIC_SCRIPT := $(WICED_SDK_CHIP_SPECIFIC_SCRIPT))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WICED_SDK_CONVERTER_OUTPUT_FILE := $(WICED_SDK_CONVERTER_OUTPUT_FILE))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WICED_SDK_FINAL_OUTPUT_FILE := $(WICED_SDK_FINAL_OUTPUT_FILE))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WICED_RAM_STUB_LIST_FILE := $(WICED_RAM_STUB_LIST_FILE))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,DCT_IMAGE_SECURE := $(DCT_IMAGE_SECURE))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,FILESYSTEM_IMAGE_SECURE := $(FILESYSTEM_IMAGE_SECURE))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,WIFI_FIRMWARE_SECURE := $(WIFI_FIRMWARE_SECURE))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,APP0_SECURE := $(APP0_SECURE))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,APP1_SECURE := $(APP1_SECURE))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_FILE) ,APP2_SECURE := $(APP2_SECURE))
