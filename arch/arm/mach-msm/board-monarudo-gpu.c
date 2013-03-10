/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/msm_kgsl.h>
#include <mach/msm_bus_board.h>
#include <mach/board.h>
#include <mach/msm_dcvs.h>


#include "devices.h"
#include "board-monarudo.h"

#ifdef CONFIG_MSM_DCVS
static struct msm_dcvs_freq_entry grp3d_freq[] = {
       {0, 0, 333932},
       {0, 0, 497532},
       {0, 0, 707610},
       {0, 0, 844545},
};

static struct msm_dcvs_freq_entry grp2d_freq[] = {
	{0, 0, 86000},
	{0, 0, 200000},
};

static struct msm_dcvs_core_info grp3d_core_info = {
       .freq_tbl = &grp3d_freq[0],
       .core_param = {
               .max_time_us = 100000,
               .num_freq = ARRAY_SIZE(grp3d_freq),
       },
       .algo_param = {
               .slack_time_us = 39000,
               .disable_pc_threshold = 86000,
               .ss_window_size = 1000000,
               .ss_util_pct = 95,
               .em_max_util_pct = 97,
               .ss_iobusy_conv = 100,
       },
};

static struct msm_dcvs_core_info grp2d_core_info = {
	.freq_tbl = &grp2d_freq[0],
	.core_param = {
		.max_time_us = 100000,
		.num_freq = ARRAY_SIZE(grp2d_freq),
	},
	.algo_param = {
		.slack_time_us = 39000,
		.disable_pc_threshold = 90000,
		.ss_window_size = 1000000,
		.ss_util_pct = 90,
		.em_max_util_pct = 95,
	},
};
#endif /* CONFIG_MSM_DCVS */

#ifdef CONFIG_MSM_BUS_SCALING
static struct msm_bus_vectors grp3d_init_vectors[] = {
	{
		.src = MSM_BUS_MASTER_GRAPHICS_3D,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 0,
	},
	{
		.src = MSM_BUS_MASTER_GRAPHICS_3D_PORT1,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 0,
	},
};

static struct msm_bus_vectors grp3d_low_vectors[] = {
	{
		.src = MSM_BUS_MASTER_GRAPHICS_3D,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = KGSL_CONVERT_TO_MBPS(2048),
	},
	{
		.src = MSM_BUS_MASTER_GRAPHICS_3D_PORT1,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = KGSL_CONVERT_TO_MBPS(2048),
	},
};

static struct msm_bus_vectors grp3d_nominal_low_vectors[] = {
	{
		.src = MSM_BUS_MASTER_GRAPHICS_3D,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = KGSL_CONVERT_TO_MBPS(3072),
	},
	{
		.src = MSM_BUS_MASTER_GRAPHICS_3D_PORT1,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = KGSL_CONVERT_TO_MBPS(3072),
	},
};

static struct msm_bus_vectors grp3d_nominal_high_vectors[] = {
	{
		.src = MSM_BUS_MASTER_GRAPHICS_3D,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = KGSL_CONVERT_TO_MBPS(4096),
	},
	{
		.src = MSM_BUS_MASTER_GRAPHICS_3D_PORT1,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = KGSL_CONVERT_TO_MBPS(4096),
	},
};

static struct msm_bus_vectors grp3d_max_vectors[] = {
	{
		.src = MSM_BUS_MASTER_GRAPHICS_3D,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = KGSL_CONVERT_TO_MBPS(5120),
	},
	{
		.src = MSM_BUS_MASTER_GRAPHICS_3D_PORT1,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = KGSL_CONVERT_TO_MBPS(5120),
	},
};

static struct msm_bus_paths grp3d_bus_scale_usecases[] = {
	{
		ARRAY_SIZE(grp3d_init_vectors),
		grp3d_init_vectors,
	},
	{
		ARRAY_SIZE(grp3d_low_vectors),
		grp3d_low_vectors,
	},
	{
		ARRAY_SIZE(grp3d_nominal_low_vectors),
		grp3d_nominal_low_vectors,
	},
	{
		ARRAY_SIZE(grp3d_nominal_high_vectors),
		grp3d_nominal_high_vectors,
	},
	{
		ARRAY_SIZE(grp3d_max_vectors),
		grp3d_max_vectors,
	},
};

