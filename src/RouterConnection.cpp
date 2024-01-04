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



OZMQPP::RouterMessage
OZMQPP::RouterConnection::ReceiveRouterMessage()
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
            char* data_array_start = (char*)zmq_msg_data(&part_message);
            char* data_array_end = (char*)zmq_msg_data(&part_message) + zmq_msg_size(&part_message);
            std::vector<int8_t> frame_data (data_array_start, data_array_end);
            // Copy envelop to message
            Frame part_msg_frame(frame_data);
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

OZMQPP::RouterConnection::RouterConnection(uint new_connection_id, void* zmq_connection) :
    Connection::Connection(new_connection_id, zmq_connection)
{
    
}

OZMQPP::RouterConnection::~RouterConnection()
{
    
}
