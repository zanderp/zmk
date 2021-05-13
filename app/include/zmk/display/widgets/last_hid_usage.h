/*
 * Copyright (c) 202! The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <lvgl.h>
#include <kernel.h>

struct zmk_widget_last_hid_usage {
    sys_snode_t node;
    lv_obj_t *obj;
};

int zmk_widget_last_hid_usage_init(struct zmk_widget_last_hid_usage *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_last_hid_usage_obj(struct zmk_widget_last_hid_usage *widget);