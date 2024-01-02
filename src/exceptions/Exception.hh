#ifndef ZMQ_EXCEPTION_HH
#define ZMQ_EXCEPTION_HH

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

// STD headers
#include <exception>
#include <string>

namespace OZMQPP
{

class ZMQ_API Exception
{
public:
    Exception(const std::string& class_name,
              const std::string& function_name,
              const std::string& error_msg) noexcept;
    
    Exception(const Exception& other);

    virtual ~Exception();

    [[nodiscard]] std::string GetClassName() const;

    [[nodiscard]] std::string GetFunctionName() const;

    [[nodiscard]] std::string GetErrorMessage() const;

protected:

private:
    const std::string m_class_name;

    const std::string m_function_name;

    const std::string m_error_message;
};

}

#endif // ZMQ_EXCEPTION_HH
