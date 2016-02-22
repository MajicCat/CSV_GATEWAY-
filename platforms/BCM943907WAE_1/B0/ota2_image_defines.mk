#
# Copyright 2015, Broadcom Corporation
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#
#
#
#  Layout for FLASH
#
#  Offset 0x000000
#   +---------------------------------------+
#   | Boot loader Area						|
#   +---------------------------------------+
#   | Factory Reset OTA Image				|
#   +---------------------------------------+
#   | DCT Save area (when updating)			|
#   +---------------------------------------+
#   | Current Area							|
#   +---------------------------------------+
#   | Last Known Good (if enabled )			|
#   +---------------------------------------+
#   | OTA Staging area (downloaded image)	|
#   +---------------------------------------+
#
#  LAST KNOWN GOOD Not supported yet
#
# total of all sizes must == FLASH size

SECTOR_SIZE := 4096

OTA2_IMAGE_FLASH_BASE                := 0x00000000

# Bootloader is 32k
# Factory Reset OTA Image is 2MB
OTA2_IMAGE_FACTORY_RESET_AREA_BASE   := 0x00008000

# DCT copy is 16K in size
OTA2_IMAGE_APP_DCT_SAVE_AREA_BASE    := 0x00208000

# Current Area 2.5MB
# LUT - 4k
# DCT - 8k
# Filesystem - 450k
OTA2_IMAGE_CURRENT_AREA_BASE         := 0x0020c000
OTA2_IMAGE_CURR_LUT_AREA_BASE        := 0x0020c000
OTA2_IMAGE_CURR_DCT_1_AREA_BASE      := 0x0020d000
OTA2_IMAGE_CURR_FS_AREA_BASE         := 0x00216000
OTA2_IMAGE_CURR_APP0_AREA_BASE       := 0x00300000

# Last Known Good not suppoorted yet
# ( size is ~2.5 Mb)
# OTA2_IMAGE_LAST_KNOWN_GOOD_AREA_BASE := 0x00000000

# Staging OTA Image is 2.5MB
OTA2_IMAGE_STAGING_AREA_BASE         := 0x00600000
OTA2_IMAGE_STAGING_AREA_SIZE         := 0x00200000	# 2MB (currently 850KB)

# TOTAL                                0x00800000	# 8 MB
# total of all sizes must <= FLASH size
#
