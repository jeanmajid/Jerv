file(GLOB_RECURSE JERV_CORE_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(jerv_core
        ${JERV_CORE_SOURCES}
)

target_include_directories(jerv_core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(jerv_core PUBLIC
        jerv::binary
        jerv::common
        jerv::raknet
        jerv::protocol
        asio
        zlibstatic
        nlohmann_json::nlohmann_json
)

if (WIN32)
    target_link_libraries(jerv_core PUBLIC ws2_32 mswsock)
endif ()

add_library(jerv::core ALIAS jerv_core)

