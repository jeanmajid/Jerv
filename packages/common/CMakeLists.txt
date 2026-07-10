add_library(jerv_common INTERFACE)

target_include_directories(jerv_common
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(jerv_common INTERFACE
        spdlog::spdlog
)

target_sources(jerv_common INTERFACE
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/include/jerv/common/logger.hpp
)

add_library(jerv::common ALIAS jerv_common)
