cmake_minimum_required(VERSION 4.0)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
project(jerv VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build time optimisations
option(CMAKE_UNITY_BUILD "Enable unity build" ON)
set(CMAKE_UNITY_BUILD_BATCH_SIZE 8 CACHE STRING "Files per unity batch")

find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    message(STATUS "using ccache")
else ()
    message(STATUS "ccache not found")
endif ()

unset(MOLD_LINKER CACHE)
find_program(MOLD_LINKER NAMES mold ld.mold)
if (MOLD_LINKER)
    set(JERV_MOLD_LINK_FLAG "-fuse-ld=mold")
    message(STATUS "using mold linker at ${MOLD_LINKER}")
else ()
    set(JERV_MOLD_LINK_FLAG "")
    message(STATUS "mold not found, using default linker")
endif ()

include(cmake/CPM.cmake)

if (WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0A00)
endif ()

# ASIO
CPMAddPackage(
        URI "gh:chriskohlhoff/asio#asio-1-36-0"
        DOWNLOAD_ONLY YES
)

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE
        ${asio_SOURCE_DIR}/asio/include
)
target_compile_definitions(asio INTERFACE ASIO_STANDALONE)

# SPDLOG
CPMAddPackage("gh:gabime/spdlog#v1.16.0")
set_target_properties(spdlog PROPERTIES UNITY_BUILD OFF)

# JSON
CPMAddPackage("gh:nlohmann/json#v3.12.0")

# ZLIB
set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
CPMAddPackage("gh:madler/zlib#v1.3.1")
set_target_properties(zlib PROPERTIES UNITY_BUILD OFF)
set_target_properties(zlibstatic PROPERTIES UNITY_BUILD OFF)

set(ZLIB_INCLUDE_DIR
        ${zlib_SOURCE_DIR}
        ${zlib_BINARY_DIR}
)
set(ZLIB_LIBRARY zlibstatic)

# SNAPPY
# CPMAddPackage("gh:google/snappy#1.2.2")

# LEVELDB
set(LEVELDB_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LEVELDB_BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
set(LEVELDB_INSTALL OFF CACHE BOOL "" FORCE)

CPMAddPackage("gh:jeanmajid/leveldb#mojang-compatible")

add_subdirectory(packages/binary)
add_subdirectory(packages/common)
add_subdirectory(packages/raknet)
add_subdirectory(packages/protocol)
add_subdirectory(packages/core)
add_subdirectory(app)
