/*
 * Copyright (c) 2023 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/led.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>

#include <zmk/indicator_layerone.h>

#include <zmk/activity.h>
#include <zmk/keymap.h>
#include <zmk/usb.h>
#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

BUILD_ASSERT(
    DT_HAS_CHOSEN(zmk_indicator_layerone),
    "CONFIG_ZMK_INDICATOR_LAYERONE is enabled but no zmk,indicator_layerone chosen node found");

static const struct device *const indicator_layerone_dev =
    DEVICE_DT_GET(DT_CHOSEN(zmk_indicator_layerone));

#define CHILD_COUNT(...) +1
#define DT_NUM_CHILD(node_id) (DT_FOREACH_CHILD(node_id, CHILD_COUNT))

#define INDICATOR_LAYERONE_NUM_LEDS (DT_NUM_CHILD(DT_CHOSEN(zmk_indicator_layerone)))

#define BRT_MAX 100

struct indicator_layerone_state {
    uint8_t brightness;
    bool on;
};

static struct indicator_layerone_state state_layerone = {
    .brightness = CONFIG_ZMK_INDICATOR_LAYERONE_BRT_START,
    .on = IS_ENABLED(CONFIG_ZMK_INDICATOR_LAYERONE_ON_START)};

static int zmk_indicator_layerone_update() {
    uint8_t layerone_brt = zmk_indicator_layerone_get_brt();
    LOG_DBG("Update indicator_layerone brightness: %d%%", layerone_brt);

    for (int i = 0; i < INDICATOR_LAYERONE_NUM_LEDS; i++) {
        int rc = led_set_brightness(indicator_layerone_dev, i, layerone_brt);
        if (rc != 0) {
            LOG_ERR("Failed to update indicator_layerone LED %d: %d", i, rc);
            return rc;
        }
    }
    return 0;
}

#if IS_ENABLED(CONFIG_SETTINGS)
static int indicator_layerone_settings_load_cb(const char *name, size_t len,
                                               settings_read_cb read_cb, void *cb_arg,
                                               void *param) {
    const char *next;
    if (settings_name_steq(name, "state", &next) && !next) {
        if (len != sizeof(state_layerone)) {
            return -EINVAL;
        }

        int rc = read_cb(cb_arg, &state_layerone, sizeof(state_layerone));
        return MIN(rc, 0);
    }
    return -ENOENT;
}

static void indicator_layerone_save_work_handler(struct k_work *work) {
    settings_save_one("indicator_layerone/state", &state_layerone, sizeof(state_layerone));
}

static struct k_work_delayable indicator_layerone_save_work;
#endif

static int zmk_indicator_layerone_init(void) {
    if (!device_is_ready(indicator_layerone_dev)) {
        LOG_ERR("Backlight device \"%s\" is not ready", indicator_layerone_dev->name);
        return -ENODEV;
    }

#if IS_ENABLED(CONFIG_SETTINGS)
    settings_subsys_init();
    int rc = settings_load_subtree_direct("indicator_layerone", indicator_layerone_settings_load_cb,
                                          NULL);
    if (rc != 0) {
        LOG_ERR("Failed to load indicator_layerone settings: %d", rc);
    }
    k_work_init_delayable(&indicator_layerone_save_work, indicator_layerone_save_work_handler);
#endif
    return zmk_indicator_layerone_update();
}

uint8_t zmk_indicator_layerone_get_brt() {
    return state_layerone.on ? state_layerone.brightness : 0;
}

#if IS_ENABLED(CONFIG_ZMK_INDICATOR_LAYERONE_AUTO_OFF_SWITCH_LAYERONE)
static int indicator_layerone_auto_state(bool *prev_state, bool new_state) {
    if (state_layerone.on == new_state) {
        return 0;
    }
    state_layerone.on = new_state && *prev_state;
    *prev_state = !new_state;
    return zmk_indicator_layerone_update();
}

static int indicator_layerone_event_listener(const zmk_event_t *eh) {

#if IS_ENABLED(CONFIG_ZMK_INDICATOR_LAYERONE_AUTO_OFF_SWITCH_LAYERONE)
    if (as_zmk_layer_state_changed(eh)) {
        static bool prev_state = false;
        return indicator_layerone_auto_state(&prev_state, zmk_keymap_highest_layer_active() == 1);
    }
#endif

    return -ENOTSUP;
}

ZMK_LISTENER(indicator_layerone, indicator_layerone_event_listener);
#endif

#if IS_ENABLED(CONFIG_ZMK_INDICATOR_LAYERONE_AUTO_OFF_SWITCH_LAYERONE)
ZMK_SUBSCRIPTION(indicator_layerone, zmk_layer_state_changed);
#endif

SYS_INIT(zmk_indicator_layerone_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
