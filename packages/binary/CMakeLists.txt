file(GLOB_RECURSE JERV_BINARY_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

add_library(jerv_binary INTERFACE)

target_include_directories(jerv_binary INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_sources(jerv_binary INTERFACE
    ${JERV_BINARY_HEADERS}
)

add_library(jerv::binary ALIAS jerv_binary)
