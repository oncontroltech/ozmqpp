#include "ozmqpp/Connection.hh"

// ZMQ headers
#include <zmq.h>
#include <cstring>

// EXCEPTION headers
#include "ozmqpp/exceptions/InitializationFailed.hh"
#include "ozmqpp/exceptions/MessageNotReceived.hh"
#include "ozmqpp/exceptions/MessageNotSent.hh"

// File const values
static constexpr char CLASS_NAME[] = "Connection";

OZMQPP::Connection::Connection(Connection&& other) :
    m_connection_unique_id(other.m_connection_unique_id),
    m_zmq_connection(other.m_zmq_connection),
    m_connection_status(other.m_connection_status)
{
    other.m_zmq_connection = nullptr;
}

OZMQPP::Connection::~Connection()
{
    if (m_connection_status == ConnectionStatus::CONNECTED)
    {
        Disconnect();
    }
    else if (m_connection_status == ConnectionStatus::BOUND)
    {
        Unbind();
    }

    if (m_zmq_connection != nullptr)
    {
        zmq_close(m_zmq_connection);
        m_zmq_connection = nullptr;
    }
}

OZMQPP::Connection::ConnectionStatus
OZMQPP::Connection::GetConnectionStatus() const
{
    return m_connection_status;
}

void
OZMQPP::Connection::Bind(const std::string& address_string)
{
    if (m_connection_status != ConnectionStatus::NOT_CONNECTED)
    {
        throw InitializationFailed(CLASS_NAME, "Bind", "Already Connected/Bound");
    }
    if (address_string.empty() == true)
    {
        throw InitializationFailed(CLASS_NAME, "Bind", "Invalid address");
    }

    const int rc = zmq_bind(m_zmq_connection, address_string.c_str());
    if (rc == -1)
    {
        throw InitializationFailed(CLASS_NAME, "Bind", zmq_strerror(zmq_errno()));
    }

    m_endpoint = address_string;
}

void
OZMQPP::Connection::Unbind()
{
    if (m_connection_status != ConnectionStatus::BOUND)
    {
        throw InitializationFailed(CLASS_NAME, "Unbind", "Not bound or is connected");
    }

    const int rc = zmq_unbind(m_zmq_connection, m_endpoint.c_str());
    if (rc == -1)
    {
        throw InitializationFailed(CLASS_NAME, "Unbind", zmq_strerror(zmq_errno()));
    }

    m_connection_status = ConnectionStatus::NOT_CONNECTED;
}

void
OZMQPP::Connection::Connect(const std::string& address_string)
{
    if (m_connection_status != ConnectionStatus::NOT_CONNECTED)
    {
        throw InitializationFailed(CLASS_NAME, "Connect", "Already Connected/Bound");
    }
    if (address_string.empty() == true)
    {
        throw InitializationFailed(CLASS_NAME, "Connect", "Invalid address");
    }

    const int rc = zmq_connect(m_zmq_connection, address_string.c_str());
    if (rc == -1)
    {
        throw InitializationFailed(CLASS_NAME, "Connect", zmq_strerror(zmq_errno()));
    }

    m_endpoint = address_string;
}

void
OZMQPP::Connection::Disconnect()
{
    if (m_connection_status != ConnectionStatus::CONNECTED)
    {
        throw InitializationFailed(CLASS_NAME, "Disconnect", "Not connected or is bound");
    }

    const int rc = zmq_disconnect(m_zmq_connection, m_endpoint.c_str());
    if (rc == -1)
    {
        throw InitializationFailed(CLASS_NAME, "Disconnect", zmq_strerror(zmq_errno()));
    }

    m_connection_status = ConnectionStatus::NOT_CONNECTED;
}

bool
OZMQPP::Connection::IsValid() const
{
    return m_zmq_connection != nullptr;
}

