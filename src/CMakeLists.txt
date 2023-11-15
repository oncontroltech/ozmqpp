# Displays a message when configuring this folder
message(STATUS "Configuring ZeroMQ")

# Add the source files to the lib
target_sources(${ZMQ_TARGET}
                    PRIVATE
                        Connection.cpp Connection.hh
                        Context.cpp Context.hh
                        Frame.cpp Frame.hh
                        Message.cpp Message.hh
                        Poll.cpp Poll.hh
                        RouterConnection.cpp RouterConnection.hh
                        RouterMessage.cpp RouterMessage.hh)

add_subdirectory(exceptions)
