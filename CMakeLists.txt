cmake_minimum_required(VERSION 3.18)

enable_testing()

set (CMAKE_CXX_STANDARD 20
    CACHE STRING "C++ Standard version")

set (CMAKE_VERBOSE_MAKEFILE ON
    CACHE BOOL "Show makefiles commands on build")

project(ZeroMQ)

set (ZMQ_TARGET "ozmqpp"
        CACHE STRING "ZeroMQ target name")

if (STATIC_LIBRARY EQUAL ON)
    message(STATUS "Build: Static")
    add_library(${ZMQ_TARGET} STATIC)
else()
    message(STATUS "Build: Shared")
    add_library(${ZMQ_TARGET} SHARED)
    target_compile_options(${ZMQ_TARGET}
        PRIVATE
            -fPIC)
endif()

target_compile_definitions(${ZMQ_TARGET}
    PRIVATE
        ZMQ_BUILD)

target_compile_options(${ZMQ_TARGET}
    PRIVATE
        -Werror
        -Wall
        -Wextra
        -Wpedantic)

target_link_libraries(${ZMQ_TARGET}
    PUBLIC
        zmq)

add_subdirectory(src)

add_subdirectory(tests)

install(
    TARGETS ${ZMQ_TARGET}
    EXPORT "${ZMQ_TARGET}Config"
    DESTINATION lib
    INCLUDES DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

install(EXPORT "${ZMQ_TARGET}Config"
        FILE "${ZMQ_TARGET}Config.cmake"
        NAMESPACE ${ZMQ_TARGET}::
        DESTINATION cmake)

install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/"
    DESTINATION include/${ZMQ_TARGET}
    FILES_MATCHING
    PATTERN "*.hh")

# uninstall target
if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
