
#pragma once

#include <zephyr/net/buf.h>

enum studio_framing_state {
    FRAMING_STATE_IDLE,
    FRAMING_STATE_AWAITING_DATA,
    FRAMING_STATE_ESCAPED,
    FRAMING_STATE_ERR,
};

#define FRAMING_SOF 0xAB
#define FRAMING_ESC 0xAC
#define FRAMING_EOF 0xAD

int studio_framing_recv_byte(struct net_buf_simple *buf, enum studio_framing_state *frame_state,
                             uint8_t data);