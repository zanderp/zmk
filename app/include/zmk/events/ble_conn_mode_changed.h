/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr/kernel.h>
#include <zmk/event_manager.h>

enum zmk_ble_conn_mode {
    ZMK_BLE_CONN_MODE_NONE,
    ZMK_BLE_CONN_MODE_CONNECTED,
    ZMK_BLE_CONN_MODE_ADV,
    ZMK_BLE_CONN_MODE_ADV_PAIRING,
    ZMK_BLE_CONN_MODE_ADV_PEER,
    /* Sub-states of peer advertising */
    ZMK_BLE_CONN_MODE_ADV_PEER_INDIR,
    ZMK_BLE_CONN_MODE_ADV_PEER_DIR,
    ZMK_BLE_CONN_MODE_ADV_PEER_DIR_LOW_DUTY,
    ZMK_BLE_CONN_MODE_SECURING,
    ZMK_BLE_CONN_MODE_SECURING_SET_L2,
    ZMK_BLE_CONN_MODE_SECURING_BONDING,
};

struct zmk_ble_conn_mode_changed {
    enum zmk_ble_conn_mode mode;
};

ZMK_EVENT_DECLARE(zmk_ble_conn_mode_changed);
