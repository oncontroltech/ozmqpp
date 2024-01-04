
#include "Exception.hh"

OZMQPP::Exception::Exception(const std::string& class_name,
                          const std::string& function_name,
                          const std::string& error_msg) noexcept :
    m_class_name(class_name),
    m_function_name(function_name),
    m_error_message(error_msg)
{

}

OZMQPP::Exception::Exception(const Exception& other) :
    m_class_name(other.m_class_name),
    m_function_name(other.m_function_name),
    m_error_message(other.m_error_message)
{
}

OZMQPP::Exception::~Exception()
{
    
}

std::string
OZMQPP::Exception::GetClassName() const
{
    return m_class_name;
}

std::string
OZMQPP::Exception::GetFunctionName() const
{
    return m_function_name;
}

std::string
OZMQPP::Exception::GetErrorMessage() const
{
    return m_error_message;
}
