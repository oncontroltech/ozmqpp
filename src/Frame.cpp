#include "ozmqpp/Frame.hh"

// STD headers
#include <cstring>
#include <algorithm>

// EXCEPTION headers
#include "ozmqpp/exceptions/BadAlloc.hh"
#include "ozmqpp/exceptions/InitializationFailed.hh"

// File const values
static const char CLASS_NAME[] = "Frame";

OZMQPP::Frame::Frame()
{

}

OZMQPP::Frame::Frame(const std::vector<int8_t>& frame_data) :
    m_frame_data (frame_data)
{
}

OZMQPP::Frame::Frame(const std::string& message_text)
{
    std::vector<int8_t> message_vector (message_text.begin(), message_text.end());
    
    m_frame_data = message_vector;
}

OZMQPP::Frame::Frame(const Frame& other) :
    m_frame_data(other.m_frame_data)
{
}

OZMQPP::Frame::~Frame()
{
    // ClearFrameMemoryContainer();
}

bool
OZMQPP::Frame::IsEmpty() const
{
    bool is_frame_empy = (m_frame_data.size() == 0);
    return is_frame_empy;
}

size_t
OZMQPP::Frame::GetFrameMessageSize() const
{
    return m_frame_data.size();
}

void
OZMQPP::Frame::SetFrameData(const std::vector<int8_t>& frame_data)
{
    m_frame_data = frame_data;
}

std::vector<int8_t>
OZMQPP::Frame::GetFrameData() const
{
    return m_frame_data;
}

void
OZMQPP::Frame::SetMessageString(const std::string& message)
{
    std::vector<int8_t> message_vector (message.begin(), message.end());
    m_frame_data = message_vector;
}

std::string
OZMQPP::Frame::GetMessageString() const
{
    std::string data_message (reinterpret_cast<const char*>(m_frame_data.data()), m_frame_data.size());

    // Return string structure
    return data_message;
}

OZMQPP::Frame&
OZMQPP::Frame::operator=(const Frame& other)
{
    if (this == &other)
    {
        return *this;
    }

     // Copy data to class
    m_frame_data = other.m_frame_data;
    
    // Return self
    return *this;
}

