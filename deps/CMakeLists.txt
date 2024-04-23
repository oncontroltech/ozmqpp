include(FetchContent)

set(ZMQ_BUILD_TESTS OFF CACHE INTERNAL "Turn off libzmq tests")

FetchContent_Declare(
  libzmq
  GIT_REPOSITORY https://github.com/zeromq/libzmq.git
  GIT_TAG v4.3.5
)

FetchContent_MakeAvailable(libzmq)
