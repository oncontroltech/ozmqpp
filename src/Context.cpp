#include "Context.hh"

// ZMQ headers
#include <zmq.h>

// EXCEPTION headers
#include "exceptions/InvalidContext.hh"

// File const values
static const char CLASS_NAME[] = "Context";

ZMQ::Context::Context() :
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

ZMQ::Context::Context(const Context& other) :
    m_zmq_context(other.m_zmq_context),
    m_connection_mutex()
{
}

ZMQ::Connection
ZMQ::Context::CreateConnection(int socket_type)
{
    if (m_zmq_context == nullptr)
    {
        throw InvalidContext(CLASS_NAME, "CreateConnection", zmq_strerror(zmq_errno()));
    }
    m_connection_mutex.lock();
    void* connection = zmq_socket(m_zmq_context, socket_type);
    m_connection_mutex.unlock();
    return Connection(connection);
}

ZMQ::RouterConnection
ZMQ::Context::CreateRouterConnection()
{
    if (m_zmq_context == nullptr)
    {
        throw InvalidContext(CLASS_NAME, "CreateRouterConnection", zmq_strerror(zmq_errno()));
    }
    m_connection_mutex.lock();
    void* connection = zmq_socket(m_zmq_context, ZMQ_ROUTER);
    m_connection_mutex.unlock();
    return RouterConnection(connection);
}

ZMQ::Context&
ZMQ::Context::operator=(const Context& other)
{
    if (this == &other)
    {
        return *this;
    }
    m_zmq_context = other.m_zmq_context;
    return *this;
}
