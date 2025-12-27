file(GLOB_RECURSE JERV_COMMON_HEADERS CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

add_library(jerv_common INTERFACE)

target_include_directories(jerv_common INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(jerv_common INTERFACE
        spdlog::spdlog
)

target_sources(jerv_common INTERFACE
        ${JERV_COMMON_HEADERS}
)

add_library(jerv::common ALIAS jerv_common)
