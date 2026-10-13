add_library(jerv_raknet
        src/jerv/raknet/raknetServer.cpp
)
add_library(jerv::raknet ALIAS jerv_raknet)

target_include_directories(jerv_raknet PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(jerv_raknet PUBLIC
        jerv::binary
        jerv::common
        asio
)
