#include "ozmqpp/exceptions/InitializationFailed.hh"

OZMQPP::InitializationFailed::InitializationFailed (const std::string_view class_name,
                                                    const std::string_view function_name,
                                                    const std::string_view error_msg) noexcept :
    Exception(class_name, function_name, error_msg)
{

}


OZMQPP::InitializationFailed::~InitializationFailed() = default;
