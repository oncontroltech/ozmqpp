#include "RouterConnection.hh"

// ZMQ headers
#include <zmq.h>

// EXCEPTION headers
#include "exceptions/InitializationFailed.hh"
#include "exceptions/MessageNotReceived.hh"

// STD headers
#include <cstring>

// File const values
static const char CLASS_NAME[] = "RouterConnection";

ZMQ::RouterConnection::RouterConnection(RouterConnection& other) :
    Connection::Connection(other)
{

}

ZMQ::RouterMessage
ZMQ::RouterConnection::ReceiveRouterMessage()
{
    
    // Return a pointer to raw zmq connection
    void* raw_zmq_connection = GetRaw();

    RouterMessage message;

    // Read all messages from
    int more = 0;
    size_t more_size = sizeof(more);
    // Flag which going to false value after first message
    bool first_message_flag = true;
    do
    {
        // Init message
        zmq_msg_t part_message;
        int rc_msg_init = zmq_msg_init(&part_message);
        if (rc_msg_init != 0)
        {
            throw InitializationFailed(CLASS_NAME,
                                       "ReceiveRouterMessage",
                                       zmq_strerror(zmq_errno()));
        }

        // Receive message
        int rc_msg_recv = zmq_msg_recv(&part_message, raw_zmq_connection, 0);
        if (rc_msg_recv == -1)
        {
            // Close message
            zmq_msg_close(&part_message);
            throw MessageNotReceived(CLASS_NAME,
                                     "ReceiveRouterMessage",
                                     zmq_strerror(zmq_errno()));
        }

        if (first_message_flag == true)
        {
            message.SetPeerID((char*)zmq_msg_data(&part_message),
                              zmq_msg_size(&part_message));
            first_message_flag = false;
        }
        else
        {
            // Copy envelop to message
            Frame part_msg_frame((char*)zmq_msg_data(&part_message),
                                 zmq_msg_size(&part_message));
            message.AppendFrame(part_msg_frame);
        }
        // Close message
        zmq_msg_close(&part_message);

        // Check if exist more message parts
        zmq_getsockopt(raw_zmq_connection, ZMQ_RCVMORE, &more, &more_size);

    } while (more != 0); // Check for last message frame

    // Return wrapper message
    return message;

}

ZMQ::RouterConnection::RouterConnection(void* zmq_connection) :
    Connection::Connection(zmq_connection)
{
    
}
