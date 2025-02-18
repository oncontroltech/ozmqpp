#ifndef ZMQ_ROUTERCONNECTION_HH
#define ZMQ_ROUTERCONNECTION_HH

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
#include "RouterMessage.hh"

namespace OZMQPP
{

//! @brief Wrap of the connection resources from zeromq with router format.
//!
//! Router pattern contain differences dealing with message formats, which
//! this class derivation takes care when dealing with this communication
//! format.
//!
class ZMQ_API RouterConnection : public Connection
{
public:

    RouterConnection() = delete;

    //! @brief Copy constructor.
    //!
    //! Initialize base class resources with input.
    //!
    //! @param other object to copy information from.
    //!
    RouterConnection(RouterConnection& other) = delete;

    //! @brief class destructor.
    //!
    ~RouterConnection();

    //! @brief Retrieve message from connection with router format.
    //!
    //! The messaging format takes in consideration the first message to be a
    //! connection ID. This field must be recorded separately from the message
    //! content.
    //!
    //! @return ZQM message in router format.
    //!
    RouterMessage ReceiveRouterMessage();

protected:
    //! @brief Default class constructor.
    //!
    //! @param raw_connection Connection to zeromq connection resource.
    //!
    RouterConnection(unsigned int new_connection_id, void* zmq_connection);

    //! @brief Friend class used to create default connection objects.
    //!
    friend class Context;

}; // class RouterConnection

} // namespace OZMQPP

#endif // ZMQ_ROUTERCONNECTION_HH
