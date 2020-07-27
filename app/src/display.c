/*
 * Copyright (c) 2020 Peter Johanson
 *
 * SPDX-License-Identifier: MIT
 */

#include <init.h>
#include <device.h>
#include <devicetree.h>

#include <logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <drivers/display.h>
#include <lvgl.h>

#define ZMK_DISPLAY_NAME CONFIG_LVGL_DISPLAY_DEV_NAME

static struct device *display;

static lv_obj_t *screen;

#ifdef CONFIG_ZMK_DISPLAY_LOGO
extern const lv_img_dsc_t zmk_logo;
#endif

int zmk_display_init()
{
    lv_obj_t *zmk_brand_lbl;
    lv_obj_t *keyboard_name_lbl;
    lv_obj_t *outer_container;
    lv_obj_t *lbl_cont;
#ifdef CONFIG_ZMK_DISPLAY_LOGO
    lv_obj_t *logo_img;
#endif 

    LOG_DBG("");

    display = device_get_binding(ZMK_DISPLAY_NAME);
    if (display == NULL) {
        LOG_ERR("Failed to find display device");
        return -EINVAL;
    }

    screen = lv_obj_create(NULL, NULL);
    lv_scr_load(screen);

    outer_container = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_style(outer_container, &lv_style_transp_tight);
    lv_cont_set_layout(outer_container, LV_LAYOUT_ROW_M);
    lv_cont_set_fit(outer_container, LV_FIT_FLOOD);

#ifdef CONFIG_ZMK_DISPLAY_LOGO
    logo_img = lv_img_create(outer_container, NULL);
    lv_img_set_src(logo_img, &zmk_logo);
    lv_obj_set_style(logo_img, &lv_style_transp_tight);
#endif 

    lbl_cont = lv_cont_create(outer_container, NULL);
    lv_obj_set_style(lbl_cont, &lv_style_transp_tight);
    lv_cont_set_layout(lbl_cont, LV_LAYOUT_COL_M);
    lv_cont_set_fit(lbl_cont, LV_FIT_FLOOD);
    zmk_brand_lbl = lv_label_create(lbl_cont, NULL);
    lv_label_set_text(zmk_brand_lbl, "ZMK");
    keyboard_name_lbl = lv_label_create(lbl_cont, NULL);
    lv_label_set_long_mode(keyboard_name_lbl, LV_LABEL_LONG_BREAK);
    lv_label_set_text(keyboard_name_lbl, CONFIG_ZMK_KEYBOARD_NAME);
    lv_task_handler();
    display_blanking_off(display);

    return 0;
}

void zmk_display_task_handler()
{
    lv_tick_inc(10);
    lv_task_handler();
    k_sleep(K_MSEC(10));
}
