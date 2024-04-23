# Displays a message when configuring this folder
message(STATUS "Configuring ZeroMQ")

# Add the source files to the lib
target_sources(${ZMQ_TARGET}
    PRIVATE
        Connection.cpp
        Context.cpp
        Frame.cpp
        Message.cpp
        Poll.cpp
        RouterConnection.cpp
        RouterMessage.cpp)

add_subdirectory(exceptions)
