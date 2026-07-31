#ifndef ZMQ_CONNECTION_HH
#define ZMQ_CONNECTION_HH

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
#include "Message.hh"

// STD headers
#include <memory>

namespace OZMQPP
{

// Forward declaration to use as friend
// Class
class Context;

//! @brief Wrap of the connection resources from zeromq.
//!
class ZMQ_API Connection
{
public:
    enum class ConnectionStatus
    {
        NOT_CONNECTED,
        CONNECTED,
        BOUND
    };

    Connection() = delete;

    //! @brief Copy constructor.
    //!
    Connection(const Connection& other) = delete;

    //! @brief Move constructor.
    //!
    Connection(Connection&& other) noexcept;

    //! @brief Class destructor.
    //!
    //! Check if contains valid raw zeromq resources to de-allocate.
    //!
    virtual ~Connection();

    [[nodiscard]] ConnectionStatus GetConnectionStatus() const;

    //! @brief Create connection for others connect to.
    //!
    //! This connection will act as server.
    //!
    //! @param address_string String containing a valid zeromq address.
    //!
    virtual void Bind(const std::string_view address_string);

    virtual void Unbind();

    //! @brief Connect to other existent connection.
    //!
    //! This connection will act as client.
    //!
    //! @param address_string String containing a valid zeromq address.
    //!
    virtual void Connect(const std::string_view address_string);

    virtual void Disconnect();

    //! @brief Check if connection is still valid.
    //!
    //! Current version of function check if context is still valid.
    //!
    //! @return True if context is valid.
    //!
    [[nodiscard]] virtual bool IsValid() const;

    //! @brief Send zeromq message.
    //!
    //! @param message Message to send by zeromq.
    //!
    virtual void SendMessage(const Message& message);

    //! @brief Receive zeromq message.
    //!
    //! @return Message received by zeromq.
    //!
    virtual Message ReceiveMessage();

    //! @brief Get zeromq raw pointer.
    //!
    //! @return Raw pointer to zeromq connection resource.
    //!
    [[nodiscard]] virtual void* GetRaw();

    //! @brief Get unique identifier of this connection in the context
    //!
    //! @return ID of the connection inside the context it was created
    //!
    [[nodiscard]] unsigned int GetUniqueID() const;

    //! @brief Method called on Context destruction
    //!
    //!
    //!
    virtual void ContextCloseCall();

    //! @brief Copy operator overload.
    //!
    Connection& operator=(const Connection& other) = delete;

    //! @brief Move operator overload.
    //!
    Connection& operator=(Connection&& other) noexcept;

protected:

    //! @brief Friend class used to create default connection objects.
    //!
    //! Invalid connection object will be used where is not desired to declare
    //! Context.
    //!
    //! @return Invalid Connection.
    //!
    friend class Context;

    //! @brief Default class constructor.
    //!
    //! @param connection_unique_id Connection ID in the connection map.
    //! @param raw_zmq_connection Connection to zeromq connection resource.
    //!
    Connection(const unsigned int connection_unique_id, void* raw_zmq_connection);

private:
    //! @brief connection id
    //!
    //! The connection id should be unique inside the context, due to be used
    //! as indexation number for operations within context.
    //!
    unsigned int m_connection_unique_id;

    //! @brief Pointer to zeromq raw connection object.
    //!
    //! This context pointer will be used by native zeromq resources.
    //!
    void* m_zmq_connection;

    std::string m_endpoint;

    ConnectionStatus m_connection_status;

}; // class Connection


} // namespace OZMQPP

#endif // ZMQ_CONNECTION_HH
