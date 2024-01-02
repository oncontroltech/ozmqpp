#ifndef ZMQ_EXCEPTION_POLLING_ERROR_HH
#define ZMQ_EXCEPTION_POLLING_ERROR_HH

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

class ZMQ_API PollingError : public Exception
{
public:
    PollingError (const std::string& class_name,
                  const std::string& function_name,
                  const std::string& error_msg) noexcept;
    
    virtual ~PollingError();
};

}

#endif // ZMQ_EXCEPTION_POLLING_ERROR_HH
