add_executable(jerver_app
    src/main.cpp
)

target_link_libraries(jerver_app PRIVATE
    jerv::core
)

set_target_properties(jerver_app PROPERTIES
    OUTPUT_NAME "jerver"
)
