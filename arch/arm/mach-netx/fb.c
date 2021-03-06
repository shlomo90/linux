// SPDX-License-Identifier: GPL-2.0-only
/*
 * arch/arm/mach-netx/fb.c
 *
 * Copyright (c) 2005 Sascha Hauer <s.hauer@pengutronix.de>, Pengutronix
 */

#include <linux/device.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/amba/bus.h>
#include <linux/amba/clcd.h>
#include <linux/err.h>
#include <linux/gfp.h>

#include <asm/irq.h>

#include <mach/netx-regs.h>
#include <mach/hardware.h>

static struct clcd_panel *netx_panel;

void netx_clcd_enable(struct clcd_fb *fb)
{
}

int netx_clcd_setup(struct clcd_fb *fb)
{
	dma_addr_t dma;

	fb->panel = netx_panel;

	fb->fb.screen_base = dma_alloc_wc(&fb->dev->dev, 1024 * 1024, &dma,
					  GFP_KERNEL);
	if (!fb->fb.screen_base) {
		printk(KERN_ERR "CLCD: unable to map framebuffer\n");
		return -ENOMEM;
	}

	fb->fb.fix.smem_start	= dma;
	fb->fb.fix.smem_len	= 1024*1024;

	return 0;
}

int netx_clcd_mmap(struct clcd_fb *fb, struct vm_area_struct *vma)
{
	return dma_mmap_wc(&fb->dev->dev, vma, fb->fb.screen_base,
			   fb->fb.fix.smem_start, fb->fb.fix.smem_len);
}

void netx_clcd_remove(struct clcd_fb *fb)
{
	dma_free_wc(&fb->dev->dev, fb->fb.fix.smem_len, fb->fb.screen_base,
		    fb->fb.fix.smem_start);
}

static AMBA_AHB_DEVICE(fb, "fb", 0, 0x00104000, { NETX_IRQ_LCD }, NULL);

int netx_fb_init(struct clcd_board *board, struct clcd_panel *panel)
{
	netx_panel = panel;
	fb_device.dev.platform_data = board;
	return amba_device_register(&fb_device, &iomem_resource);
}
