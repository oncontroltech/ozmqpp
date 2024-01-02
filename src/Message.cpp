#include "Message.hh"

OZMQPP::Message::Message() :
    m_message_frames()
{

}

OZMQPP::Message::Message(const std::deque<Frame>& message_frames) :
    m_message_frames(message_frames)
{

}

OZMQPP::Message::Message(const Message& other) :
    m_message_frames(other.m_message_frames)
{

}

void
OZMQPP::Message::AppendFrame(const Frame& frame)
{
    m_message_frames.push_back(frame);
}

unsigned int
OZMQPP::Message::Size() const
{
    return m_message_frames.size();
}

OZMQPP::Frame
OZMQPP::Message::GetFrame(unsigned int frame_number) const
{
    return m_message_frames.at(frame_number);
}

bool
OZMQPP::Message::IsEmpty() const
{
    return m_message_frames.empty();
}

void
OZMQPP::Message::Clear()
{
    m_message_frames.clear();
}

OZMQPP::Message&
OZMQPP::Message::operator=(const Message& other)
{
    m_message_frames = other.m_message_frames;
    return *this;
}
