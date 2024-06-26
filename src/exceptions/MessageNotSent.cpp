#include "ozmqpp/exceptions/MessageNotSent.hh"

OZMQPP::MessageNotSent::MessageNotSent (const std::string& class_name,
                                        const std::string& function_name,
                                        const std::string& error_msg) noexcept :
    Exception(class_name, function_name, error_msg)
{

}

OZMQPP::MessageNotSent::~MessageNotSent()
{

}

