#ifndef ZMQ_FRAME_HH
#define ZMQ_FRAME_HH

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

// STD headers
#include <string>
#include <vector>

namespace OZMQPP
{
//! @brief zeromq envelope wrapper.
//!
//! The envelop is characterized by a continuous array of characters. This
//! class contain the resources to needed to represent the zeromq message
//! envelope.
//!
//! The bytes do not always represent a valid string, where only the null
//! character exist at the end of the array of bytes. In this class, if the
//! data array can be a valid string, can also return a string object.
//!
class ZMQ_API Frame
{
public:
    //! @brief Default class constructor.
    //!
    //! Initialization of internal resources.
    //!
    Frame();

    //! @brief Constructor with copy of raw information.
    //!
    //! The information is copied, with internal space allocation.
    //!
    //! @param frame_data vector of bytes representing frame information.
    //!
    Frame(const std::vector<int8_t>& frame_data);


    //! @brief Constructor with initialization by a string (c++ style).
    //!
    //! @param message_text Envelop character string.
    //!
    explicit Frame(const std::string& message_text);

    //! @brief Copy constructor.
    //!
    //! @param other object to copy information from.
    //!
    Frame(const Frame& other);

    //! @brief Class destructor.
    //!
    ~Frame();

    //! @brief Check if Frame is empty.
    //!
    //! The frame is considered empty if no memory is allocated.
    //!
    //! @return true if no allocated memory, false otherwise.
    //! 
    [[nodiscard]] bool IsEmpty() const;

    //! @brief Retrieve the size information.
    //!
    //! @return size of frame information in bytes.
    //!
    [[nodiscard]] size_t GetFrameMessageSize() const;

    //! @brief Set frame information in raw data.
    //!
    //! The information will be copied into frame internal containers. The 
    //! message_information memory allocation must be handled elsewhere.
    //!
    //! @param message_information raw information array.
    //! @param message_size raw information array size.
    //!
    void SetFrameData(const std::vector<int8_t>& frame_data);

    //! @brief Retrieve raw data frame information.
    //!
    //! The information contained in the frame is copied into message_information
    //! container. 
    //!
    //! @return values
    //!
    std::vector<int8_t> GetFrameData () const;

    //! @brief Set the envelope characters by std::string.
    //!
    //! @param message Envelope characters stream as std::string.
    //!
    void SetMessageString(const std::string& message);

    //! @brief Retrieve the envelope memory as std::string.
    //!
    //! @return Envelope characters stream as std::string.
    //!
    [[nodiscard]] std::string GetMessageString() const;

    //! @brief Copy operator overload.
    //!
    //! @param socket_type type of socket to create.
    //! @return Valid ZMQConnection object to be used on communication.
    //!
    Frame& operator=(const Frame& other);

protected:


private:

    //! @brief Buffer containing the information contained in the frame.
    //!
    std::vector<int8_t> m_frame_data;

}; // class Frame

} // namespace OZMQPP

#endif // ZMQ_FRAME_HH
