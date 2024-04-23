target_sources(${ZMQ_TARGET}
    PUBLIC FILE_SET HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/include FILES
            BadAlloc.hh
            Exception.hh
            InitializationFailed.hh
            InvalidContext.hh
            MessageNotReceived.hh
            MessageNotSent.hh
            PollingError.hh)