#include "ozmqpp/RouterConnection.hh"

// EXCEPTION headers
#include "ozmqpp/exceptions/InitializationFailed.hh"
#include "ozmqpp/exceptions/MessageNotReceived.hh"

// ZMQ headers
#include <zmq.h>

// STD headers
#include <cstring>

// File const values
static constexpr char CLASS_NAME[] = "RouterConnection";

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
        const int rc_msg_init = zmq_msg_init(&part_message);
        if (rc_msg_init != 0)
        {
            throw InitializationFailed(CLASS_NAME,
                                       "ReceiveRouterMessage",
                                       zmq_strerror(zmq_errno()));
        }

        // Receive message
        const int rc_msg_recv = zmq_msg_recv(&part_message, raw_zmq_connection, 0);
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
            const std::size_t peer_id_size = zmq_msg_size(&part_message);
            if (peer_id_size > ZMQ_PEER_ID_SIZE)
            {
                zmq_msg_close(&part_message);
                const std::string error_msg =
                    "Peer ID size expected to be " + std::to_string(ZMQ_PEER_ID_SIZE) +
                    " but got " + std::to_string(peer_id_size);
                throw MessageNotReceived(CLASS_NAME,
                                         "ReceiveRouterMessage",
                                         error_msg);
            }

            const char* peer_id_data = static_cast<char*>(zmq_msg_data(&part_message));
            PeerID peer_id{ 0 };
            std::memcpy(peer_id.data(), peer_id_data, peer_id_size);

            message.SetPeerID(peer_id);
            first_message_flag = false;
        }
        else
        {
            // Create data vector from c pointer
            const char* data_array_start = static_cast<char*>(zmq_msg_data(&part_message));
            const char* data_array_end = data_array_start + zmq_msg_size(&part_message);
            const std::vector<int8_t> frame_data(data_array_start, data_array_end);

            // Copy envelop to message
            const Frame part_msg_frame(frame_data);
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

OZMQPP::RouterConnection::RouterConnection(const unsigned int new_connection_id, void* zmq_connection) :
    Connection::Connection(new_connection_id, zmq_connection)
{

}

OZMQPP::RouterConnection::~RouterConnection() = default;
