#include <client/client.h>
#include <core/call_handler_base.h>
#include <core/message.h>

#include <system_error>

namespace msgrpc::client {

client::client(std::shared_ptr<transport> _transport)
    : m_transport(_transport), m_async_pool(std::chrono::milliseconds(1)) {}

void client::send_data(std::vector<unsigned char> data, call_id id,
                       core::call_handler_base& handler) {
    m_async_pool.submit([&] {
        auto err = m_transport->send(data);

        if (err) {
            handler.fail_call(id, *err);
        }
    });
}

void client::on_received_data(std::vector<unsigned char> data) {
    std::error_code ec;

    const auto message = msgpack::unpack<core::messages::Message>(data, ec);

    if (ec) {
        // send the error here
        // actually there is no way to send the error expect send a generic
        // error but we don't know which exact id failed
        return;
    }

    if (auto i = m_calls.find(message.name); i != m_calls.end()) {
        i->second->on_receive_payload(message);
    } else {
        // handler not attached for this name
        // TODO: handle the error
        // also send the error back to the caller
    }
}

}  // namespace msgrpc::client
