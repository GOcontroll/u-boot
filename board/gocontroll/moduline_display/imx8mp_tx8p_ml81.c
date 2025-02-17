// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2019 NXP
 */

#include <asm/arch/sys_proto.h>
#include <env.h>

int board_init(void)
{
	return 0;
}

int board_late_init(void)
{
	board_late_mmc_env_init();

	return 0;
}
