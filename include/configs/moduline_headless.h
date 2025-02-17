/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2019 NXP
 */

#ifndef __MODULINE_HEADLESS_H
#define __MODULINE_HEADLESS_H

#include <linux/sizes.h>
#include <linux/stringify.h>
#include <asm/arch/imx-regs.h>

#define UBOOT_ITB_OFFSET			0x57C00
#define CFG_SYS_UBOOT_BASE	\
	(QSPI0_AMBA_BASE + CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR * 512)

#ifdef CONFIG_XPL_BUILD
/* malloc f used before GD_FLG_FULL_MALLOC_INIT set */
#define CFG_MALLOC_F_ADDR		0x930000
/* For RAW image gives a error info not panic */

#endif

/* Link Definitions */

#define CFG_SYS_INIT_RAM_ADDR        0x40000000
#define CFG_SYS_INIT_RAM_SIZE        0x80000

#define CFG_SYS_SDRAM_BASE           0x40000000
#define PHYS_SDRAM                   0x40000000
#define PHYS_SDRAM_SIZE		     SZ_1G /* 1GB DDR */

#endif
