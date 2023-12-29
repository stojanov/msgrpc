#include "client.h"


namespace msgrpc::client {

client::client(std::shared_ptr<transport> _transport)
    :   m_transport(_transport)
{
    m_transport->received.connect([this](std::vector<unsigned char> data)
    {
        handle_received_data(data);
    });
}

void client::handle_send_data(std::vector<unsigned char> data)
{

}

void client::handle_received_data(std::vector<unsigned char> data)
{
    
}

}
