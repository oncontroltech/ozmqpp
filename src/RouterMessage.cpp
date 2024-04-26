#include "ozmqpp/RouterMessage.hh"

// EXCEPTION headers
#include "ozmqpp/exceptions/BadAlloc.hh"

// STD headers
#include <algorithm>
#include <cstring>

// File const values
static const char CLASS_NAME[] = "RouterMessage";

OZMQPP::RouterMessage::RouterMessage() :
    Message(),
    m_peer_id_field(nullptr),
    m_peer_id_field_size(0)
{
}

OZMQPP::RouterMessage::RouterMessage(const RouterMessage& other) :
    Message(),
    m_peer_id_field(nullptr),
    m_peer_id_field_size(0)
{
    RouterMessage::operator=(other);
}

OZMQPP::RouterMessage::~RouterMessage()
{
    if (m_peer_id_field != nullptr)
    {
        delete[] m_peer_id_field;
        m_peer_id_field = nullptr;
        m_peer_id_field_size = 0;
    }
}

unsigned int
OZMQPP::RouterMessage::Size() const
{
    return Message::Size() + 1;
}

OZMQPP::Frame
OZMQPP::RouterMessage::GetFrame(unsigned int frame_number) const
{
    // Special case is present at when frame number is 0
    // the information is from peer connection address
    if (frame_number == 0)
    {
        std::vector<int8_t> id_array_value (m_peer_id_field, 
                                          m_peer_id_field + m_peer_id_field_size);
        return Frame(id_array_value);
    }
    return Message::GetFrame(frame_number - 1);
}

bool
OZMQPP::RouterMessage::IsPeerIDValid() const
{
    if (m_peer_id_field != nullptr)
    {
        return true;
    }
    return false;
}

void
OZMQPP::RouterMessage::SetPeerID(const char* const peer_id, 
                              unsigned int peer_id_size)
{
    // Check if it's valid
    if (IsPeerIDValid() == true)
    {
        ClearPeerID();
    }

    // Create space to contain peer id
    m_peer_id_field = new char[peer_id_size];
    std::memcpy(m_peer_id_field, peer_id, peer_id_size);
    m_peer_id_field_size = peer_id_size;
}

unsigned int
OZMQPP::RouterMessage::GetPeerIDSize() const
{
    return m_peer_id_field_size;
}

unsigned int
OZMQPP::RouterMessage::GetRawPeerID(char* peer_id_field,
                                 unsigned int max_size) const
{
    // Check max size to copy
    unsigned int copy_size = std::min(max_size, m_peer_id_field_size);
    
    // Check if possible to copy
    if (m_peer_id_field == nullptr)
    {
        throw BadAlloc(CLASS_NAME, "GetRawPeerID", "Failed to allocate memory");
    }

    std::memcpy(peer_id_field, m_peer_id_field, copy_size);
    return copy_size;
}

void
OZMQPP::RouterMessage::SetPeerIDVector(const std::vector<char>& peer_id_vector)
{
    // Check if data must be cleared
    if (IsEmpty() == false)
    {
        ClearPeerID();
    }

    // Get the size of vector id and allocate temporary space
    unsigned int temp_peer_id_size = peer_id_vector.size();
    char* temp_peer_id = new char[temp_peer_id_size];

    // Copy data locally
    for (std::size_t position = 0; position < peer_id_vector.size(); ++position)
    {
        temp_peer_id[position] = peer_id_vector[position];
    }

    // Attribute data
    SetPeerID(temp_peer_id, temp_peer_id_size);

    // Clear temp allocated space
    delete[] temp_peer_id;
}

std::vector<char>
OZMQPP::RouterMessage::GetPeerIDVector() const
{
    // Declare return container and record data
    std::vector<char> peer_id_container;
    for (unsigned int i = 0; i < m_peer_id_field_size; ++i)
    {
        peer_id_container.push_back(m_peer_id_field[i]);
    }

    // Return data container
    return peer_id_container;
}

void
OZMQPP::RouterMessage::ClearPeerID()
{
    if (m_peer_id_field != nullptr)
    {
        free(m_peer_id_field);
        m_peer_id_field = nullptr;
    }

    m_peer_id_field_size = 0;
}

OZMQPP::RouterMessage
OZMQPP::RouterMessage::DerivateAnswerMessage() const
{
    // Create and initialize message
    RouterMessage answer_message;
    answer_message.SetPeerID(m_peer_id_field, m_peer_id_field_size);
    
    return answer_message;
}

OZMQPP::RouterMessage&
OZMQPP::RouterMessage::operator=(const RouterMessage& other)
{
    if (this == &other)
    {
        return *this;
    }
    // Clear custom data
    ClearPeerID();
    Clear();

    // Copy peer id message
    SetPeerID(other.m_peer_id_field, other.m_peer_id_field_size);
    Message::operator=(other);

    return *this;
}
