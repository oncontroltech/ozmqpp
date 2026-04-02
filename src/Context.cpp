#include "ozmqpp/Context.hh"

// ZMQ headers
#include <zmq.h>

// EXCEPTION headers
#include "ozmqpp/exceptions/InvalidContext.hh"

// File const values
static constexpr char CLASS_NAME[] = "Context";

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
        for (auto& [connection_id, connection] : m_connection_map)
        {
            connection.ContextCloseCall();
        }
        m_connection_map.clear();

        zmq_ctx_destroy(m_zmq_context);
        m_zmq_context = nullptr;
    }
}

OZMQPP::Connection&
OZMQPP::Context::CreateConnection(const SocketType socket_type)
{
    // check for valid context to create connections
    if (m_zmq_context == nullptr)
    {
        throw InvalidContext(CLASS_NAME, "CreateConnection", "Context not valid");
    }

    std::scoped_lock<std::mutex> guard(m_connection_mutex);
    void* connection = zmq_socket(m_zmq_context, static_cast<int>(socket_type));

    const std::uint32_t new_connection_id = m_connections_id_counter;
    const std::pair<ConnectionIterator, bool>& new_connection_pair =
        m_connection_map.try_emplace(new_connection_id, Connection(new_connection_id, connection));
    ++m_connections_id_counter;
    
    return new_connection_pair.first->second;
}

OZMQPP::RouterConnection&
OZMQPP::Context::CreateRouterConnection()
{
    return dynamic_cast<OZMQPP::RouterConnection&>(CreateConnection(SocketType::ROUTER));
}

void
OZMQPP::Context::EraseConnection(const Connection& connection_ref)
{
    if (m_connection_map.contains(connection_ref.GetUniqueID()) == true)
    {
        m_connection_map.erase(connection_ref.GetUniqueID());
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
