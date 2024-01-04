enable_testing()

include(CTest)

# Add a test <name>_test.cpp
function(add_test_incl_memcheck name)
    add_executable(${name}_test ${name}_test.cpp)
    target_include_directories(${name}_test
        PRIVATE
            ../src)
    target_link_libraries(${name}_test
        PRIVATE
            $<TARGET_FILE:${ZMQ_TARGET}>
            libzmq.so)

    add_dependencies(${name}_test ${ZMQ_TARGET})

    add_test(NAME ${name}_test COMMAND $<TARGET_FILE:${name}_test>)
    add_test(NAME ${name}_memchecked_test
        COMMAND valgrind
        --error-exitcode=1
        --tool=memcheck
        --track-origins=yes
        --leak-check=full
        --errors-for-leak-kinds=definite
        --show-leak-kinds=definite $<TARGET_FILE:${name}_test>
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
endfunction()

add_test_incl_memcheck(conn_explicit_destroy)
add_test_incl_memcheck(conn_implicit_destroy)
add_test_incl_memcheck(conn_tcp)
add_test_incl_memcheck(multi_frame_transfer)
