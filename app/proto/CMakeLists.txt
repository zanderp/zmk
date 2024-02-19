


include(${CMAKE_CURRENT_LIST_DIR}/zmk/studio-msgs.cmake)
target_include_directories(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/zmk/include)
target_link_libraries(app PRIVATE studio-msgs)
