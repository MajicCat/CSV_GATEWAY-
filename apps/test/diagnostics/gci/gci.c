/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

/** @file
 *
 * GCI core testing
 *
 */

#include "wiced.h"

#define GCI_STATUS_REG_NUM 13

static char *gci_status_4_names[] =
{
    "sdio_mode", "gspi_mode", "hsic_mode", "sdioh_mode",
    "trim_en", "lpo_sel", "strap_lpo_sel", "rsrc_init_mode_0",
    "rsrc_init_mode_1", "sel0_sdio", "sel1_sdio", "mode_sdio_clk",
    "bb_pll_locked", "hsic_pll_locked", "line_ltate", "none",
    "usb2_dsel", "audio_pll_locked", "usb_phy_mode", "usbphy_pll_lock",
    "host_iface_strap_0 (0 = sdiod, 1 = sdioh)", "host_iface_strap_1 ((0 = acpu_rom, 1 = acpu_ram)",
    "host_iface_strap_2 (0 = hsic_phy, 1 = usb_phy)", "host_iface_strap_3 ((0 = wcpu_rom, 1 = wcpu_ram)",
    "otp_sdio_3v3", "app_clk_switch_sel_readback", "strap_trim_en", "host_iface_strap_4 (gspi mode)", "package_option_3_0",
    "none", "none", "none"
};

static void print_status_reg_4(uint32_t st)
{
    int i;
    for (i = 0; i < 32; i++)
    {
        if (strcmp(gci_status_4_names[i], "none"))
        {
            printf("   %s %d\n", gci_status_4_names[i], (unsigned)((st >> i) & 0x1));
        }
    }
}

static void print_status_reg_8(uint32_t st)
{
    printf("   fab_id 0x%02x(%d)\n", (unsigned)((st >> 24) & 0xff), (unsigned)((st >> 24) & 0xff));
    printf("   package_option 0x%02x(%d)\n", (unsigned)((st >> 20) & 0x0f), (unsigned)((st >> 20) & 0x0f));
    printf("   chip_rev_id 0x%02x(%d)\n", (unsigned)((st >> 16) & 0x0f), (unsigned)((st >> 16) & 0x0f));
    printf("   radio_rev_id 0x%02x(%d)\n", (unsigned)((st >> 8) & 0xff), (unsigned)((st >> 8) & 0xff));
    printf("   misc_rev_id 0x%02x(%d)\n", (unsigned)(st & 0xff), (unsigned)(st & 0xff));
}

void gci_test(void)
{
    int i;
    uint32_t st;

    printf("GCI core status register dump\n");
    for (i = 0; i < GCI_STATUS_REG_NUM; i++)
    {
        st = platform_gci_chipstatus(i);
        printf("%2d %08x\n", i, (unsigned)st);
        if (i == 4)
        {
            print_status_reg_4(st);
        }
        else if (i == 8)
        {
            print_status_reg_8(st);
        }
    }
}
