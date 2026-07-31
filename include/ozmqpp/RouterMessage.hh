#ifndef ZMQ_ROUTERMESSAGE_HH
#define ZMQ_ROUTERMESSAGE_HH

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
#include "Message.hh"

// STD headers
#include <cstdint>
#include <array>

namespace OZMQPP
{
//! @brief Size of the peer id frame
//!
//! Peer ID should start by 0 followed by 4 bytes
constexpr std::uint8_t ZMQ_PEER_ID_SIZE = 5;

//! @brief Type used for Peer ID
//!
using PeerID = std::array<char, ZMQ_PEER_ID_SIZE>;

//! @brief Message adapted to ZMQ router message structure.
//!
//! The ZMQ router have a different message structure from other request-reply
//! pattern mechanism. It contains two, or more, mandatory message frames:
//!
//! <ol>
//!   <li> ID frame (it is not a string structure. Initial chapter is zero).
//!   <li> Message frame 0
//!   <li> Message frame ...
//! </ol>
//!
class ZMQ_API RouterMessage : public Message
{
public:
    //! @brief Default class constructor.
    //!
    RouterMessage();

    //! @brief Class constructor with default Peer ID.
    //!
    explicit RouterMessage(const PeerID& peer_id);

    //! @brief Copy constructor.
    //!
    //! @param other object to copy information from.
    //!
    RouterMessage(const RouterMessage& other);

    //! @brief Class destructor.
    //!
    ~RouterMessage() override;

    //! @brief Get number of frames on message.
    //!
    //! The first frame of the message is the ID of the peer related with
    //! current message.
    //!
    //! @return Number of frames in the message.
    //!
    [[nodiscard]] std::size_t Size() const override;

    //! @brief Get frame from message to transmit.
    //!
    //! Attention is taken when frame_number id is zero, which constructs a
    //! frame with the peer ID.
    //!
    //! @param frame_number ID of the frame to send.
    //! @return Frame with the information to send.
    //!
    [[nodiscard]] Frame GetFrame(const std::size_t frame_number) const override;

    //! @brief Check ig the message contain a valid ID information.
    //!
    //! The information is considered valid if the id starts with a 0.
    //!
    //! @return True if id field is valid.
    //!
    [[nodiscard]] bool IsPeerIDValid() const;

    //! @brief Record ID data into router message object.
    //!
    //! All data is copied to objects inside the class. No deallocation of
    //! parameter resources is handled inside method.
    //!
    //! @param new_peer_id Array containing peer id data.
    //!
    void SetPeerID(const PeerID& new_peer_id);

    //! @brief Retrieve peer if information by std::vector object (c++ style).
    //!
    //! @return peer ID in std::vector object format.
    //!
    [[nodiscard]] PeerID GetPeerID() const;

    //! @brief Create a resource-initialize message for answer this one.
    //!
    //! The router communication pattern must contain the device to answer
    //! current message.
    //!
    //! This method creates a message with all resources initialized
    //! for a correct answer.
    //!
    //! @return Message with initialize resources for the answer.
    //!
    [[nodiscard]] RouterMessage DerivateAnswerMessage() const;

    //! @brief equal operator.
    //!
    //! @param other object to copy information from.
    //! @return Reference to self.
    //!
    RouterMessage& operator=(const RouterMessage& other);

private:
    //! @brief buffer to contain id information.
    //!
    //! The id can be not in string format, as so it must be a raw array
    //! of bytes.
    //!
    PeerID m_peer_id_field;
}; // class RouterMessage

} // namespace OZMQPP

#endif // ZMQ_ROUTERMESSAGE_HH
