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

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

if (BUILDS_SHARED_LIBS EQUAL ON)
    message(STATUS "Build: Shared")
else()
    message(STATUS "Build: Static")
endif()

add_library(${ZMQ_TARGET})

message(STATUS "Configuration type: ${CMAKE_BUILD_TYPE}")

if (MSVC)
    target_compile_options(${ZMQ_TARGET}
        PRIVATE
            /GS # Checks buffer security
            /sdl # Enables Strict mode for /GS and additional checks
            /arch:SSE2 # Target SSE2 CPU arch (year ~2001)
            #/WX # Treat every warning as error
            /W4 # Enable extra warnings
            /permissive- # Disable permissive flags
            /EHsc
            /guard:cf # Control Flow Guard
            #/guard:ehcont # Extended CFG for exception handlers
            /Zc:preprocessor # Use standard conforming preprocessor
            /Zc:inline # Remove unreferenced inline functions
            /Zc:externConstexpr # Enforce correct linkage for constexpr
            /MP # Enable multithread compilation
            /FS # Allow multiple processes to write to .pdb files
    )

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${ZMQ_TARGET}
            PRIVATE
                /RTC1 # Enable runtime checks
                /Zi # Enable debug info
                /Od # Disable optimizations
                /analyze # Enable static analyzer
        )

        target_link_options(${ZMQ_TARGET}
            PRIVATE
                /DEBUG # Generate debug information
                /INCREMENTAL # Faster incremental linking
        )
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        # Check and enable LTO support if possible
        include(CheckIPOSupported)
        check_ipo_supported(RESULT supported OUTPUT error)
        if (supported)
            message(STATUS "IPO / LTO support: enabled")
            set_property(TARGET ${ZMQ_TARGET} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
        else()
            message(STATUS "IPO / LTO support: disabled (<${error}>)")
        endif()

        target_compile_options(${ZMQ_TARGET}
            PRIVATE
                /O2 # Optimize for speed
                /Oi # Inline intrinsics
                /Ot # Favor fast code
                /GL # Whole program optimization (LTO)
                /Gy # Function level linking
                /Gw # Data level linking
                /GF # Enable string pooling
                /Qspectre # Mitigate certain Spectre variant 1 security vulnerabilities
                /Qspectre-load # Extends the /Qspectre flag, mitigating against any possible speculative execution side-channel attacks based on loads
                /Qspectre-jmp # Extends the /Qspectre flag and mitigates speculative execution side-channel attacks on unconditional direct branches
        )

        target_link_options(${ZMQ_TARGET}
            PRIVATE
                /LTCG # Link time code generation
                /OPT:REF # Remove unreference functions/data
                /OPT:ICF # Fold identical functions
                /INCREMENTAL:NO # No incremental linking in release
                /NXCOMPAT # Marks binary as Data Execution Prevention compatible
                /DYNAMICBASE # Generate PIE code for ASLR
                /HIGHENTROPYVA # 64-bit ASLR
        )
    endif()

    target_compile_definitions(${ZMQ_TARGET}
        PUBLIC
            _CRT_SECURE_NO_WARNINGS=1
    )

    target_link_options(${ZMQ_TARGET}
        PRIVATE
            /guard:cf # Control Flow Guard
    )
else ()
    # Add extra flags to the compiler
    target_compile_options(${ZMQ_TARGET}
        PRIVATE
            -Werror
            -Wall
            -Wextra
            -pedantic
            # Comparing floats can be unprecise
            -Wfloat-equal
            # Enable table-based thread cancellation
            -fexceptions
            # No text relocations for shared libraries
            -fpic
            # Detect and reject underlinking
            -Wl,-z,defs
            # Disable lazy binding
            -Wl,-z,now
            # Read-only segments after relocation
            -Wl,-z,relro
    )

    # x86 cpu arch specific flags
    if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64" OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64")
        target_compile_options(${ZMQ_TARGET}
            PRIVATE
                # Control flow integrity protection
                -fcf-protection
                -march=x86-64
        )
    else()
        target_compile_options(${ZMQ_TARGET}
            PRIVATE
                -march=native
        )
    endif()

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${ZMQ_TARGET}
            PRIVATE
                # GCC debug information for GDB
                -ggdb
                # Run-time bounds checking for C++ strings and containers
                -D_GLIBCXX_ASSERTIONS
                # Increased reliability of stack overflow detection
                -fstack-clash-protection
                # Stack smashing protector
                -fstack-protector-strong
                # Increased reliability of backtraces
                -fasynchronous-unwind-tables
                # Store compiler flags in debugging information
                -grecord-gcc-switches
        )

    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${ZMQ_TARGET}
            PRIVATE
                # Run-time buffer overflow detection
                -D_FORTIFY_SOURCE=2
                # Eliminate function call overhead
                -finline-functions
                # Reduce loop iterations
                -funroll-loops
                # Optimizations
                -O3)
    endif()
endif()

target_include_directories(${ZMQ_TARGET}
    PUBLIC
        $<INSTALL_INTERFACE:include>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
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
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
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
