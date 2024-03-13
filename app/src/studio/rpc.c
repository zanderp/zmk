
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/studio/rpc.h>

zmk_Response zmk_rpc_handle_request(const zmk_Request *req) {
    LOG_WRN("Got a req of union type: %d", req->which_subsystem);
    STRUCT_SECTION_FOREACH(zmk_rpc_subsystem, sub) {
        if (sub->subsystem_choice == req->which_subsystem) {
            LOG_WRN("Found a func!");
            zmk_Response resp = sub->func(sub, req);
            resp.type.request_response.request_id = req->request_id;

            return resp;
        }
    }

    LOG_WRN("No HANDLER FOUND");
    // TODO: NOT SUPPORTED
    zmk_Response resp = zmk_Response_init_zero;
    return resp;
}

static struct zmk_rpc_subsystem *find_subsystem_for_choice(uint8_t choice) {
    STRUCT_SECTION_FOREACH(zmk_rpc_subsystem, sub) {
        if (sub->subsystem_choice == choice) {
            return sub;
        }
    }

    return NULL;
}

static int zmk_rpc_init(void) {
    int16_t prev_choice = -1;
    struct zmk_rpc_subsystem *prev_sub = NULL;
    int i = 0;
    LOG_ERR("INIT!");
    STRUCT_SECTION_FOREACH(zmk_rpc_subsystem_handler, handler) {
        struct zmk_rpc_subsystem *sub = find_subsystem_for_choice(handler->subsystem_choice);

        __ASSERT(sub != NULL, "RPC Handler for unknown subsystem choice %d",
                 handler->subsystem_choice);

        LOG_DBG("Init handler %d sub %d", handler->request_choice, sub->subsystem_choice);

        if (prev_choice < 0) {
            sub->handlers_start_index = i;
        } else if ((prev_choice != handler->subsystem_choice) && prev_sub) {
            prev_sub->handlers_end_index = i - 1;
            sub->handlers_start_index = i;
        }

        prev_choice = handler->subsystem_choice;
        prev_sub = sub;
        i++;
    }

    if (prev_sub) {
        prev_sub->handlers_end_index = i - 1;
    }

    return 0;
}

SYS_INIT(zmk_rpc_init, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);