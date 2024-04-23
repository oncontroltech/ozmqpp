// OZMQPP headers
#include <ozmqpp/Context.hh>
#include <ozmqpp/Connection.hh>
#include <ozmqpp/Poll.hh>

// STD headers
#include <thread>

constexpr const char SERVER_INPROC_ADDRESS[] = "inproc://test_ozmqpp";

constexpr const char CLOSE_INPROC_ADDRESS[] = "inproc://close_ozmqpp";

constexpr const char CLOSE_STRING[] = "goodbye world";

constexpr const char TEST_STRING1[] = "hello world";

constexpr const char TEST_STRING2[] = "yet another test";

int g_error_code = 0;

class ClientThread
{
public:
    ClientThread(OZMQPP::Connection& client_server_connection,
                 OZMQPP::Connection& client_close_connection) :
        m_client_server_connection(client_server_connection),
        m_client_close_connection(client_close_connection)
    {

    }

    void operator()()
    {
        OZMQPP::Message send_message;
        send_message.AppendFrame(OZMQPP::Frame(TEST_STRING1));
        send_message.AppendFrame(OZMQPP::Frame(TEST_STRING2));
        m_client_server_connection.SendMessage(send_message);

        OZMQPP::Message recv_msg = m_client_server_connection.ReceiveMessage();

        std::string test_string2(TEST_STRING2);
        std::vector<int8_t> text_vector(test_string2.begin(), test_string2.end());
        if (recv_msg.GetFrame(0).GetMessageString() != TEST_STRING1)
        {
            g_error_code = 1;
        }
        else if (recv_msg.GetFrame(1).GetFrameData() != text_vector)
        {
            g_error_code = 2;
        }

        OZMQPP::Message end_message;
        end_message.AppendFrame(OZMQPP::Frame(CLOSE_STRING));
        m_client_close_connection.SendMessage(end_message);
    }

protected:

private:
    OZMQPP::Connection& m_client_server_connection;

    OZMQPP::Connection& m_client_close_connection;
};

class ServerThread
{
public:
    ServerThread(OZMQPP::RouterConnection& server_client_connection,
                 OZMQPP::Connection& server_close_connection) :
        m_server_client_connection(server_client_connection),
        m_server_close_connection(server_close_connection)
    {

    }

    void operator()()
    {
        OZMQPP::Poll server_pool;
        server_pool.AddConnection(0, m_server_close_connection);
        server_pool.AddConnection(1, m_server_client_connection);

        bool terminate_cicle = false;
        do
        {
            int active_connection_id = server_pool.Wait();
            switch (active_connection_id)
            {
            case 0:
                terminate_cicle = true;

                break;
            case 1:
                OZMQPP::RouterMessage recv_msg =
                    m_server_client_connection.ReceiveRouterMessage();
                OZMQPP::RouterMessage send_msg =
                    recv_msg.DerivateAnswerMessage();
                send_msg.AppendFrame(recv_msg.GetFrame(1));
                send_msg.AppendFrame(recv_msg.GetFrame(2));
                m_server_client_connection.SendMessage(send_msg);
                break;
            }

        } while (terminate_cicle == false);
    }

protected:

private:
    OZMQPP::RouterConnection& m_server_client_connection;

    OZMQPP::Connection& m_server_close_connection;
};

int
main()
{
    OZMQPP::Context client_context;

    OZMQPP::Connection& server_close_connection =
        client_context.CreateConnection(OZMQPP::SocketType::PAIR);
    server_close_connection.Bind(CLOSE_INPROC_ADDRESS);

    OZMQPP::RouterConnection& server_client_connection =
        client_context.CreateRouterConnection();
    server_client_connection.Bind(SERVER_INPROC_ADDRESS);

    ServerThread server(server_client_connection, server_close_connection);

    std::thread server_thread(server);

    OZMQPP::Connection& client_server_connection =
        client_context.CreateConnection(OZMQPP::SocketType::DEALER);
    client_server_connection.Connect(SERVER_INPROC_ADDRESS);

    OZMQPP::Connection& client_close_connection =
        client_context.CreateConnection(OZMQPP::SocketType::PAIR);
    client_close_connection.Connect(CLOSE_INPROC_ADDRESS);

    ClientThread client(client_server_connection, client_close_connection);

    std::thread client_thread(client);

    server_thread.join();

    client_thread.join();

    return g_error_code;
}
