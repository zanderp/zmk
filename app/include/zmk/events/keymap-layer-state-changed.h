/*
 * Copyright (c) 2020 Peter Johanson <peter@peterjohanson.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr.h>
#include <zmk/event-manager.h>

struct keymap_layer_state_data
{
    u32_t state;
    u8_t default_layer;
};

struct keymap_layer_state_changed {
    struct zmk_event_header header;
    struct keymap_layer_state_data data;
};

ZMK_EVENT_DECLARE(keymap_layer_state_changed);