#include "InvalidContext.hh"

ZMQ::InvalidContext::InvalidContext(const char* class_name,
                                    const char* function_name,
                                    const char* error_msg) noexcept :
    Exception(class_name, function_name, error_msg)
{

}
