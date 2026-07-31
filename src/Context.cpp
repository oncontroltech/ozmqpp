#include "ozmqpp/Context.hh"

// ZMQ headers
#include <zmq.h>

// EXCEPTION headers
#include "ozmqpp/exceptions/InvalidContext.hh"

// File const values
static constexpr char CLASS_NAME[] = "Context";

OZMQPP::Context::Context() :
    m_connection_map(),
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
    m_connection_map(other.m_connection_map),
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
        for (ConnectionIterator iter = m_connection_map.begin(); iter != m_connection_map.end(); ++iter)
        {
            iter->second->ContextCloseCall();
            delete iter->second;
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
    if (connection == nullptr)
    {
        const std::string error_msg = "Failed to create connection: " + std::string(zmq_strerror(zmq_errno()));
        throw InvalidContext(CLASS_NAME, "CreateConnection", error_msg);
    }

    const std::uint32_t new_connection_id = m_connections_id_counter;
    const std::pair<ConnectionIterator, bool> new_connection_pair =
        m_connection_map.try_emplace(new_connection_id, new Connection(new_connection_id, connection));
    if (new_connection_pair.second == false)
    {
        throw InvalidContext(CLASS_NAME, "CreateConnection", "Failed to create connection: Error adding to map");
    }
    ++m_connections_id_counter;

    return *new_connection_pair.first->second;
}

OZMQPP::RouterConnection&
OZMQPP::Context::CreateRouterConnection()
{
    // check for valid context to create connections
    if (m_zmq_context == nullptr)
    {
        throw InvalidContext(CLASS_NAME, "CreateConnection", "Context not valid");
    }

    std::scoped_lock<std::mutex> guard(m_connection_mutex);
    void* connection = zmq_socket(m_zmq_context, ZMQ_ROUTER);
    if (connection == nullptr)
    {
        const std::string error_msg = "Failed to create connection: " + std::string(zmq_strerror(zmq_errno()));
        throw InvalidContext(CLASS_NAME, "CreateConnection", error_msg);
    }

    const std::uint32_t new_connection_id = m_connections_id_counter;
    RouterConnection* new_connection = new RouterConnection(new_connection_id, connection);
    const std::pair<ConnectionIterator, bool> new_connection_pair =
        m_connection_map.try_emplace(new_connection_id, new_connection);
    if (new_connection_pair.second == false)
    {
        throw InvalidContext(CLASS_NAME, "CreateConnection", "Failed to create connection: Error adding to map");
    }
    ++m_connections_id_counter;

    return *new_connection;
}

void
OZMQPP::Context::EraseConnection(const Connection& connection_ref)
{
    const ConnectionIterator iter = m_connection_map.find(connection_ref.GetUniqueID());
    if (iter != m_connection_map.end())
    {
        delete iter->second;
        m_connection_map.erase(iter);
    }
}

OZMQPP::Context&
OZMQPP::Context::operator=(Context&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }
    m_zmq_context = other.m_zmq_context;
    m_connection_map = other.m_connection_map;
    other.m_zmq_context = nullptr;
    return *this;
}