static struct msm_bus_scale_pdata grp3d_bus_scale_pdata = {
	grp3d_bus_scale_usecases,
	ARRAY_SIZE(grp3d_bus_scale_usecases),
	.name = "grp3d",
	
};

static struct msm_bus_vectors grp2d0_init_vectors[] = {
	{
		.src = MSM_BUS_MASTER_GRAPHICS_2D_CORE0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 0,
	},
};

static struct msm_bus_vectors grp2d0_nominal_vectors[] = {
	{
		.src = MSM_BUS_MASTER_GRAPHICS_2D_CORE0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = KGSL_CONVERT_TO_MBPS(1000),
	},
};

static struct msm_bus_vectors grp2d0_max_vectors[] = {
	{
		.src = MSM_BUS_MASTER_GRAPHICS_2D_CORE0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = KGSL_CONVERT_TO_MBPS(2048),
	},
};

static struct msm_bus_paths grp2d0_bus_scale_usecases[] = {
	{
		ARRAY_SIZE(grp2d0_init_vectors),
		grp2d0_init_vectors,
	},
	{
		ARRAY_SIZE(grp2d0_nominal_vectors),
		grp2d0_nominal_vectors,
	},
	{
		ARRAY_SIZE(grp2d0_max_vectors),
		grp2d0_max_vectors,
	},
};

struct msm_bus_scale_pdata grp2d0_bus_scale_pdata = {
	grp2d0_bus_scale_usecases,
	ARRAY_SIZE(grp2d0_bus_scale_usecases),
	.name = "grp2d0",
};

static struct msm_bus_vectors grp2d1_init_vectors[] = {
	{
		.src = MSM_BUS_MASTER_GRAPHICS_2D_CORE1,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 0,
	},
};

static struct msm_bus_vectors grp2d1_nominal_vectors[] = {
	{
		.src = MSM_BUS_MASTER_GRAPHICS_2D_CORE1,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = KGSL_CONVERT_TO_MBPS(1000),
	},
};

static struct msm_bus_vectors grp2d1_max_vectors[] = {
	{
		.src = MSM_BUS_MASTER_GRAPHICS_2D_CORE1,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = KGSL_CONVERT_TO_MBPS(2048),
	},
};

static struct msm_bus_paths grp2d1_bus_scale_usecases[] = {
	{
		ARRAY_SIZE(grp2d1_init_vectors),
		grp2d1_init_vectors,
	},
	{
		ARRAY_SIZE(grp2d1_nominal_vectors),
		grp2d1_nominal_vectors,
	},
	{
		ARRAY_SIZE(grp2d1_max_vectors),
		grp2d1_max_vectors,
	},
};

struct msm_bus_scale_pdata grp2d1_bus_scale_pdata = {
	grp2d1_bus_scale_usecases,
	ARRAY_SIZE(grp2d1_bus_scale_usecases),
	.name = "grp2d1",
};
#endif

static struct resource kgsl_3d0_resources[] = {
	{
		.name = KGSL_3D0_REG_MEMORY,
		.start = 0x04300000, /* GFX3D address */
		.end = 0x0431ffff,
		.flags = IORESOURCE_MEM,
	},
	{
		.name = KGSL_3D0_IRQ,
		.start = GFX3D_IRQ,
		.end = GFX3D_IRQ,
		.flags = IORESOURCE_IRQ,
	},
};

static const struct kgsl_iommu_ctx kgsl_3d0_iommu0_ctxs[] = {
       { "gfx3d_user", 0 },
       { "gfx3d_priv", 1 },
};

static const struct kgsl_iommu_ctx kgsl_3d0_iommu1_ctxs[] = {
       { "gfx3d1_user", 0 },
       { "gfx3d1_priv", 1 },
};

