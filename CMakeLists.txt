cmake_minimum_required(VERSION 4.0)
project(jerv VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build time optimisations
option(CMAKE_UNITY_BUILD "Enable unity (jumbo) build" ON)
set(CMAKE_UNITY_BUILD_BATCH_SIZE 8 CACHE STRING "Files per unity batch")

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    message(STATUS "using ccache")
else()
    message(STATUS "ccache not found")
endif()

unset(MOLD_LINKER CACHE)
find_program(MOLD_LINKER NAMES mold ld.mold)
if(MOLD_LINKER)
    set(JERV_MOLD_LINK_FLAG "-fuse-ld=mold")
    message(STATUS "using mold linker at ${MOLD_LINKER}")
else()
    set(JERV_MOLD_LINK_FLAG "")
    message(STATUS "mold not found, using default linker")
endif()

include(FetchContent)

FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-36-0
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(asio)
add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
target_compile_definitions(asio INTERFACE ASIO_STANDALONE)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.16.0
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(spdlog)
set_target_properties(spdlog PROPERTIES UNITY_BUILD OFF)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
    zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_SHALLOW ON
    GIT_TAG v1.3.1
)
set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(zlib)

set_target_properties(zlib PROPERTIES UNITY_BUILD OFF)
set_target_properties(zlibstatic PROPERTIES UNITY_BUILD OFF)

set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
set(ZLIB_LIBRARY zlibstatic)

add_subdirectory(packages/binary)
add_subdirectory(packages/common)
add_subdirectory(packages/raknet)
add_subdirectory(packages/protocol)
add_subdirectory(packages/core)
add_subdirectory(app)
