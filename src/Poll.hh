#ifndef ZMQ_POLL_HH
#define ZMQ_POLL_HH

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

#include "Connection.hh"

// ZMQ headers
#include <zmq.h>

// STD headers
#include <vector>

namespace ZMQ
{
//! @brief Wrapper for zeromq poll resources.
//!
//! The zeromq permit to observe multiple connection to check for incoming
//! messages.
//!
class ZMQ_API Poll
{
public:

    //! @brief Default constructor.
    //!
    Poll();

    //! @brief Copy constructor.
    //!
    //! @param other object to copy information from.
    //!
    Poll(const Poll& other);

    //! @brief Class destructor.
    //!
    ~Poll() = default;

    //! @brief Add connection resource to pull with identification.
    //!
    //! @param id Poll id.
    //! @param connection Connection that triggers the poll.
    //!
    void AddConnection(int id, Connection& connection);

    //! @brief Remove connection using identification.
    //!
    //! @param id Poll id.
    //!
    void RemoveConnection(int id);

    //! @brief Wait for incoming message.
    //!
    int Wait();

    //! @brief Copy assignment method.
    //!
    Poll& operator=(const Poll& other);

protected:

    //! @brief Representation of data needed to record on.
    //!
    struct PoolBlock
    {
        int id;

        void* raw_zmqconnection;

        inline bool operator==(const PoolBlock& other) const
        {
            if (other.id == id && other.raw_zmqconnection == raw_zmqconnection)
            {
                return true;
            }
            return false;
        }
    };

    //! @brief Synchronize m_raw_zmqitems with m_ids_poll.
    //!
    void SynchronizeRaw();

private:

    //! @brief Pool with identification of ids.
    //!
    //! While inserting/removing data on poll, raw zqm items must be
    //! synchronized.
    //!
    //! @sa SynchronizeRaw
    //! @sa m_raw_zmqitems
    //!
    std::vector<PoolBlock> m_ids_poll;

    //! @brief Raw zeromq pollitem array.
    //!
    //! Data used for Wait method, and is not created internally to the method
    //! in order to improve run-time performance.
    //!
    //! Member updated whenever the m_ids_poll is changed.
    //!
    //! @sa m_ids_poll
    //! @sa Wait
    //!
    std::vector<zmq_pollitem_t> m_raw_zmqitems;
};
}

#endif // ZMQ_POLL_HH
