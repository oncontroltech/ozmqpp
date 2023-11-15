#include "Connection.hh"

// ZMQ headers
#include <zmq.h>

// EXCEPTION headers
#include "exceptions/InitializationFailed.hh"
#include "exceptions/MessageNotReceived.hh"
#include "exceptions/MessageNotSent.hh"

// File const values
static const char CLASS_NAME[] = "Connection";

ZMQ::Connection::Connection(Connection& other) :
    m_zmq_connection(other.m_zmq_connection)
{
    other.m_zmq_connection = nullptr;
}

ZMQ::Connection::~Connection()
{
    if (m_zmq_connection != nullptr)
    {
        zmq_close(m_zmq_connection);
        m_zmq_connection = nullptr;
    }
}

void
ZMQ::Connection::Bind(const std::string& address_string)
{
    int rc = zmq_bind(m_zmq_connection, address_string.c_str());
    if (rc == -1)
    {
        throw InitializationFailed(CLASS_NAME, "Bind", zmq_strerror(zmq_errno()));
    }
}

void
ZMQ::Connection::Connect(const std::string& address_string)
{
    int rc = zmq_connect(m_zmq_connection, address_string.c_str());
    if (rc == -1)
    {
        throw InitializationFailed(CLASS_NAME, "Connect", zmq_strerror(zmq_errno()));
    }
}

bool
ZMQ::Connection::IsValid() const
{
    return(m_zmq_connection != nullptr);
}

void
ZMQ::Connection::SendMessage(const Message& message)
{
    // Check for number envelops to send
    unsigned int number_multiparts = message.Size();
    for(unsigned int i = 0; i < number_multiparts; ++i)
    {
        // Get the raw pointer to message string
        Frame frame = message.GetFrame(i);
        unsigned int frame_information_size = frame.GetFrameMessageSize();
        // char* frame_information = new char[frame_information_size];
        zmq_msg_t message_struct;
        if (zmq_msg_init_size(&message_struct, frame_information_size) == -1)
        {
            throw InitializationFailed(CLASS_NAME, "SendMessage", zmq_strerror(zmq_errno()));
        }
        frame.GetFrameInformation(reinterpret_cast<char *>(zmq_msg_data(&message_struct)), frame_information_size);
        // frame.GetFrameInformation(frame_information, frame_information_size);

        // Copy message string
        // zmq_msg_t message_struct;
        // if (zmq_msg_init_size(&message_struct, frame_information_size) == -1)
        // {
        //     throw InitializationFailed(CLASS_NAME, "SendMessage", zmq_strerror(zmq_errno()));
        // }
        // memmove(zmq_msg_data(&message_struct), 
        //        frame_information, 
        //        frame_information_size);

        // // Delete information
        // delete[] frame_information;
        // frame_information = nullptr;
        // frame_information_size = 0;

        // Check flags for multipart message
        int flags;
        if ((i + 1) < number_multiparts)
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
            int zmq_error_code = zmq_errno();
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

ZMQ::Message
ZMQ::Connection::ReceiveMessage()
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
        int rc_msg_init = zmq_msg_init(&part_message);
        if (rc_msg_init != 0)
        {
            throw InitializationFailed(CLASS_NAME, "ReceiveMessage", zmq_strerror(zmq_errno()));
        }

        // receive message
        int rc_msg_recv = zmq_msg_recv(&part_message, m_zmq_connection, 0);
        if (rc_msg_recv == -1)
        {
            // Save the errno code to throw later
            int zmq_error_code = zmq_errno();
            // If the message is valid close it
            if (zmq_error_code != EFAULT)
            {
                // close message
                zmq_msg_close(&part_message);
            }
            throw MessageNotReceived(CLASS_NAME, "ReceiveMessage", zmq_strerror(zmq_error_code));
        }

        // copy envelop to message
        Frame part_msg_frame(reinterpret_cast<char*>(zmq_msg_data(&part_message)),
                             zmq_msg_size(&part_message));
        message.AppendFrame(part_msg_frame);

        // close message
        zmq_msg_close(&part_message);

        // check if exist more message parts
        zmq_getsockopt(m_zmq_connection, ZMQ_RCVMORE, &more, &more_size);

    } while (more != 0); // check for last message frame

    //return wrapper message
    return message;
}

void*
ZMQ::Connection::GetRaw()
{
    return m_zmq_connection;
}

ZMQ::Connection&
ZMQ::Connection::operator=(const Connection& other)
{
    if (this == &other)
    {
        return *this;
    }
    m_zmq_connection = other.m_zmq_connection;
    return *this;
}

ZMQ::Connection& 
ZMQ::Connection::operator=(Connection&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }
    m_zmq_connection = other.m_zmq_connection;
    other.m_zmq_connection = nullptr;
    return *this;
}

ZMQ::Connection::Connection(void* raw_connection) :
    m_zmq_connection(raw_connection)
{
    
}

ZMQ::Connection
ZMQ::make_invalid_zmqconnection()
{
    // create and assign semaphore resource
    // The creation of the shared must be with a raw pointer because the class 
    // does not have public constructor. make_invalid_zmqconnection is friend 
    // from class.
    return Connection(nullptr);
} 