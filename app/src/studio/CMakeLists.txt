
target_sources(app PRIVATE common.c)
target_sources(app PRIVATE rpc.c)
target_sources(app PRIVATE core_subsystem.c)
target_sources_ifdef(CONFIG_ZMK_STUDIO_INTERFACE_UART app PRIVATE uart_rpc_interface.c)
target_sources_ifdef(CONFIG_ZMK_STUDIO_INTERFACE_BLE app PRIVATE gatt_rpc_interface.c)