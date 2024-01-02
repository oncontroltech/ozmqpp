#ifndef ZMQ_MESSAGE_HH
#define ZMQ_MESSAGE_HH

#ifdef _WIN32
    #ifdef ZMQ_BUILD
        #define ZMQ_API __declspec(dllexport)
    #else
        #define ZMQ_API __declspec(dllimport)
    #endif
#elif __unix__
    #ifndef ZMQ_API
        #define ZMQ_API
    #endif
#endif

// ZMQ headers
#include "Frame.hh"

// STD headers
#include <deque>

namespace OZMQPP
{

//! @brief zeromq message container.
//!
//! In zeromq, messages is defined has a group of text envelopes. This
//! mechanism is wrapper in this class.
//!
class ZMQ_API Message
{
public:

    //! @brief Default class constructor.
    //!
    Message();

    //! @brief Constructor with complete frame container.
    //!
    explicit Message(const std::deque<Frame>& message_frames);

    //! @brief Copy constructor.
    //!
    Message(const Message& other);

    //! @brief Class destructor.
    //!
    virtual ~Message() = default;

    //! @brief Add frame to message.
    //!
    //! Frame is added at the end of the message.
    //!
    //! @param frame Frame to append to the message.
    //!
    virtual void AppendFrame(const Frame& frame);

    //! @brief Retrieve the number of message envelopes.
    //!
    //! @return Number of frames in the message.
    //!
    [[nodiscard]] virtual unsigned int Size() const;

    //! @brief Get frame by number.
    //!
    //! @param frame_number ID of the frame to send.
    //! @return Frame with the information to send.
    //!
    [[nodiscard]] virtual Frame GetFrame(unsigned int frame_number) const;

    //! @brief Check if message has no envelopes.
    //!
    //! @return True if message has no envelops.
    //!
    [[nodiscard]] virtual bool IsEmpty() const;

    //! @brief Erase all envelopes from message.
    //!
    //! @brief Erase all message content.
    //!
    virtual void Clear();

    //! @brief Copy operator overload.
    //!
    //! @param other object to copy information from.
    //! @return Reference to self.
    //!
    Message& operator=(const Message& other);

protected:

private:
    //! @brief constituents message frames.
    //!
    //! Each zeromq is constituted by zero or more data frame. These
    //! frames are recorded here to be used further.
    //!
    std::deque<Frame> m_message_frames;

}; // class Message

} // namespace OZMQPP

#endif // ZMQ_MESSAGE_HH
