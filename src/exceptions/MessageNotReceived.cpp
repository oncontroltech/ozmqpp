#include "exceptions/MessageNotReceived.hh"


OZMQPP::MessageNotReceived::MessageNotReceived(const std::string& class_name,
                                               const std::string& function_name,
                                               const std::string& error_msg) noexcept :
    Exception(class_name, function_name, error_msg)
{

}

OZMQPP::MessageNotReceived::~MessageNotReceived()
{
    
}
