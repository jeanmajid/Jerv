file(GLOB_RECURSE JERV_RAKNET_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(jerv_raknet
        ${JERV_RAKNET_SOURCES}
)

target_include_directories(jerv_raknet PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(jerv_raknet PUBLIC
        jerv::binary
        jerv::common
        asio
)

target_precompile_headers(jerv_raknet PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.hpp"
)

add_library(jerv::raknet ALIAS jerv_raknet)

