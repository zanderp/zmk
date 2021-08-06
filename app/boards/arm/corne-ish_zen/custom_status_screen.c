/*
*
* Copyright (c) 2021 Darryl deHaan
* SPDX-License-Identifier: MIT
*
*/


#include "widgets/battery_status.h"
#include "widgets/output_status.h"
#include "widgets/layer_status.h"

#include <logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

LV_IMG_DECLARE(zenlogo);

#if IS_ENABLED(CONFIG_CUSTOM_WIDGET_BATTERY_STATUS)
static struct zmk_widget_battery_status battery_status_widget;
#endif

#if IS_ENABLED(CONFIG_CUSTOM_WIDGET_OUTPUT_STATUS)
static struct zmk_widget_output_status output_status_widget;
#endif

#if IS_ENABLED(CONFIG_CUSTOM_WIDGET_LAYER_STATUS)
static struct zmk_widget_layer_status layer_status_widget;
#endif

lv_obj_t *zmk_display_status_screen() {

    lv_obj_t *screen;
    screen = lv_obj_create(NULL, NULL);

#if IS_ENABLED(CONFIG_CUSTOM_WIDGET_BATTERY_STATUS)
    zmk_widget_battery_status_init(&battery_status_widget, screen);
    lv_obj_align(zmk_widget_battery_status_obj(&battery_status_widget), NULL, LV_ALIGN_IN_TOP_MID,
                 0, 2);
#endif

#if IS_ENABLED(CONFIG_CUSTOM_WIDGET_OUTPUT_STATUS)
    zmk_widget_output_status_init(&output_status_widget, screen);
    lv_obj_align(zmk_widget_output_status_obj(&output_status_widget), NULL, LV_ALIGN_IN_TOP_MID, 0,
                 40);
#endif

#if IS_ENABLED(CONFIG_CUSTOM_WIDGET_LAYER_STATUS)
    zmk_widget_layer_status_init(&layer_status_widget, screen);
    lv_obj_align(zmk_widget_layer_status_obj(&layer_status_widget), NULL, LV_ALIGN_IN_BOTTOM_MID, 0,
                 -3);
#endif

#if CONFIG_BOARD_CORNEISH_ZEN_RIGHT
    lv_obj_t * zenlogo_icon;
    zenlogo_icon = lv_img_create(screen, NULL);
    lv_img_set_src(zenlogo_icon, &zenlogo);
    lv_obj_align(zenlogo_icon, NULL, LV_ALIGN_IN_BOTTOM_MID, 2, -5);
#endif

#if CONFIG_BOARD_CORNEISH_ZEN_LEFT
    lv_obj_t * LayersLabel;
    lv_style_t LayersLabelStyle;
    lv_style_init(&LayersLabelStyle);
    lv_style_set_text_color(&LayersLabelStyle, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&LayersLabelStyle, LV_STATE_DEFAULT, &lv_font_montserrat_16);
    /*lv_style_set_text_letter_space(&LayersLabelStyle, LV_STATE_DEFAULT, 1);
    lv_style_set_text_line_space(&LayersLabelStyle, LV_STATE_DEFAULT, 1); */
    LayersLabel = lv_label_create(screen, NULL);
    lv_obj_add_style(LayersLabel, LV_LABEL_PART_MAIN, &LayersLabelStyle);
    lv_label_set_text(LayersLabel, "LAYER");
    lv_label_set_align(LayersLabel, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(LayersLabel, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -12);
#endif

    return screen;
}