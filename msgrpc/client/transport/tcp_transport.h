#include "core/common.h"
#include "transport.h"

#include <asio.hpp>

namespace msgrpc::client
{

    // TODO: implement packets, with middleware
    // NOT FINISHED! 
    class tcp_transport: public transport 
    {
    public:
        tcp_transport();
        ~tcp_transport() = default;

        // this should be used to send the data on the the socket in an async way
        // as to not block the thread calling this
        // void async_send(const std::vector<unsigned char>&& data, const transport::async_send_callback&& callback) override;tcp-tra

        std::optional<error::err> send(core::data_buffer& data) override;

        // connect should be called after calling start
        // maybe we should start the thread when constructing
        std::optional<error::err> connect(const std::string_view ip, const std::string_view port);
        void disconnect();

        std::optional<error::err> start();
        void stop();

    private:
        void handle_read();
    private:
        // even though we are going to be doing async reading
        // the reading will only be done sequentially on a different thread
        // thus meaning we can use a single buffer
        std::vector<unsigned char> m_read_buffer;
        
        std::atomic_bool m_is_running;
        std::thread m_thread;

        asio::io_context m_ctx;
        asio::ip::tcp::socket m_socket;
    };

}
