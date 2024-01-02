#include "Context.hh"

// ZMQ headers
#include <zmq.h>

// EXCEPTION headers
#include "exceptions/InvalidContext.hh"

// File const values
static const char CLASS_NAME[] = "Context";

OZMQPP::Context::Context() :
    m_connection_mutex()
{
    // Create raw zmq context
    m_zmq_context = zmq_ctx_new();

    // Check if the context is valid
    if (m_zmq_context == nullptr)
    {
        throw InvalidContext(CLASS_NAME, CLASS_NAME, zmq_strerror(zmq_errno()));
    }
}

OZMQPP::Context::Context(Context& other) :
    m_zmq_context(other.m_zmq_context),
    m_connection_mutex(),
    m_connections_id_counter (0)
{
    // context ownership is given to current class on copy
    other.m_zmq_context = nullptr;
}

OZMQPP::Context::~Context()
{
    if (m_zmq_context != nullptr)
    {
        for ( const auto& [ k, v ] : m_connection_map )
        {
            v->ContextCloseCall ();
        }

        zmq_ctx_destroy(m_zmq_context);
        m_zmq_context = nullptr;
    }
}

OZMQPP::Connection&
OZMQPP::Context::CreateConnection(SocketType socket_type)
{
    // check for valid context to create connections
    if (m_zmq_context == nullptr)
    {
        throw InvalidContext(CLASS_NAME, "CreateConnection", "Context not valid");
    }

    // convert socket type to c style zmq library resources
    int c_socket_type = -1;
    switch (socket_type)
    {
    case SocketType::PUBLISHER:
        c_socket_type = ZMQ_PUB;
        break;
    case SocketType::SUBSCRIBER:
        c_socket_type = ZMQ_SUB;
        break;
    case SocketType::REQUEST:
        c_socket_type = ZMQ_REQ;
        break;
    case SocketType::REPLY:
        c_socket_type = ZMQ_REP;
        break;
    case SocketType::ROUTER:
        c_socket_type = ZMQ_ROUTER;
        break;
    case SocketType::DEALER:
        c_socket_type = ZMQ_DEALER;
        break;
    case SocketType::PUSH:
        c_socket_type = ZMQ_PUSH;
        break;
    case SocketType::PULL:
        c_socket_type = ZMQ_PULL;
        break;
    case SocketType::PAIR:
        c_socket_type = ZMQ_PAIR;
        break;
    }

    m_connection_mutex.lock();
    void* connection = zmq_socket(m_zmq_context, c_socket_type);
    m_connection_mutex.unlock();

    uint new_connection_id = m_connections_id_counter;
    Connection* connection_ptr = new Connection(new_connection_id, connection);
    m_connection_map.insert(std::pair(new_connection_id, connection_ptr));
    ++m_connections_id_counter;
    
    return *connection_ptr;
}

OZMQPP::RouterConnection&
OZMQPP::Context::CreateRouterConnection()
{
    if (m_zmq_context == nullptr)
    {
        throw InvalidContext(CLASS_NAME, "CreateRouterConnection", zmq_strerror(zmq_errno()));
    }
    m_connection_mutex.lock();
    void* connection = zmq_socket(m_zmq_context, ZMQ_ROUTER);
    m_connection_mutex.unlock();

    uint new_connection_id = m_connections_id_counter;
    RouterConnection* connection_ptr = new RouterConnection(new_connection_id, connection);
    m_connection_map.insert(std::pair(new_connection_id, connection_ptr));
    ++m_connections_id_counter;
    
    return *connection_ptr;
}

void
OZMQPP::Context::EraseConnection (uint connection_id)
{
    std::map<uint, Connection*>::iterator iter = m_connection_map.find (connection_id);
    if (iter != m_connection_map.end())
    {
        m_connection_map.erase(connection_id);
    }

}

OZMQPP::Context&
OZMQPP::Context::operator=(Context& other)
{
    if (this == &other)
    {
        return *this;
    }
    m_zmq_context = other.m_zmq_context;
    other.m_zmq_context = nullptr;
    return *this;
}
