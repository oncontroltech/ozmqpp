#ifndef ZMQ_EXCEPTION_MESSAGE_NOT_RECEIVED_HH
#define ZMQ_EXCEPTION_MESSAGE_NOT_RECEIVED_HH

#ifdef _WIN32
    #ifdef ZMQ_BUILD
        #define ZMQ_API __declspec(dllexport)
    #else
        #define ZMQ_API __declspec(dllimport)
    #endif
#elif __unix__
    #ifndef ZMQ_API
        #define ZMQ_API
    #endif
#endif

// ZMQ headers
#include "Exception.hh"

namespace ZMQ
{
class ZMQ_API MessageNotReceived : public Exception
{
public:
    MessageNotReceived(const char* class_name,
                       const char* function_name,
                       const char* error_msg) noexcept;
    
    ~MessageNotReceived() override = default;
};
}

#endif // ZMQ_EXCEPTION_MESSAGE_NOT_RECEIVED_HH
