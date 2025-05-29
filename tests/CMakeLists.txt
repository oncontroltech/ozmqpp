option(OZMQPP_TESTS "Build tests for ozmqpp" OFF)

if (OZMQPP_TESTS)
    enable_testing()

    include(CTest)

    # Add a test <name>_test.cpp
    function(add_test_incl_memcheck name)
        add_executable(${name}_test ${name}_test.cpp)
        target_include_directories(${name}_test
            PRIVATE
                ../include)
        target_link_libraries(${name}_test
            PRIVATE
                ozmqpp)

        add_dependencies(${name}_test ${ZMQ_TARGET})
        set_target_properties(${name}_test PROPERTIES EXCLUDE_FROM_ALL FALSE)

        add_test(NAME ${name}_test COMMAND $<TARGET_FILE:${name}_test>)
        if (WIN32)
            add_test(NAME ${name}_memchecked_test
                COMMAND "${DRMEM_EXECUTABLE}"
                    -exit_code_if_errors 1
                    -quiet
                    -batch
                    -results_to_stdout
                    -check_leaks
                    -check_uninitialized
                    -check_stack
                    -check_heap
                    -check_frees
                    -- $<TARGET_FILE:${name}_test>
                WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
            )
        elseif (UNIX)
            add_test(NAME ${name}_memchecked_test
                COMMAND valgrind
                    --error-exitcode=1
                    --tool=memcheck
                    --track-origins=yes
                    --leak-check=full
                    --errors-for-leak-kinds=all
                    --show-leak-kinds=all
                    --num-callers=500
                    --undef-value-errors=yes
                    $<TARGET_FILE:${name}_test>
                WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
            )
        endif ()
    endfunction()

    add_test_incl_memcheck(conn_explicit_destroy)
    add_test_incl_memcheck(conn_implicit_destroy)
    add_test_incl_memcheck(conn_tcp)
    add_test_incl_memcheck(multi_frame_transfer)
endif ()
