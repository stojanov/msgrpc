#include <transport/tcp_transport.h>

#include <chrono>

#include "core/common.h"

namespace msgrpc {

using asio::ip::tcp;

p2p_tcp_transport::p2p_tcp_transport(int port) : my_session(m_ctx) {
    m_thread = std::thread([&] { other_work(port); });
}

std::optional<error::err> p2p_tcp_transport::send(data_buffer data) {}

msg_result<data_buffer> p2p_tcp_transport::receive(
    std::chrono::milliseconds timeout) {}

void p2p_tcp_transport::other_work(int port) {
    tcp::resolver resolver(m_ctx);
    tcp::acceptor a(m_ctx, tcp::endpoint(tcp::v4(), port));
    auto s = a.accept();
}

}  // namespace msgrpc
