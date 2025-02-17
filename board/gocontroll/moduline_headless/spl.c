// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2025 GOcontroll
 */

#include <command.h>
#include <cpu_func.h>
#include <hang.h>
#include <image.h>
#include <init.h>
#include <log.h>
#include <spl.h>
#include <asm/global_data.h>
#include <asm/io.h>
#include <asm/mach-imx/iomux-v3.h>
#include <asm/arch/clock.h>
#include <asm/arch/imx8mm_pins.h>
#include <asm/arch/sys_proto.h>
#include <asm/mach-imx/boot_mode.h>
#include <asm/arch/ddr.h>
#include <asm/sections.h>

#include <dm/uclass.h>
#include <dm/device.h>
#include <dm/uclass-internal.h>
#include <dm/device-internal.h>

#include <power/pmic.h>
#include <power/bd71837.h>

DECLARE_GLOBAL_DATA_PTR;

int spl_board_boot_device(enum boot_device boot_dev_spl)
{
	switch (boot_dev_spl) {
	case USB_BOOT:
		return BOOT_DEVICE_BOARD;
	case SD2_BOOT:
	case MMC2_BOOT:
		return BOOT_DEVICE_MMC1;
	case SD3_BOOT:
	case MMC3_BOOT:
		return BOOT_DEVICE_MMC2;
	default:
		return BOOT_DEVICE_NONE;
	}
}

static void spl_dram_init(void)
{
	ddr_init(&dram_timing);
}

void spl_board_init(void)
{
	arch_misc_init();
}

#ifdef CONFIG_SPL_LOAD_FIT
int board_fit_config_name_match(const char *name)
{
	/* Just empty function now - can't decide what to choose */
	debug("%s: %s\n", __func__, name);

	return 0;
}
#endif

static int power_init_board(void)
{
	struct udevice *dev;
	int ret;

	ret = pmic_get("bd71847@25", &dev);
	if (ret == -ENODEV) {
		puts("No pmic\n");
		return 0;
	}
	if (ret != 0)
		return ret;

	pmic_reg_write(dev, BD718XX_PWRONCONFIG1, 0x0);
	pmic_reg_write(dev, BD718XX_REGLOCK, BD718XX_REGLOCK_PWRSEQ);

	pmic_reg_write(dev, BD718XX_BUCK1_CTRL, 0x40);
	pmic_reg_write(dev, BD718XX_BUCK2_CTRL, 0x40);

	pmic_reg_write(dev, BD718XX_1ST_NODVS_BUCK_CTRL, 0x00);
	pmic_reg_write(dev, BD718XX_2ND_NODVS_BUCK_CTRL, 0x00);
	pmic_reg_write(dev, BD718XX_3RD_NODVS_BUCK_CTRL, 0x00);

	/* vdd soc 0.9v */
	pmic_reg_write(dev, BD718XX_BUCK1_VOLT_RUN, 0x20);

	/* vdd arm 0.9v */
	pmic_reg_write(dev, BD718XX_BUCK2_VOLT_RUN, 0x20);

	/* vdd dram 0.9v */
	pmic_reg_write(dev, BD718XX_1ST_NODVS_BUCK_VOLT, 0x02);

	/* 3v3 */
	pmic_reg_write(dev, BD718XX_2ND_NODVS_BUCK_VOLT, 0x03);

	/* 1v8 */
	pmic_reg_write(dev, BD718XX_3RD_NODVS_BUCK_VOLT, 0x03);

	/* nvcc dram 1v35 */
	pmic_reg_write(dev, BD718XX_4TH_NODVS_BUCK_VOLT, 0x1E);

	pmic_reg_write(dev, BD718XX_REGLOCK, BD718XX_REGLOCK_VREG | BD718XX_REGLOCK_PWRSEQ);

	return 0;
}

void board_init_f(ulong dummy)
{
	struct udevice *dev;
	int ret;

	arch_cpu_init();

	init_uart_clk(2);

	timer_init();

	/* Clear the BSS. */
	memset(__bss_start, 0, __bss_end - __bss_start);

	ret = spl_early_init();
	if (ret) {
		debug("spl_early_init() failed: %d\n", ret);
		hang();
	}

	ret = uclass_get_device_by_name(UCLASS_CLK,
					"clock-controller@30380000",
					&dev);
	if (ret < 0) {
		printf("Failed to find clock node. Check device tree\n");
		hang();
	}

	preloader_console_init();

	enable_tzc380();

	power_init_board();

	/* DDR initialization */
	spl_dram_init();

	board_init_r(NULL, 0);
}
