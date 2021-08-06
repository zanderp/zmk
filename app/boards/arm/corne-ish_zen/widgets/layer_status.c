/*
*
* Copyright (c) 2021 Darryl deHaan
* SPDX-License-Identifier: MIT
*
*/

#include <logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include "layer_status.h"
#include <zmk/events/layer_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/endpoints.h>
#include <zmk/keymap.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);
static lv_style_t label_style;
//static lv_style_t label_style2;

static bool style_initialized = false;

//LV_IMG_DECLARE(layers);

void layer_status_init() {
    if (style_initialized) {
        return;
    }

    style_initialized = true;
    lv_style_init(&label_style);
    lv_style_set_text_color(&label_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&label_style, LV_STATE_DEFAULT, &lv_font_montserrat_16);
    lv_style_set_text_letter_space(&label_style, LV_STATE_DEFAULT, 1);
    lv_style_set_text_line_space(&label_style, LV_STATE_DEFAULT, 1);
    /*
    lv_style_init(&label_style2);
    lv_style_set_text_color(&label_style2, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_font(&label_style2, LV_STATE_DEFAULT, &lv_font_montserrat_20);
    lv_style_set_text_letter_space(&label_style2, LV_STATE_DEFAULT, 1);
    lv_style_set_text_line_space(&label_style2, LV_STATE_DEFAULT, 1);
    lv_style_set_bg_color(&label_style2, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    */
}

void set_layer_symbol(lv_obj_t *label) {
    int active_layer_index = zmk_keymap_highest_layer_active();

    LOG_DBG("Layer changed to %i", active_layer_index);

    const char *layer_label = zmk_keymap_layer_label(active_layer_index);
    if (layer_label == NULL) {
        char text[6] = {};

        sprintf(text, " %i", active_layer_index);
        //lv_img_set_src(icon, &layers);

        lv_label_set_text(label, text);
    } else {
      
        //lv_img_set_src(icon, &layers);
        //lv_label_set_text(label, "LAYER\n");
        lv_label_set_text(label, layer_label);
    }
}

int zmk_widget_layer_status_init(struct zmk_widget_layer_status *widget, lv_obj_t *parent) {
    layer_status_init();
    //widget->obj = lv_img_create(parent, NULL);
    widget->obj = lv_label_create(parent, NULL);
    lv_obj_add_style(widget->obj, LV_LABEL_PART_MAIN, &label_style);
    //lv_obj_add_style(widget->obj2, LV_LABEL_PART_MAIN, &label_style);

    //lv_obj_set_size(widget->obj2, 40, 15);
    //lv_obj_align(widget->obj, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -1);
    //lv_obj_align(widget->obj2, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -4);
    //set_layer_symbol(widget->obj, widget->obj2);
    set_layer_symbol(widget->obj);

    sys_slist_append(&widgets, &widget->node); 

    return 0;
}

lv_obj_t *zmk_widget_layer_status_obj(struct zmk_widget_layer_status *widget) {
    return widget->obj;
}

int layer_status_listener(const zmk_event_t *eh) {
    struct zmk_widget_layer_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_layer_symbol(widget->obj); }
    return 0;
}

ZMK_LISTENER(widget_layer_status, layer_status_listener)
ZMK_SUBSCRIPTION(widget_layer_status, zmk_layer_state_changed);