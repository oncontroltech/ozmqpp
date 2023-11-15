#ifndef ZMQ_CONTEXT_HH_
#define ZMQ_CONTEXT_HH_

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

// ZMQ headers
#include "Connection.hh"
#include "RouterConnection.hh"

// STD headers
#include <memory>
#include <mutex>
#include <zmq.h>

namespace ZMQ
{
//! @brief Wrapper for zeromq context object.
//!
//! Context is the base object where connection objects are created, using
//! zeromq resources.
//!
class ZMQ_API Context
{
public:
    //! @brief Default context constructor.
    //!
    Context();

    //! @brief Copy constructor.
    //!
    //! @param other object to copy information from.
    //!
    Context(const Context& other);

    //! @brief Class destructor.
    //!
    ~Context() = default;

    //! @brief Create zeromq connection wrapper.
    //!
    //! @param socket_type Type of socket to create.
    //! @return Valid Connection object to be used on communication.
    //!
    Connection CreateConnection(int socket_type);

    //! @brief Create zeromq router connection wrapper.
    //!
    //! @param socket_type Type of socket to create.
    //! @return Valid RouterConnection object to be used on communication.
    //!
    RouterConnection CreateRouterConnection();

    //! @brief Copy operator overload.
    //!
    //! @param other Object to copy information from.
    //! @return Reference to self.
    //!
    Context& operator=(const Context& other);

private:

    //! @brief Pointer to zeromq raw context object.
    //!
    //! This context pointer will be used by native zeromq resources.
    //!
    void* m_zmq_context;

    //! @brief This mutex protects the creation of the socket.
    //!
    std::mutex m_connection_mutex;
};
}

#endif // ZMQ_CONTEXT_HH_