void
OZMQPP::Connection::SendMessage(const Message& message)
{
    // Check for number envelops to send
    const std::size_t number_multi_parts = message.Size();
    for (std::size_t i = 0; i < number_multi_parts; ++i)
    {
        // Get the raw pointer to message string
        const Frame frame = message.GetFrame(i);
        const std::size_t frame_information_size = frame.GetFrameMessageSize();

        zmq_msg_t message_struct;
        if (zmq_msg_init_size(&message_struct, frame_information_size) == -1)
        {
            throw InitializationFailed(CLASS_NAME, "SendMessage", zmq_strerror(zmq_errno()));
        }
        const std::vector<int8_t> frame_data = frame.GetFrameData();
        // frame.GetFrameInformation(reinterpret_cast<char *>(zmq_msg_data(&message_struct)), frame_information_size);
        // zmq_msg_data(&message_struct));

        memcpy(zmq_msg_data(&message_struct), frame_data.data(), frame_data.size());

        // Check flags for multipart message
        int flags;
        if (i + 1 < number_multi_parts)
        {
            flags = ZMQ_SNDMORE;
        }
        else
        {
            flags = 0;
        }

        // Send message
        if (zmq_msg_send(&message_struct, m_zmq_connection, flags) == -1)
        {
            // Save the errno code to throw later
            const int zmq_error_code = zmq_errno();
            // If the message is valid close it
            if (zmq_error_code != EFAULT)
            {
                // close message
                zmq_msg_close(&message_struct);
            }
            throw MessageNotSent(CLASS_NAME, "SendMessage", zmq_strerror(zmq_error_code));
        }

        // close message
        if (zmq_msg_close(&message_struct) == -1)
        {
            throw MessageNotSent(CLASS_NAME, "SendMessage", zmq_strerror(zmq_errno()));
        }
    }
}

OZMQPP::Message
OZMQPP::Connection::ReceiveMessage()
{
    // retrieve message from zeromq framework
    Message message;
    // read all messages from
    int more = 0;
    size_t more_size = sizeof(more);
    do
    {
        // init message
        zmq_msg_t part_message;
        const int rc_msg_init = zmq_msg_init(&part_message);
        if (rc_msg_init != 0)
        {
            throw InitializationFailed(CLASS_NAME, "ReceiveMessage", zmq_strerror(zmq_errno()));
        }

        // receive message
        const int rc_msg_recv = zmq_msg_recv(&part_message, m_zmq_connection, 0);
        if (rc_msg_recv == -1)
        {
            // Save the errno code to throw later
            const int zmq_error_code = zmq_errno();
            // If the message is valid close it
            if (zmq_error_code != EFAULT)
            {
                // close message
                zmq_msg_close(&part_message);
            }
            throw MessageNotReceived(CLASS_NAME, "ReceiveMessage", zmq_strerror(zmq_error_code));
        }

        // copy envelop to message
        const int8_t* msg_data = static_cast<int8_t*>(zmq_msg_data(&part_message));
        const std::vector<int8_t> frame_raw_data(msg_data, msg_data + zmq_msg_size(&part_message));
        Frame part_msg_frame;
        part_msg_frame.SetFrameData(frame_raw_data);

        message.AppendFrame(part_msg_frame);

        // close message
        zmq_msg_close(&part_message);

        // check if exist more message parts
        zmq_getsockopt(m_zmq_connection, ZMQ_RCVMORE, &more, &more_size);
    }
    while (more != 0); // check for last message frame

    //return wrapper message
    return message;
}

void*
OZMQPP::Connection::GetRaw()
{
    return m_zmq_connection;
}

unsigned int
OZMQPP::Connection::GetUniqueID() const
{
    return m_connection_unique_id;
}

void
OZMQPP::Connection::ContextCloseCall()
{
}

OZMQPP::Connection&
OZMQPP::Connection::operator=(Connection&& other)
{
    m_connection_unique_id = other.m_connection_unique_id;
    m_zmq_connection = other.m_zmq_connection;
    m_connection_status = other.m_connection_status;
    other.m_zmq_connection = nullptr;
    other.m_connection_status = ConnectionStatus::NOT_CONNECTED;
    return *this;
}

OZMQPP::Connection::Connection(const unsigned int connection_unique_id, void* raw_zmq_connection) :
    m_connection_unique_id(connection_unique_id),
    m_zmq_connection(raw_zmq_connection),
    m_connection_status(ConnectionStatus::NOT_CONNECTED)
{
}
