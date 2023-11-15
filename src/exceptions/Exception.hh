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

namespace ZMQ
{
class ZMQ_API Exception : public std::exception
{
public:
    Exception(const char* class_name,
              const char* function_name,
              const char* error_msg) noexcept;
    
    Exception(const Exception& other);

    ~Exception() override = default;

    [[nodiscard]] const char* what() const noexcept override;

    [[nodiscard]] const char* GetClassName() const;

    [[nodiscard]] const char* GetFunctionName() const;

    [[nodiscard]] const char* GetErrorMessage() const;

private:
    const char* m_class_name;
    const char* m_function_name;
    const char* m_error_message;
};
}

#endif // ZMQ_EXCEPTION_HH