static struct kgsl_device_iommu_data kgsl_3d0_iommu_data[] = {
	{
                .iommu_ctxs = kgsl_3d0_iommu0_ctxs,
                .iommu_ctx_count = ARRAY_SIZE(kgsl_3d0_iommu0_ctxs),
		.physstart = 0x07C00000,
		.physend = 0x07C00000 + SZ_1M - 1,
	},
	{
                .iommu_ctxs = kgsl_3d0_iommu1_ctxs,
                .iommu_ctx_count = ARRAY_SIZE(kgsl_3d0_iommu1_ctxs),
		.physstart = 0x07D00000,
		.physend = 0x07D00000 + SZ_1M - 1,
	},
};

static struct kgsl_device_platform_data kgsl_3d0_pdata = {
	.pwrlevel = {
		{
			.gpu_freq = 500000000,
			.bus_freq = 4,
			.io_fraction = 0,
		},
		{
			.gpu_freq = 400000000,
			.bus_freq = 3,
			.io_fraction = 33,
		},
		{
			.gpu_freq = 300000000,
			.bus_freq = 2,
			.io_fraction = 100,
		},
		{
			.gpu_freq = 27000000,
			.bus_freq = 0,
		},
	},
	.init_level = 2,
	.num_levels = 4,
	.set_grp_async = NULL,
	.idle_timeout = HZ/5,
	.nap_allowed = true,
	.clk_map = KGSL_CLK_CORE | KGSL_CLK_IFACE | KGSL_CLK_MEM_IFACE,
#ifdef CONFIG_MSM_BUS_SCALING
	.bus_scale_table = &grp3d_bus_scale_pdata,
#endif
	.iommu_data = kgsl_3d0_iommu_data,
	.iommu_count = ARRAY_SIZE(kgsl_3d0_iommu_data),
	.core_info = &grp3d_core_info,
};

static struct platform_device device_kgsl_3d0 = {
	.name = "kgsl-3d0",
	.id = 0,
	.num_resources = ARRAY_SIZE(kgsl_3d0_resources),
	.resource = kgsl_3d0_resources,
	.dev = {
		.platform_data = &kgsl_3d0_pdata,
	},
};
static struct resource kgsl_2d0_resources[] = {
	{
		.name = KGSL_2D0_REG_MEMORY,
		.start = 0x04100000, /* Z180 base address */
		.end = 0x04100FFF,
		.flags = IORESOURCE_MEM,
	},
	{
		.name  = KGSL_2D0_IRQ,
		.start = GFX2D0_IRQ,
		.end = GFX2D0_IRQ,
		.flags = IORESOURCE_IRQ,
	},
};

static const struct kgsl_iommu_ctx kgsl_2d0_iommu_ctxs[] = {
	{ "gfx2d0_2d0", 0 },
};

static struct kgsl_device_iommu_data kgsl_2d0_iommu_data[] = {
	{
		.iommu_ctxs = kgsl_2d0_iommu_ctxs,
		.iommu_ctx_count = ARRAY_SIZE(kgsl_2d0_iommu_ctxs),
		.physstart = 0x07D00000,
		.physend = 0x07D00000 + SZ_1M - 1,
	},
};

static struct kgsl_device_platform_data kgsl_2d0_pdata = {
	.pwrlevel = {
		{
			.gpu_freq = 300000000,
			.bus_freq = 4,
		},
		{
			.gpu_freq = 266667000,
			.bus_freq = 3,
		},
		{
			.gpu_freq = 200000000,
			.bus_freq = 2,
		},
		{
			.gpu_freq = 96000000,
			.bus_freq = 1,
		},
		{
			.gpu_freq = 27000000,
			.bus_freq = 0,
		},
	},
	.init_level = 0,
	.num_levels = 5,
	.set_grp_async = NULL,
	.idle_timeout = HZ/10,
	.nap_allowed = true,
	.clk_map = KGSL_CLK_CORE | KGSL_CLK_IFACE,
#ifdef CONFIG_MSM_BUS_SCALING
	.bus_scale_table = &grp2d0_bus_scale_pdata,
#endif
	.iommu_data = kgsl_2d0_iommu_data,
	.iommu_count = ARRAY_SIZE(kgsl_2d0_iommu_data),
	.core_info = &grp2d_core_info,
};

