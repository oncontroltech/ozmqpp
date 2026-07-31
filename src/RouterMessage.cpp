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

OZMQPP::PeerID
OZMQPP::RouterMessage::GetPeerID() const
{
    return m_peer_id_field;
}

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
