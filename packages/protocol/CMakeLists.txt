add_library(jerv_protocol INTERFACE)

target_include_directories(jerv_protocol INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(jerv_protocol INTERFACE
        jerv::binary
        nlohmann_json::nlohmann_json
)

add_library(jerv::protocol ALIAS jerv_protocol)

