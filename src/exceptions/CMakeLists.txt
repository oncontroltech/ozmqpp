message(STATUS "Configuring Exceptions")

target_sources(${ZMQ_TARGET}
    PRIVATE
        BadAlloc.cpp BadAlloc.hh
        Exception.cpp Exception.hh
        InitializationFailed.cpp InitializationFailed.hh
        InvalidContext.cpp InvalidContext.hh
        MessageNotReceived.cpp MessageNotReceived.hh
        MessageNotSent.cpp MessageNotSent.hh
        PollingError.cpp PollingError.hh)
