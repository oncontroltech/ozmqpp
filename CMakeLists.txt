cmake_minimum_required(VERSION 3.18)

enable_testing()

set (CMAKE_CXX_STANDARD 20
    CACHE STRING "C++ Standard version")

set (CMAKE_VERBOSE_MAKEFILE ON
    CACHE BOOL "Show makefiles commands on build")

project(ZeroMQ)

set (ZMQ_TARGET "ozmqpp"
        CACHE STRING "ZeroMQ target name")

add_library(${ZMQ_TARGET} SHARED)

target_compile_definitions(${ZMQ_TARGET}
    PRIVATE
        ZMQ_BUILD)

target_compile_options(${ZMQ_TARGET}
    PRIVATE
        -Werror
        -Wall
        -Wextra
        -Wpedantic)

add_subdirectory(src)

add_subdirectory(tests)
