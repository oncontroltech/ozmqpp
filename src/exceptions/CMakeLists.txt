message(STATUS "Configuring Exceptions")

target_sources(${ZMQ_TARGET}
    PRIVATE
        BadAlloc.cpp
        Exception.cpp
        InitializationFailed.cpp
        InvalidContext.cpp
        MessageNotReceived.cpp
        MessageNotSent.cpp
        PollingError.cpp)
