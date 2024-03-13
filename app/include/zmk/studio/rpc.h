
#pragma once

#include <zephyr/sys/iterable_sections.h>

#include <proto/zmk/studio-msgs.pb.h>

struct zmk_rpc_subsystem;

typedef zmk_Response(subsystem_func)(const struct zmk_rpc_subsystem *subsys,
                                     const zmk_Request *req);

typedef zmk_Response(rpc_func)(const zmk_Request *neq);

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
    zmk_Response subsystem_func_##prefix(const struct zmk_rpc_subsystem *subsys,                   \
                                         const zmk_Request *req) {                                 \
        uint8_t which_req = req->subsystem.prefix.which_request_type;                              \
        LOG_DBG("Got subsystem func for %d", subsys->subsystem_choice);                            \
                                                                                                   \
        for (int i = subsys->handlers_start_index; i <= subsys->handlers_end_index; i++) {         \
            struct zmk_rpc_subsystem_handler *sub_handler;                                         \
            STRUCT_SECTION_GET(zmk_rpc_subsystem_handler, i, &sub_handler);                        \
            if (sub_handler->request_choice == which_req) {                                        \
                return sub_handler->func(req);                                                     \
            }                                                                                      \
        }                                                                                          \
        LOG_ERR("No handler func found for %d", which_req);                                        \
        zmk_Response fallback_resp = zmk_Response_init_zero;                                       \
        return fallback_resp;                                                                      \
    }                                                                                              \
    STRUCT_SECTION_ITERABLE(zmk_rpc_subsystem, prefix##_subsystem) = {                             \
        .func = subsystem_func_##prefix,                                                           \
        .subsystem_choice = zmk_Request_##prefix##_tag,                                            \
    };

#define ZMK_RPC_SUBSYSTEM_HANDLER(prefix, request_id)                                              \
    STRUCT_SECTION_ITERABLE(zmk_rpc_subsystem_handler,                                             \
                            prefix##_subsystem_handler_##request_id) = {                           \
        .func = request_id,                                                                        \
        .subsystem_choice = zmk_Request_##prefix##_tag,                                            \
        .request_choice = zmk_##prefix##_response_##request_id##_tag,                              \
    };

#define ZMK_RPC_RESPONSE(subsys, _type, ...)                                                       \
    ((zmk_Response){                                                                               \
        .which_type = zmk_Response_request_response_tag,                                           \
        .type =                                                                                    \
            {                                                                                      \
                .request_response =                                                                \
                    {                                                                              \
                        .which_subsystem = zmk_RequestResponse_##subsys##_tag,                     \
                        .subsystem =                                                               \
                            {                                                                      \
                                .subsys =                                                          \
                                    {                                                              \
                                        .which_response_type =                                     \
                                            zmk_##subsys##_response_##_type##_tag,                 \
                                        .response_type = {._type = __VA_ARGS__},                   \
                                    },                                                             \
                            },                                                                     \
                    },                                                                             \
            },                                                                                     \
    })

zmk_Response zmk_rpc_handle_request(const zmk_Request *req);
