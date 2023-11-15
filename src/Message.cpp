#include "Message.hh"

ZMQ::Message::Message() :
    m_message_frames()
{

}

ZMQ::Message::Message(const std::deque<Frame>& message_frames) :
    m_message_frames(message_frames)
{

}

ZMQ::Message::Message(const Message& other) :
    m_message_frames(other.m_message_frames)
{

}

void
ZMQ::Message::AppendFrame(const Frame& frame)
{
    m_message_frames.push_back(frame);
}

unsigned int
ZMQ::Message::Size() const
{
    return m_message_frames.size();
}

ZMQ::Frame
ZMQ::Message::GetFrame(unsigned int frame_number) const
{
    return m_message_frames.at(frame_number);
}

bool
ZMQ::Message::IsEmpty() const
{
    return m_message_frames.empty();
}

void
ZMQ::Message::Clear()
{
    m_message_frames.clear();
}

ZMQ::Message&
ZMQ::Message::operator=(const Message& other)
{
    m_message_frames = other.m_message_frames;
    return *this;
}
