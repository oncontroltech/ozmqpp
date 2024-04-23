# Add the header files to the lib
target_sources(${ZMQ_TARGET}
    PUBLIC FILE_SET HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/include FILES
            Connection.hh
            Context.hh
            Frame.hh
            Message.hh
            Poll.hh
            RouterConnection.hh
            RouterMessage.hh)

add_subdirectory(exceptions)
