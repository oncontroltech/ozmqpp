include(FetchContent)

set(ZMQ_BUILD_TESTS OFF CACHE INTERNAL "Turn off libzmq tests")

FetchContent_Declare(
	libzmq
	URL "https://github.com/zeromq/libzmq/releases/download/v4.3.5/zeromq-4.3.5.zip"
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

add_dependencies(${ZMQ_TARGET} libzmq)

FetchContent_MakeAvailable(libzmq)
