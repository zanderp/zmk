
set (zcbor_command
    python3 ${ZEPHYR_ZCBOR_MODULE_DIR}/zcbor/zcbor.py code
    --decode --encode
    --short-names
    -c ${CMAKE_CURRENT_LIST_DIR}/studio-msgs.cddl
    -t request response
    --output-c ${CMAKE_CURRENT_LIST_DIR}/src/studio-msgs.c
    --output-h ${CMAKE_CURRENT_LIST_DIR}/include/studio-msgs.h
    )

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_LIST_DIR}/src/studio-msgs_decode.c"
    "${CMAKE_CURRENT_LIST_DIR}/src/studio-msgs_encode.c"
    "${CMAKE_CURRENT_LIST_DIR}/include/studio-msgs_encode.h"
    "${CMAKE_CURRENT_LIST_DIR}/include/studio-msgs_decode.h"
    "${CMAKE_CURRENT_LIST_DIR}/include/studio-msgs_types.h"
  COMMAND ${zcbor_command}
  DEPENDS "${CMAKE_CURRENT_LIST_DIR}/studio-msgs.cddl"
  VERBATIM)

zephyr_library_named(studio-msgs)
zephyr_library_sources(
    ${CMAKE_CURRENT_LIST_DIR}/../../../modules/lib/zcbor/src/zcbor_decode.c
    ${CMAKE_CURRENT_LIST_DIR}/../../../modules/lib/zcbor/src/zcbor_encode.c
    ${CMAKE_CURRENT_LIST_DIR}/../../../modules/lib/zcbor/src/zcbor_common.c
    ${CMAKE_CURRENT_LIST_DIR}/src/studio-msgs_decode.c
    ${CMAKE_CURRENT_LIST_DIR}/src/studio-msgs_encode.c
    )
zephyr_library_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/../../../modules/lib/zcbor/include
    )
