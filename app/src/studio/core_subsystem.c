
#include <zmk/studio/rpc.h>

ZMK_RPC_SUBSYSTEM(core)

#define CORE_RESPONSE(type, ...) ZMK_RPC_RESPONSE(core, type, __VA_ARGS__)

struct response_r get_lock_status(const struct request_r *req) {
    return CORE_RESPONSE(get_lock_state_response, {
                                                      .locked = true,
                                                  });
}

ZMK_RPC_SUBSYSTEM_HANDLER(core, core_request_get_lock_state_m_c, get_lock_status);
