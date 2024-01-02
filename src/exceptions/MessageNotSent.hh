#ifndef ZMQ_EXCEPTION_MESSAGE_NOT_SENT_HH
#define ZMQ_EXCEPTION_MESSAGE_NOT_SENT_HH

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

namespace OZMQPP
{

class ZMQ_API MessageNotSent : public Exception
{
public:
    MessageNotSent (const std::string& class_name,
                    const std::string& function_name,
                    const std::string& error_msg) noexcept;

    virtual ~MessageNotSent();
};

}

#endif // ZMQ_EXCEPTION_MESSAGE_NOT_SENT_HH
