#define DT_DRV_COMPAT trackpad9900

#include <init.h>
#include <drivers/sensor.h>
#include <logging/log.h>

#include "trackpad9900.h"

#define Trackpad_ADDR 0x3B

// Registers
#define REG_PID 0x00
#define REG_REV 0x01
#define REG_MOTION 0x02
#define REG_DELTA_X 0x03
#define REG_DELTA_Y 0x04
#define REG_DELTA_XY_H 0x05
#define REG_CONFIG 0x11
#define REG_OBSERV 0x2E
#define REG_MBURST 0x42

LOG_MODULE_REGISTER(trackpad9900, CONFIG_SENSOR_LOG_LEVEL);

static int trackpad9900_seq_read(const struct device *dev, const uint8_t addr, uint8_t *buf,
                                 const uint8_t len) {
    const struct trackpad9900_config *config = dev->config;
