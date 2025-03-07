#include "ozmqpp/Context.hh"

// ZMQ headers
#include <zmq.h>

// EXCEPTION headers
#include "ozmqpp/exceptions/InvalidContext.hh"

// File const values
static const char CLASS_NAME[] = "Context";

OZMQPP::Context::Context() :
    m_connection_mutex(),
    m_connections_id_counter(0)
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
    m_connections_id_counter(other.m_connections_id_counter)
{
    // context ownership is given to current class on copy
    other.m_zmq_context = nullptr;
}

OZMQPP::Context::~Context()
{
    if (m_zmq_context != nullptr)
    {
        for (auto map_it = m_connection_map.begin(); map_it != m_connection_map.end(); ++map_it)
        {
            map_it->second->ContextCloseCall();
            delete map_it->second;
        }
        m_connection_map.clear();

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

    m_connection_mutex.lock();
    void* connection = zmq_socket(m_zmq_context, socket_type);
    m_connection_mutex.unlock();

    unsigned int new_connection_id = m_connections_id_counter;
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

    unsigned int new_connection_id = m_connections_id_counter;
    RouterConnection* connection_ptr = new RouterConnection(new_connection_id, connection);
    m_connection_map.insert(std::pair(new_connection_id, connection_ptr));
    ++m_connections_id_counter;
    
    return *connection_ptr;
}

void
OZMQPP::Context::EraseConnection(Connection& connection_ref)
{
    for (auto map_it = m_connection_map.begin(); map_it != m_connection_map.end(); ++map_it)
    {
        if (map_it->second->GetRaw() == connection_ref.GetRaw())
        {
            delete map_it->second;
            m_connection_map.erase(map_it);
            break;
        }
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
