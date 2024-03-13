
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/studio/rpc.h>

ZMK_RPC_SUBSYSTEM(core)

#define CORE_RESPONSE(type, ...) ZMK_RPC_RESPONSE(core, type, __VA_ARGS__)

zmk_Response get_lock_status(const zmk_Request *req) {
    zmk_core_get_lock_status_response resp = zmk_core_get_lock_status_response_init_zero;
    resp.locked = true;

    return CORE_RESPONSE(get_lock_status, resp);
}

ZMK_RPC_SUBSYSTEM_HANDLER(core, get_lock_status);
