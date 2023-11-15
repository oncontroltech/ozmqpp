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

namespace ZMQ
{
// Forward declaration to use as friend
// Class
class Context;
// Function
class Connection;
Connection make_invalid_zmqconnection();

//! @brief Wrap of the connection resources from zeromq.
//!
class ZMQ_API Connection
{
public:
    Connection() = delete;

    //! @brief Copy constructor.
    //!
    //! @param other Object to copy information from.
    //!
    Connection(Connection& other);

    //! @brief Class destructor.
    //!
    //! Check if contains valid raw zeromq resources to de-allocate.
    //!
    virtual ~Connection();

    //! @brief Create connection for others connect to.
    //!
    //! This connection will act as server.
    //!
    //! @param address_string String containing a valid zeromq address.
    //!
    virtual void Bind(const std::string& address_string);

    //! @brief Connect to other existent connection.
    //!
    //! This connection will act as client.
    //!
    //! @param address_string String containing a valid zeromq address.
    //!
    virtual void Connect(const std::string& address_string);

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
    virtual void* GetRaw();

    //! @brief Copy operator overload.
    //!
    //! @param other Object to copy information from.
    //! @return Reference to self.
    //!
    Connection& operator=(const Connection& other);

    Connection& operator=(Connection&& other) noexcept ;

protected:
    //! @brief Default class constructor.
    //!
    //! @param raw_connection Connection to zeromq connection resource.
    //!
    explicit Connection (void* zmq_connection);

    //! @brief Friend class used to create default connection objects.
    //!
    //! Invalid connection object will be used where is not desired to declare
    //! Context.
    //!
    //! @return Invalid Connection.
    //!
    friend class Context;
    friend Connection make_invalid_zmqconnection();

private:

    //! @brief Pointer to zeromq raw connection object.
    //!
    //! This context pointer will be used by native zeromq resources.
    //!
    void* m_zmq_connection;
};
 
//! @brief Make Connection object which is invalid
//!
ZMQ_API Connection make_invalid_zmqconnection();
}

#endif // ZMQ_CONNECTION_HH
