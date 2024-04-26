cmake_minimum_required(VERSION 3.23)

enable_testing()

set(CMAKE_CXX_STANDARD 20
    CACHE STRING "C++ Standard version")

set(CMAKE_VERBOSE_MAKEFILE ON
    CACHE BOOL "Show makefiles commands on build"
)

set(ZMQ_TARGET "ozmqpp"
    CACHE STRING "ZeroMQ target name"
)

project(${ZMQ_TARGET})

if (STATIC_LIBRARY EQUAL ON)
    message(STATUS "Build: Static")
    add_library(${ZMQ_TARGET} STATIC)

else()
    message(STATUS "Build: Shared")
    add_library(${ZMQ_TARGET} SHARED)

endif()

message(STATUS "Configuration type: ${CMAKE_BUILD_TYPE}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions("DEBUG")
    target_compile_options(${ZMQ_TARGET}
        PRIVATE
            # Enable memory errors detection
            -fsanitize=address
            # Increased reliability of backtraces
            -fasynchronous-unwind-tables
            # Store compiler flags in debugging information
            -grecord-gcc-switches
    )

    target_link_options(${ZMQ_TARGET}
        PRIVATE
            # Enable memory errors detection
            -fsanitize=address
    )

elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${ZMQ_TARGET}
        PRIVATE
            # Optimizations
            -O3
            # Run-time buffer overflow detection
            -D_FORTIFY_SOURCE=2
    )
    
    # Check and enable LTO support if possible
    include(CheckIPOSupported)
    check_ipo_supported(RESULT supported OUTPUT error)
    if (supported)
        message(STATUS "IPO / LTO support: enabled")
        set_property(TARGET ${ZMQ_TARGET} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(STATUS "IPO / LTO support: disabled (<${error}>)")
    endif()
endif()

# Add extra flags to the compiler
target_compile_options(${ZMQ_TARGET}
    PRIVATE
        -Werror
        -Wall
        -Wextra
        -Wpedantic
        # Run-time bounds checking for C++ strings and containers
        -D_GLIBCXX_ASSERTIONS
        # Enable table-based thread cancellation
        -fexceptions
        # No text relocations for shared libraries
        -fpic
        # Increased reliability of stack overflow detection
        -fstack-clash-protection
        # Stack smashing protector
        -fstack-protector-strong
        # Detect and reject underlinking
        -Wl,-z,defs
        # Disable lazy binding
        -Wl,-z,now
        # Read-only segments after relocation
        -Wl,-z,relro
)

if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "64Bit")
    target_compile_options(${ZMQ_TARGET}
        PRIVATE
            # Control flow integrity protection
            -fcf-protection
    )
endif ()

target_include_directories(${ZMQ_TARGET}
    PUBLIC
        $<INSTALL_INTERFACE:include>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

target_link_libraries(${ZMQ_TARGET}
    PUBLIC
        zmq
)

target_compile_definitions(${ZMQ_TARGET}
    PRIVATE
        ZMQ_BUILD
)

add_subdirectory(deps)

add_subdirectory(include/ozmqpp)
add_subdirectory(src)

add_subdirectory(tests)

###
# INSTALL
###

include(GNUInstallDirs)

install(TARGETS ${ZMQ_TARGET}
    EXPORT "${ZMQ_TARGET}Config"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT ${ZMQ_TARGET}
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT "${ZMQ_TARGET}Config"
    FILE "${ZMQ_TARGET}Config.cmake"
    NAMESPACE ${ZMQ_TARGET}::
    DESTINATION ${CMAKE_INSTALL_DATADIR}/cmake
)

# Add uninstall target
if (NOT TARGET uninstall)
  configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY
    )

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif ()
