#ifndef ZMQ_EXCEPTION_INVALID_CONTEXT_HH
#define ZMQ_EXCEPTION_INVALID_CONTEXT_HH

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

class ZMQ_API InvalidContext : public Exception
{
public:
    InvalidContext(const std::string& class_name,
                   const std::string& function_name,
                   const std::string& error_msg) noexcept;
    
    virtual ~InvalidContext();

protected:

private:

}; // class InvalidContext

} // namespace OZMQPP

#endif // ZMQ_EXCEPTION_INVALID_CONTEXT_HH
