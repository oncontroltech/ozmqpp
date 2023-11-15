#include "MessageNotReceived.hh"

ZMQ::MessageNotReceived::MessageNotReceived(const char* class_name,
                                            const char* function_name,
                                            const char* error_msg) noexcept :
    Exception(class_name, function_name, error_msg)
{

}
