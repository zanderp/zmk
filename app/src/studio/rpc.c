
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/studio/rpc.h>

struct response_r zmk_rpc_handle_request(const struct request_r *req) {
    STRUCT_SECTION_FOREACH(zmk_rpc_subsystem, sub) {
        if (sub->subsystem_choice == req->request_choice) {
            return sub->func(sub, req);
        }
    }
    // TODO: NOT SUPPORTED
    return (struct response_r){

    };
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
    STRUCT_SECTION_FOREACH(zmk_rpc_subsystem_handler, handler) {
        struct zmk_rpc_subsystem *sub = find_subsystem_for_choice(handler->subsystem_choice);

        if (sub) {
            if (prev_choice < 0) {
                sub->handlers_start_index = i;
            } else if ((prev_choice != handler->subsystem_choice) && prev_sub) {
                prev_sub->handlers_end_index = i - 1;
            }

            continue;
        } else {
            LOG_ERR("Handler registered for choice %d with no match RPC subsystem",
                    handler->subsystem_choice);
            return -ENODEV;
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