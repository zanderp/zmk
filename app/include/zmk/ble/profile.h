/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <bluetooth/addr.h>
#include <bluetooth/gatt.h>

#define ZMK_BLE_PROFILE_NAME_MAX 15

enum zmk_ble_profile_status {
    ZMK_BLE_PROFILE_STATUS_DISCONNECTED,
    ZMK_BLE_PROFILE_STATUS_ADVERTISING,
    ZMK_BLE_PROFILE_STATUS_CONNECTED,
};
struct zmk_ble_profile {
    char name[ZMK_BLE_PROFILE_NAME_MAX];
    bt_addr_le_t peer;
    zmk_ble_profile_status status;
};
