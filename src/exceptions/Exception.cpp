#include "Exception.hh"

ZMQ::Exception::Exception(const char* class_name,
                          const char* function_name,
                          const char* error_msg) noexcept :
    m_class_name(class_name),
    m_function_name(function_name),
    m_error_message(error_msg)
{

}

ZMQ::Exception::Exception(const Exception& other) :
    m_class_name(other.m_class_name),
    m_function_name(other.m_function_name),
    m_error_message(other.m_error_message)
{

}

const char*
ZMQ::Exception::what() const noexcept
{
    // Calculate the size to be allocated.
    int length = std::snprintf(nullptr,
                               0,
                               "[%s](%s) %s",
                               m_class_name,
                               m_function_name,
                               m_error_message);
    // Allocate the necessary space plus null terminator.
    char* c_str = new char[length + 1];
    // Constructs the string
    std::snprintf(c_str,
                  length + 1,
                  "[%s](%s) %s",
                  m_class_name,
                  m_function_name,
                  m_error_message);
    return c_str;
}

const char*
ZMQ::Exception::GetClassName() const
{
    return m_class_name;
}

const char*
ZMQ::Exception::GetFunctionName() const
{
    return m_function_name;
}

const char*
ZMQ::Exception::GetErrorMessage() const
{
    return m_error_message;
}
