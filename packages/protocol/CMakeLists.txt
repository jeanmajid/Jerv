add_library(jerv_protocol INTERFACE)

target_include_directories(jerv_protocol
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(jerv_protocol INTERFACE
        spdlog::spdlog
)

target_sources(jerv_protocol INTERFACE
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES

)

add_library(jerv::protocol ALIAS jerv_protocol)
