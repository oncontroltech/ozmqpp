#include "Poll.hh"

// EXCEPTION headers
#include "exceptions/PollingError.hh"

// File const values
static const char CLASS_NAME[] = "Poll";

ZMQ::Poll::Poll() :
    m_ids_poll()
{

}

ZMQ::Poll::Poll(const Poll& other) :
    m_ids_poll(other.m_ids_poll)
{
    SynchronizeRaw();
}

void
ZMQ::Poll::AddConnection(int id, Connection& connection)
{
    PoolBlock block{
        .id = id,
        .raw_zmqconnection = connection.GetRaw()
    };
    m_ids_poll.push_back(block);

    SynchronizeRaw();
}

void
ZMQ::Poll::RemoveConnection(int id)
{
    // Search vector for identifier
    std::vector<Poll::PoolBlock>::iterator iter = m_ids_poll.begin();
    for (; iter != m_ids_poll.end(); ++iter)
    {
        if (iter->id == id)
        {
            // If identifier found -> erase it
            m_ids_poll.erase(iter);
            break;
        }
    }

    // If not found
    if (iter == m_ids_poll.end())
    {
        throw PollingError(CLASS_NAME, "RemoveConnection", "Invalid poll id");
    }
    SynchronizeRaw();
}

int
ZMQ::Poll::Wait()
{
    // Wait for message from zmq framework
    int zmq_error_code = zmq_poll(m_raw_zmqitems.data(), m_ids_poll.size(), -1);

    // If there was an error
    if (zmq_error_code == -1)
    {
        throw PollingError(CLASS_NAME, "Wait", zmq_strerror(zmq_error_code));
    }

    // Check the number of pull item to be activated
    int unlocked_id = -1;
    for (unsigned int i = 0; i < m_ids_poll.size(); ++i)
    {
        if (m_raw_zmqitems[i].revents & ZMQ_POLLIN)
        {
            unlocked_id = m_ids_poll[i].id;
            break;
        }
    }

    return unlocked_id;
}

ZMQ::Poll&
ZMQ::Poll::operator=(const Poll& other)
{
    if (m_ids_poll != other.m_ids_poll)
    {
        std::copy(other.m_ids_poll.begin(), other.m_ids_poll.end(), m_ids_poll.begin());
    }
    SynchronizeRaw();
    return *this;
}

void
ZMQ::Poll::SynchronizeRaw()
{
    std::size_t pool_size = m_ids_poll.size();

    // Clear previous raw container
    if (m_raw_zmqitems.size() != pool_size)
    {
        m_raw_zmqitems.resize(pool_size);
    }

    // Copy items from pool to zmq raw item
    if (pool_size > 0)
    {
        for (std::size_t i = 0; i < pool_size; ++i)
        {
            m_raw_zmqitems[i].socket = m_ids_poll[i].raw_zmqconnection;
            m_raw_zmqitems[i].events = ZMQ_POLLIN;
        }
    }
}
