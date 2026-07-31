#include "ozmqpp/exceptions/MessageNotReceived.hh"


OZMQPP::MessageNotReceived::MessageNotReceived(const std::string_view class_name,
                                               const std::string_view function_name,
                                               const std::string_view error_msg) noexcept :
    Exception(class_name, function_name, error_msg)
{

}

OZMQPP::MessageNotReceived::~MessageNotReceived() = default;
