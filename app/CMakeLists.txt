file(GLOB_RECURSE JERVER_APP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(jerver_app
    ${JERVER_APP_SOURCES}
)

target_link_libraries(jerver_app PRIVATE
    jerv::core
)

set_target_properties(jerver_app PROPERTIES
    OUTPUT_NAME "jerver"
)
