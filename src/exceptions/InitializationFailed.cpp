#include "exceptions/InitializationFailed.hh"

OZMQPP::InitializationFailed::InitializationFailed (const std::string& class_name,
                                                    const std::string& function_name,
                                                    const std::string& error_msg) noexcept :
    Exception(class_name, function_name, error_msg)
{

}


OZMQPP::InitializationFailed::~InitializationFailed()
{

}

