#ifndef ZMQ_EXCEPTION_BAD_ALLOC_HH
#define ZMQ_EXCEPTION_BAD_ALLOC_HH

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
class ZMQ_API BadAlloc : public Exception
{
public:
    BadAlloc(const char* class_name,
             const char* function_name,
             const char* error_msg) noexcept;
    
    ~BadAlloc() override = default;
};
}

#endif // ZMQ_EXCEPTION_BAD_ALLOC_HH
