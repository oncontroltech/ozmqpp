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
#include <map>

namespace OZMQPP
{
    //! @brief Types of socket provided by ZMQ architecture
    //!
    enum SocketType : uint8_t
    {
        PUBLISHER = ZMQ_PUB,
        SUBSCRIBER = ZMQ_SUB,
        REQUEST = ZMQ_REQ,
        REPLY = ZMQ_REP,
        ROUTER = ZMQ_ROUTER,
        DEALER = ZMQ_DEALER,
        PUSH = ZMQ_PUSH,
        PULL = ZMQ_PULL,
        PAIR = ZMQ_PAIR
    };

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
    Context(Context& other);

    //! @brief Class destructor.
    //!
    ~Context();

    //! @brief Create zeromq connection wrapper.
    //!
    //! @param socket_type Type of socket to create.
    //! @return Valid Connection object to be used on communication.
    //!
    [[nodiscard]] Connection& CreateConnection(SocketType socket_type);

    //! @brief Create zeromq router connection wrapper.
    //!
    //! @param socket_type Type of socket to create.
    //! @return Valid RouterConnection object to be used on communication.
    //!
    [[nodiscard]] RouterConnection& CreateRouterConnection();

    //! @brief Erase connection object from ownership list
    //!
    //! @param connection_object Object to handle ownership.
    //!
    void EraseConnection(Connection& connection_ref);

    //! @brief Copy operator overload.
    //!
    //! @param other Object to copy information from.
    //! @return Reference to self.
    //!
    Context& operator=(Context& other);

protected:

private:

    //! @brief Pointer to zeromq raw context object.
    //!
    //! This context pointer will be used by native zeromq resources.
    //!
    void* m_zmq_context;

    //! @brief Map of connection owned by current context.
    //!
    //! The context creates zqm connections, which will be used by applications
    //! to perform communication actions. The ownership belong to the context,
    //! which will be responsible to erase them from memory.
    //!
    std::map<uint, Connection*> m_connection_map;

    //! @brief This mutex protects the creation of the socket.
    //!
    std::mutex m_connection_mutex;

    //! @brief
    //!
    uint m_connections_id_counter;

}; // class Context

} // namespace OZMQPP

#endif // ZMQ_CONTEXT_HH_
