add_library(jerv_binary INTERFACE)
add_library(jerv::binary ALIAS jerv_binary)

target_include_directories(jerv_binary
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_sources(jerv_binary INTERFACE
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES
        include/jerv/binary/cursor.hpp
        include/jerv/binary/nbt.hpp
)
