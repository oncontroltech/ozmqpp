#include "ozmqpp/RouterMessage.hh"

// STD headers
#include <algorithm>
#include <vector>

// File const values
static constexpr char CLASS_NAME[] = "RouterMessage";

OZMQPP::RouterMessage::RouterMessage() :
    Message(),
    m_peer_id_field({0})
{
}

OZMQPP::RouterMessage::RouterMessage(const PeerID& peer_id) :
    Message(),
    m_peer_id_field(peer_id)
{
}

OZMQPP::RouterMessage::RouterMessage(const RouterMessage& other) :
    Message(),
    m_peer_id_field({0})
{
    RouterMessage::operator=(other);
}

OZMQPP::RouterMessage::~RouterMessage() = default;

std::size_t
OZMQPP::RouterMessage::Size() const
{
    return Message::Size() + 1;
}

OZMQPP::Frame
OZMQPP::RouterMessage::GetFrame(const std::size_t frame_number) const
{
    // Special case is present at when frame number is 0
    // the information is from peer connection address
    if (frame_number == 0)
    {
        const std::vector<int8_t> id_array_value(m_peer_id_field.data(), m_peer_id_field.data() + m_peer_id_field.size());
        return Frame(id_array_value);
    }
    return Message::GetFrame(frame_number - 1);
}

bool
OZMQPP::RouterMessage::IsPeerIDValid() const
{
    // Peer ID should always start with a 0 followed by 4 bytes
    if (m_peer_id_field[0] == 0)
    {
        return true;
    }
    return false;
}

void
OZMQPP::RouterMessage::SetPeerID(const PeerID& new_peer_id)
{
    m_peer_id_field = new_peer_id;
}

//std::size_t
//OZMQPP::RouterMessage::GetPeerIDSize() const
//{
//    return m_peer_id_field_size;
//}

//std::size_t
//OZMQPP::RouterMessage::GetRawPeerID(char* peer_id_field,
//                                    std::size_t max_size) const
//{
//    // Check max size to copy
//    std::size_t copy_size = std::min(max_size, m_peer_id_field_size);
//
//    // Check if possible to copy
//    if (m_peer_id_field == nullptr)
//    {
//        throw BadAlloc(CLASS_NAME, "GetRawPeerID", "Failed to allocate memory");
//    }
//
//    std::memcpy(peer_id_field, m_peer_id_field, copy_size);
//    return copy_size;
//}

//void
//OZMQPP::RouterMessage::SetPeerIDVector(const std::vector<char>& peer_id_vector)
//{
//    // Check if data must be cleared
//    if (IsEmpty() == false)
//    {
//        ClearPeerID();
//    }
//
//    // Get the size of vector id and allocate temporary space
//    //std::size_t temp_peer_id_size = peer_id_vector.size();
//    //char* temp_peer_id = new char[temp_peer_id_size];
//
//    // Copy data locally
//    //for (std::size_t position = 0; position < peer_id_vector.size(); ++position)
//    //{
//    //    temp_peer_id[position] = peer_id_vector[position];
//    //}
//
//    // Attribute data
//    SetPeerID(peer_id_vector.data(), peer_id_vector.size());
//
//    // Clear temp allocated space
//    //delete[] temp_peer_id;
//}

//std::vector<char>
//OZMQPP::RouterMessage::GetPeerIDVector() const
//{
//    // Declare return container and record data
//    std::vector<char> peer_id_container;
//    for (unsigned int i = 0; i < m_peer_id_field_size; ++i)
//    {
//        peer_id_container.push_back(m_peer_id_field[i]);
//    }
//
//    // Return data container
//    return peer_id_container;
//}

//void
//OZMQPP::RouterMessage::ClearPeerID()
//{
//    if (m_peer_id_field != nullptr)
//    {
//        free(m_peer_id_field);
//        m_peer_id_field = nullptr;
//    }
//
//    m_peer_id_field_size = 0;
//}

OZMQPP::RouterMessage
OZMQPP::RouterMessage::DerivateAnswerMessage() const
{
    // Create and initialize message
    return RouterMessage(m_peer_id_field);
}

OZMQPP::RouterMessage&
OZMQPP::RouterMessage::operator=(const RouterMessage& other)
{
    if (this == &other)
    {
        return *this;
    }
    // Clear custom data
    Clear();

    // Copy peer id message
    SetPeerID(other.m_peer_id_field);
    Message::operator=(other);

    return *this;
}
