/*
 * Copyright (c) 2020 Peter Johanson
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <settings/settings.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/matrix.h>
#include <zmk/kscan.h>
#include <zmk/endpoints.h>


#include <init.h>
#include <display/cfb.h>

#define ZMK_KSCAN_DEV DT_LABEL(ZMK_MATRIX_NODE_ID)
#define DISPLAY_DRIVER "SSD1306"

int cfb_init(struct device *_arg)
{
	struct device *dev;
	u16_t rows;
	u8_t ppt;
	u8_t font_width;
	u8_t font_height;
	
	dev = device_get_binding(DISPLAY_DRIVER);

	LOG_DBG("");
	if (!dev) {
		LOG_ERR("Failed to find device: %s", DISPLAY_DRIVER);
		return -EIO;
	}

	if (display_set_pixel_format(dev, PIXEL_FORMAT_MONO10) != 0) {
			LOG_ERR("Failed to set required pixel format");
			return -EIO;
	}

	LOG_DBG("initialized %s", DISPLAY_DRIVER);

	if (cfb_framebuffer_init(dev)) {
			LOG_ERR("Framebuffer initialization failed!");
			return -EIO;
	}

	cfb_framebuffer_invert(dev);

	cfb_framebuffer_clear(dev, true);

	display_blanking_off(dev);

	rows = cfb_get_display_parameter(dev, CFB_DISPLAY_ROWS);
	ppt = cfb_get_display_parameter(dev, CFB_DISPLAY_PPT);

	for (int idx = 0; idx < 42; idx++) {
			if (cfb_get_font_size(dev, idx, &font_width, &font_height)) {
					break;
			}
			cfb_framebuffer_set_font(dev, 0);
			LOG_DBG("font width %d, font height %d",
					font_width, font_height);
	}

	LOG_DBG("x_res %d, y_res %d, ppt %d, rows %d, cols %d",
			cfb_get_display_parameter(dev, CFB_DISPLAY_WIDTH),
			cfb_get_display_parameter(dev, CFB_DISPLAY_HEIGH),
			ppt,
			rows,
			cfb_get_display_parameter(dev, CFB_DISPLAY_COLS));

	for (int i = 0; i < rows; i++) {
			cfb_framebuffer_clear(dev, false);
			if (cfb_print(dev,
							"0123456789mMgj!\"§$%&/()=",
							0, i * ppt)) {
					LOG_ERR("Failed to print a string");
					continue;
			}

			cfb_framebuffer_finalize(dev);
	}

	LOG_DBG("PRINTED TO THE CFB");
	return 0;
}

void main(void)
{
	printk("Welcome to ZMK!\n");

	if (zmk_kscan_init(ZMK_KSCAN_DEV) != 0)
	{
		return;
	}

#ifdef CONFIG_SETTINGS
	settings_load();
#endif
}

SYS_INIT(cfb_init,
	APPLICATION,
	CONFIG_APPLICATION_INIT_PRIORITY);
