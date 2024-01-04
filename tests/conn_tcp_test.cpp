// OZMQPP headers
#include <Context.hh>
#include <Connection.hh>
#include <Poll.hh>

// STD headers
#include <thread>

constexpr const char SERVER_IP_ADDRESS[] = "tcp://127.0.0.1:8080";

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
        m_client_server_connection.Connect(SERVER_IP_ADDRESS);

        OZMQPP::Message send_message;
        send_message.AppendFrame(OZMQPP::Frame(CLOSE_STRING));
        m_client_server_connection.SendMessage(send_message);
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
        m_server_client_connection.Bind(SERVER_IP_ADDRESS);

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

    return 0;
}
