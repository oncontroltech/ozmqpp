#include "ozmqpp/Poll.hh"

// EXCEPTION headers
#include "ozmqpp/exceptions/PollingError.hh"

// File const values
static constexpr char CLASS_NAME[] = "Poll";

OZMQPP::Poll::Poll() :
    m_ids_poll()
{

}

OZMQPP::Poll::Poll(const Poll& other) :
    m_ids_poll(other.m_ids_poll)
{
    SynchronizeRaw();
}

void
OZMQPP::Poll::AddConnection(const int id, Connection& connection)
{
    const PoolBlock block{
        .id = id,
        .raw_zmqconnection = connection.GetRaw()
    };
    m_ids_poll.push_back(block);

    SynchronizeRaw();
}

void
OZMQPP::Poll::RemoveConnection(const int id)
{
    // Search vector for identifier
    std::vector<Poll::PoolBlock>::iterator iter = m_ids_poll.begin();
    // If not found
    if (iter == m_ids_poll.end())
    {
        throw PollingError(CLASS_NAME, "RemoveConnection", "Invalid poll id");
    }

    for (; iter != m_ids_poll.end(); ++iter)
    {
        if (iter->id == id)
        {
            // If identifier found -> erase it
            m_ids_poll.erase(iter);
            break;
        }
    }

    SynchronizeRaw();
}

int
OZMQPP::Poll::Wait()
{
    // Wait for message from zmq framework
    const int zmq_error_code = zmq_poll(m_raw_zmqitems.data(), static_cast<int>(m_ids_poll.size()), -1);

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

OZMQPP::Poll&
OZMQPP::Poll::operator=(const Poll& other)
{
    if (m_ids_poll != other.m_ids_poll)
    {
        std::ranges::copy(other.m_ids_poll, m_ids_poll.begin());
    }
    SynchronizeRaw();
    return *this;
}

void
OZMQPP::Poll::SynchronizeRaw()
{
    const std::size_t pool_size = m_ids_poll.size();

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