struct platform_device msm_kgsl_2d0 = {
	.name = "kgsl-2d0",
	.id = 0,
	.num_resources = ARRAY_SIZE(kgsl_2d0_resources),
	.resource = kgsl_2d0_resources,
	.dev = {
		.platform_data = &kgsl_2d0_pdata,
	},
};

static const struct kgsl_iommu_ctx kgsl_2d1_iommu_ctxs[] = {
	{ "gfx2d1_2d1", 0 },
};

static struct kgsl_device_iommu_data kgsl_2d1_iommu_data[] = {
	{
		.iommu_ctxs = kgsl_2d1_iommu_ctxs,
		.iommu_ctx_count = ARRAY_SIZE(kgsl_2d1_iommu_ctxs),
		.physstart = 0x07E00000,
		.physend = 0x07E00000 + SZ_1M - 1,
	},
};

static struct resource kgsl_2d1_resources[] = {
	{
		.name = KGSL_2D1_REG_MEMORY,
		.start = 0x04200000, /* Z180 device 1 base address */
		.end =   0x04200FFF,
		.flags = IORESOURCE_MEM,
	},
	{
		.name  = KGSL_2D1_IRQ,
		.start = GFX2D1_IRQ,
		.end = GFX2D1_IRQ,
		.flags = IORESOURCE_IRQ,
	},
};

static struct kgsl_device_platform_data kgsl_2d1_pdata = {
	.pwrlevel = {
		{
			.gpu_freq = 300000000,
			.bus_freq = 4,
		},
		{
			.gpu_freq = 266667000,
			.bus_freq = 3,
		},
		{
			.gpu_freq = 200000000,
			.bus_freq = 2,
		},
		{
			.gpu_freq = 96000000,
			.bus_freq = 1,
		},
		{
			.gpu_freq = 27000000,
			.bus_freq = 0,
		},
	},
	.init_level = 0,
	.num_levels = 5,
	.set_grp_async = NULL,
	.idle_timeout = HZ/10,
	.nap_allowed = true,
	.clk_map = KGSL_CLK_CORE | KGSL_CLK_IFACE,
#ifdef CONFIG_MSM_BUS_SCALING
	.bus_scale_table = &grp2d1_bus_scale_pdata,
#endif
	.iommu_data = kgsl_2d1_iommu_data,
	.iommu_count = ARRAY_SIZE(kgsl_2d1_iommu_data),
	.core_info = &grp2d_core_info,
};

struct platform_device msm_kgsl_2d1 = {
	.name = "kgsl-2d1",
	.id = 1,
	.num_resources = ARRAY_SIZE(kgsl_2d1_resources),
	.resource = kgsl_2d1_resources,
	.dev = {
		.platform_data = &kgsl_2d1_pdata,
	},
};


#ifdef CONFIG_CMDLINE_OPTIONS
/* setters for cmdline_gpu */
int set_kgsl_3d0_freq(unsigned int freq0, unsigned int freq1)
{
	kgsl_3d0_pdata.pwrlevel[0].gpu_freq = freq0;
	kgsl_3d0_pdata.pwrlevel[1].gpu_freq = freq1;
	return 0;
}
int set_kgsl_2d0_freq(unsigned int freq)
{
	kgsl_2d0_pdata.pwrlevel[0].gpu_freq = freq;
	return 0;
}
int set_kgsl_2d1_freq(unsigned int freq)
{
	kgsl_2d1_pdata.pwrlevel[0].gpu_freq = freq;
	return 0;
}
#endif

void __init monarudo_init_gpu(void)
{
	platform_device_register(&device_kgsl_3d0);
}
