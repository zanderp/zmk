/*
 * Copyright (c) 2020 Peter Johanson <peter@peterjohanson.com>
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_bluetooth

#include <device.h>
#include <drivers/behavior.h>

#include <dt-bindings/zmk/bt.h>

#include <bluetooth/conn.h>

#include <logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static int behavior_bt_reset()
{
  bt_unpair(BT_ID_DEFAULT, NULL);
  return 0;
}

static int on_keymap_binding_pressed(struct device *dev, u32_t position, u32_t action, u32_t _)
{
  switch (action)
  {
    case BT_RST:
      return behavior_bt_reset();
  }

  return -ENOTSUP;
}

static const struct behavior_driver_api behavior_bt_driver_api = {
  .binding_pressed = on_keymap_binding_pressed,
};

DEVICE_AND_API_INIT(behavior_bt, DT_INST_LABEL(0),
                    NULL,
                    NULL,
                    NULL,
                    APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
                    &behavior_bt_driver_api);