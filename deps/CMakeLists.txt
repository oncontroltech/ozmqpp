include(FetchContent)

set(ZMQ_BUILD_TESTS OFF CACHE INTERNAL "Turn off libzmq tests")
set(BUILD_SHARED OFF CACHE INTERNAL "Link libzmq dynamically")
set(BUILD_STATIC ON CACHE INTERNAL "Link libzmq-static library")

FetchContent_Declare(
	libzmq
	URL "https://github.com/zeromq/libzmq/releases/download/v4.3.5/zeromq-4.3.5.zip"
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

add_dependencies(${ZMQ_TARGET} libzmq-static)

FetchContent_MakeAvailable(libzmq)

target_link_libraries(${ZMQ_TARGET}
    PRIVATE
        libzmq-static
)

get_target_property(ZMQ_INCLUDE_DIRS libzmq-static INTERFACE_INCLUDE_DIRECTORIES)
if(ZMQ_INCLUDE_DIRS)
	target_include_directories(${ZMQ_TARGET}
		PUBLIC
			${ZMQ_INCLUDE_DIRS}
	)
endif()
