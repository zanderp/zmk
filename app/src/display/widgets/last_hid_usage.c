/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display/widgets/last_hid_usage.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/event_manager.h>
#include <dt-bindings/zmk/hid_usage_pages.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

uint8_t map_usage_to_ascii(uint32_t usage) {
    if (usage >= HID_USAGE_KEY_KEYBOARD_A && usage <= HID_USAGE_KEY_KEYBOARD_Z) {
        return usage + 0x5D;
    }

    return '?';
}

void set_usage(lv_obj_t *label, uint32_t usage) {
    char text[2] = {map_usage_to_ascii(usage), 0};

    LOG_DBG("usage changed changed to %s", text);

    lv_label_set_text(label, text);
}

int zmk_widget_last_hid_usage_init(struct zmk_widget_last_hid_usage *widget, lv_obj_t *parent) {
    widget->obj = lv_label_create(parent, NULL);
    lv_label_set_align(widget->obj, LV_LABEL_ALIGN_RIGHT);

    lv_obj_set_size(widget->obj, 40, 15);
    set_usage(widget->obj, HID_USAGE_KEY_KEYBOARD_SPACEBAR);

    sys_slist_append(&widgets, &widget->node);

    return 0;
}

lv_obj_t *zmk_widget_last_hid_usage_obj(struct zmk_widget_last_hid_usage *widget) {
    return widget->obj;
}

int last_hid_usage_listener(const zmk_event_t *eh) {
    struct zmk_keycode_state_changed *ev = as_zmk_keycode_state_changed(eh);
    if (ev->usage_page != HID_USAGE_KEY) {
        return 0;
    }

    struct zmk_widget_last_hid_usage *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_usage(widget->obj, ev->keycode); }
    return 0;
}

ZMK_LISTENER(widget_last_hid_usage, last_hid_usage_listener)
ZMK_SUBSCRIPTION(widget_last_hid_usage, zmk_keycode_state_changed);