// OZMQPP headers
#include <ozmqpp/Context.hh>
#include <ozmqpp/Connection.hh>
#include <ozmqpp/Poll.hh>

// STD headers
#include <thread>

constexpr const char SERVER_INPROC_ADDRESS[] = "inproc://test_ozmqpp";

constexpr const char CLOSE_STRING[] = "goodbye world";

class ClientThread
{
public:
    ClientThread(OZMQPP::Connection& client_server_connection) :
        m_client_server_connection(client_server_connection)
    {

    }

    void operator()()
    {
        m_client_server_connection.Connect(SERVER_INPROC_ADDRESS);

        OZMQPP::Message end_message;
        end_message.AppendFrame(OZMQPP::Frame(CLOSE_STRING));
        m_client_server_connection.SendMessage(end_message);
    }

protected:

private:
    OZMQPP::Connection& m_client_server_connection;
};

class ServerThread
{
public:
    ServerThread(OZMQPP::RouterConnection& server_client_connection) :
        m_server_client_connection(server_client_connection)
    {

    }

    void operator()()
    {
        m_server_client_connection.Bind(SERVER_INPROC_ADDRESS);

        OZMQPP::Poll server_pool;
        server_pool.AddConnection(0, m_server_client_connection);

        bool terminate_cicle = false;
        do
        {
            int active_connection_id = server_pool.Wait();
            switch (active_connection_id)
            {
            case 0:
                terminate_cicle = true;
                break;
            }

        } while (terminate_cicle == false);
    }

protected:

private:
    OZMQPP::RouterConnection& m_server_client_connection;
};

int main()
{
    OZMQPP::Context zmq_context;

    OZMQPP::RouterConnection& server_connection =
        zmq_context.CreateRouterConnection();

    ServerThread server(server_connection);

    std::thread server_thread(server);

    OZMQPP::Connection& client_connection =
        zmq_context.CreateConnection(OZMQPP::SocketType::DEALER);

    ClientThread client(client_connection);

    std::thread client_thread(client);

    server_thread.join();

    client_thread.join();

    zmq_context.EraseConnection(server_connection);

    zmq_context.EraseConnection(client_connection);

    return 0;
}
