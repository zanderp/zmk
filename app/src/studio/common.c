

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include "common.h"

int studio_framing_recv_byte(struct net_buf_simple *buf,
                             enum studio_framing_state *rpc_framing_state, uint8_t c) {
    switch (*rpc_framing_state) {
    case FRAMING_STATE_ERR:
        switch (c) {
        case FRAMING_EOF:
            net_buf_simple_reset(buf);
            *rpc_framing_state = FRAMING_STATE_IDLE;
            break;
        case FRAMING_SOF:
            net_buf_simple_reset(buf);
            *rpc_framing_state = FRAMING_STATE_AWAITING_DATA;
            break;
        default:
            LOG_WRN("Discarding unexpected data 0x%02x", c);
            break;
        }
        break;
    case FRAMING_STATE_IDLE:
        switch (c) {
        case FRAMING_SOF:
            *rpc_framing_state = FRAMING_STATE_AWAITING_DATA;
            break;
        default:
            LOG_WRN("Expected SOF, got 0x%02x", c);
            break;
        }
        break;
    case FRAMING_STATE_AWAITING_DATA:
        switch (c) {
        case FRAMING_SOF:
            LOG_WRN("Enescaped SOF mid-data");
            *rpc_framing_state = FRAMING_STATE_ERR;
            break;
        case FRAMING_ESC:
            *rpc_framing_state = FRAMING_STATE_ESCAPED;
            break;
        case FRAMING_EOF:
            LOG_WRN("GOT A FULL MSG");
            LOG_HEXDUMP_DBG(buf->data, buf->len, "MSG");

            *rpc_framing_state = FRAMING_STATE_IDLE;
            break;
        default:
            net_buf_simple_add_u8(buf, c);
            break;
        }

        break;
    case FRAMING_STATE_ESCAPED:
        *rpc_framing_state = FRAMING_STATE_AWAITING_DATA;
        net_buf_simple_add_u8(buf, c);
        break;
    }

    return 0;
}

int studio_framing_encode_frame(const uint8_t *buf, size_t len, uint8_t *dest, size_t dest_size) {
    int added = 0;

    dest[added++] = FRAMING_SOF;
    for (int i = 0; i < len && added < dest_size; i++) {
        switch (buf[i]) {
        case FRAMING_SOF:
        case FRAMING_ESC:
        case FRAMING_EOF:
            dest[added++] = FRAMING_ESC;
            if (added == dest_size) {
                return -EIO;
            }
            // Flow to default to add the actual byte
        default:
            dest[added++] = buf[i];
        }
    }

    dest[added++] = FRAMING_EOF;

    return added;
}