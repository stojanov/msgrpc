#include "client.h"
#include <system_error>
#include "core/message.h"
#include "msgpack/msgpack.hpp"


namespace msgrpc::client {

client::client(std::shared_ptr<transport> _transport)
    :   m_transport(_transport)
{
    m_transport->received.connect([this](std::vector<unsigned char> data)
    {
        handle_received_data(data);
    });
}

// TODO: make this return an err
// handle it in the call_handler 
void client::handle_send_data(std::vector<unsigned char> data)
{
    m_transport->send(data);
}

void client::handle_received_data(std::vector<unsigned char> data)
{
    std::error_code ec;

    auto message = msgpack::unpack<core::messages::Message>(data);

    if (ec)
    {
        // only temporary, handle the error
        return;
    }

    if (auto i = m_calls.find(message.name); i != m_calls.end())
    {
        i->second->handle_received_payload(message);
    }
    else
    {
        // TODO: handle the error
    }
}

}
