
#pragma once

#include <zephyr/sys/iterable_sections.h>
#include <studio-msgs_types.h>

struct zmk_rpc_subsystem;

typedef struct response_r(subsystem_func)(const struct zmk_rpc_subsystem *subsys,
                                          const struct request *req);

typedef struct response_r(rpc_func)(const struct request *req);

struct zmk_rpc_subsystem {
    subsystem_func *func;
    uint16_t handlers_start_index;
    uint16_t handlers_end_index;
    uint8_t subsystem_choice;
};

struct zmk_rpc_subsystem_handler {
    rpc_func *func;
    uint8_t subsystem_choice;
    uint8_t request_choice;
};

#define ZMK_RPC_SUBSYSTEM(prefix)                                                                  \
    struct response_r subsystem_func_##prefix(const struct zmk_rpc_subsystem *subsys,              \
                                              const struct request *req) {                         \
        uint8_t choice = req->prefix##_subsystem_m.prefix##_request_m.prefix##_request_choice;     \
                                                                                                   \
        for (int i = subsys->handlers_start_index; i <= subsys->handlers_end_index; i++) {         \
            struct zmk_rpc_subsystem_handler *sub_handler;                                         \
            STRUCT_SECTION_GET(zmk_rpc_subsystem_handler, i, &sub_handler);                        \
            if (sub_handler->request_choice == choice) {                                           \
                return sub_handler->func(req);                                                     \
            }                                                                                      \
        }                                                                                          \
        LOG_ERR("No handler func found for %d", choice);                                           \
        return (struct response_r){};                                                              \
    }                                                                                              \
    STRUCT_SECTION_ITERABLE(zmk_rpc_subsystem, prefix##_subsystem) = {                             \
        .func = subsystem_func_##prefix,                                                           \
        .subsystem_choice = request_union_##prefix##_subsystem_m_c,                                \
    };

#define ZMK_RPC_SUBSYSTEM_HANDLER(prefix, request_choice_val, func_val)                            \
    STRUCT_SECTION_ITERABLE(zmk_rpc_subsystem_handler,                                             \
                            prefix##_subsystem_handler_##request_choice_val) = {                   \
        .func = func_val,                                                                          \
        .subsystem_choice = request_union_##prefix##_subsystem_m_c,                                \
        .request_choice = request_choice_val,                                                      \
    };

#define ZMK_RPC_RESPONSE(subsys, type, ...)                                                        \
    ((struct response_r){                                                                          \
        .response_choice = request_response_m_c,                                                   \
        .request_response_m =                                                                      \
            {                                                                                      \
                .response_payload_m =                                                              \
                    {                                                                              \
                        .response_payload_choice = response_payload_##subsys##_response_m_c,       \
                        .subsys##_response_m =                                                     \
                            {                                                                      \
                                .union_choice = subsys##_response_union_##type##_m_c,              \
                                .type##_m = __VA_ARGS__,                                           \
                            },                                                                     \
                    },                                                                             \
            },                                                                                     \
    })

struct response_r zmk_rpc_handle_request(const struct request *req);
