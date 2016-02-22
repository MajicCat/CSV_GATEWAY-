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

CONFIG_FILE := build/$(FRAPP)/config.mk

include $(CONFIG_FILE)

OUTPUT_DIR :=./build/$(FRAPP)/
OUTPUT_DIR_CONVD := $(call CONV_SLASHES,$(OUTPUT_DIR))

SFLASHWRITER_TGT:=waf.sflash_write-NoOS-$(PLATFORM)-$(BUS)
SFLASHWRITER_TGT_DIR:=$(SFLASHWRITER_TGT)

.PHONY: sflash sflash_download sflash_writer_app

sflash_writer_app:
	$(QUIET)$(ECHO) Building the Serial Flash Writer App $(SFLASHWRITER_TGT) $(FRAPP)
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(MAKE) $(SILENT) -f $(SOURCE_ROOT)Makefile -s $(SFLASHWRITER_TGT) NO_BUILD_BOOTLOADER=1
	$(QUIET)$(ECHO) Done
	$(QUIET)$(ECHO_BLANK_LINE)

./build/$(SFLASHWRITER_TGT_DIR)/config.mk: sflash_writer_app

sflash_download: sflash_writer_app sflash
	$(QUIET)$(ECHO) Downloading Serial Flash image
	$(QUIET)$(ECHO_BLANK_LINE)
	$(call CONV_SLASHES,$(OPENOCD_FULL_NAME)) -f $(OPENOCD_PATH)$(JTAG).cfg -f $(OPENOCD_PATH)$(HOST_OPENOCD).cfg -f apps/waf/sflash_write/sflash_write.tcl -c "sflash_write_file $(OUTPUT_DIR)sflash.bin 0x0 $(PLATFORM)-$(BUS) 1 0" -c shutdown
	$(QUIET)$(ECHO_BLANK_LINE)

#pad_dct:
#	$(QUIET)$(PERL) $(TOOLS_ROOT)/create_dct/pad_dct.pl $(DCT)

sflash: $(SFLASH_INCLUDE)
	$(QUIET)$(ECHO) Concatenating the binaries into an image for the serial-flash chip $(OTA_APP)
	$(QUIET)$(ECHO_BLANK_LINE)
	rm -rf $(OUTPUT_DIR_CONVD)sflash.bin
ifneq ($(strip $(FR_APP)),)
	$(QUIET)$(CAT) $(call CONV_SLASHES,$(FR_APP)) >> $(OUTPUT_DIR_CONVD)sflash.bin
endif
ifneq ($(strip $(DCT_IMAGE)),)
	$(QUIET)$(CAT) $(call CONV_SLASHES,$(DCT_IMAGE)) >> $(OUTPUT_DIR_CONVD)sflash.bin
endif
ifneq ($(strip $(OTA_APP)),)
	$(QUIET)$(CAT) $(call CONV_SLASHES,$(OTA_APP)) >> $(OUTPUT_DIR_CONVD)sflash.bin
endif
ifneq ($(strip $(APP0)),)
	$(QUIET)$(CAT) $(call CONV_SLASHES,$(APP0)) >> $(OUTPUT_DIR_CONVD)sflash.bin
endif