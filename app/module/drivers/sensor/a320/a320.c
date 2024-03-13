#define DT_DRV_COMPAT avago_a320

#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/crc.h>
#include <zephyr/logging/log.h>

#include "a320.h"

LOG_MODULE_REGISTER(A320, CONFIG_SENSOR_LOG_LEVEL);
K_THREAD_STACK_DEFINE(a320_stack_area, 512);

struct k_thread a320_thread_data;
k_tid_t a320_tid;

static int a320_write_reg(const struct device *dev, uint8_t reg_addr, uint8_t value) {
    const struct a320_config *cfg = dev->config;

    if (!i2c_reg_write_byte_dt(&cfg->bus, reg_addr, value)) {
        return 0;
    }

    LOG_ERR("failed to write 0x%x to 0x%x", value, reg_addr);
    return -1;
}
static int a320_read_reg(const struct device *dev, uint8_t reg_addr) {
    const struct a320_config *cfg = dev->config;
    uint8_t value = 0;

    if (!i2c_reg_read_byte_dt(&cfg->bus, reg_addr, &value)) {
        return value;
    }

    LOG_ERR("failed to read 0x%x register", reg_addr);
    return -1;
}

static int a320_sample_fetch(const struct device *dev, enum sensor_channel chan) { return 0; }

static int a320_channel_get(const struct device *dev, enum sensor_channel chan,
                            struct sensor_value *val) {
    const uint8_t ifmotion = a320_read_reg(dev, Motion);
    const uint8_t ovflow = a320_read_reg(dev, Motion);
    if ((ifmotion & BIT_MOTION_MOT) && !(ovflow & BIT_MOTION_OVF)) {
        val->val1 = a320_read_reg(dev, Delta_X);
        val->val2 = a320_read_reg(dev, Delta_Y);
        LOG_DBG("x value : %d , y value : %d\r\n", val->val1, val->val2);
        return 0;
    }
    return -1;
}

static const struct sensor_driver_api a320_driver_api = {
    .sample_fetch = a320_sample_fetch,
    .channel_get = a320_channel_get,
};

void a320_polling_thread_entry(void *arg, void *arg1, void *arg2) {
    const struct device *dev = (const struct device *)arg;
    struct a320_config *cfg = dev->config;
    int polling_ms = (1.0 / (float)CONFIG_INPUT_A320_POLLINGRATE) * 1000.0;
    polling_ms = (polling_ms <= 0) ? 1 : polling_ms;
    while (1) {
        k_mutex_lock(&cfg->polling_mutex, K_FOREVER);
        struct sensor_value val;
        a320_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &val);
        k_mutex_unlock(&cfg->polling_mutex);
        k_sleep(K_MSEC(polling_ms));
    }
}

static int a320_init(const struct device *dev) {

    struct a320_config *cfg = dev->config;

    k_mutex_init(&cfg->polling_mutex);
    if (!device_is_ready(cfg->bus.bus)) {
        LOG_ERR("I2C bus %s is not ready!", cfg->bus.bus->name);
        return -EINVAL;
    }
    a320_read_reg(dev, Motion);
    a320_read_reg(dev, Motion);
    a320_read_reg(dev, Delta_X);
    a320_read_reg(dev, Delta_Y);
    // 15. LED_Control
    // ...

    a320_tid =
        k_thread_create(&a320_thread_data, a320_stack_area, K_THREAD_STACK_SIZEOF(a320_stack_area),
                        a320_polling_thread_entry, (void *)dev, NULL, NULL, 5, 0, K_NO_WAIT);
    return 0;
}

#define A320_DEFINE(inst)                                                                          \
    struct a320_data a3200_data_##inst;                                                            \
    static const struct a320_config a3200_cfg_##inst = {.bus = I2C_DT_SPEC_INST_GET(inst)};        \
    DEVICE_DT_INST_DEFINE(inst, a320_init, NULL, &a3200_data_##inst, &a3200_cfg_##inst,            \
                          POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY, &a320_driver_api);

DT_INST_FOREACH_STATUS_OKAY(A320_DEFINE)
