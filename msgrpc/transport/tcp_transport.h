#pragma once

#include <core/common.h>
#include <transport/transport.h>

#include <asio.hpp>
#include <asio/io_context.hpp>
#include <thread>

namespace msgrpc {

// minimal example
class p2p_tcp_transport : public transport {
   public:
    p2p_tcp_transport(int port);

    void connect(const std::string& address, const std::string& port);
    std::optional<error::err> send(data_buffer data);
    msg_result<data_buffer> receive(std::chrono::milliseconds timeout);

   private:
    void other_work(int port);

   private:
    asio::io_context m_ctx;
    asio::ip::tcp::socket my_session;

    std::thread m_thread;
};

}  // namespace msgrpc
