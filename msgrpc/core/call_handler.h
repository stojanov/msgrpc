#pragma once

#include <string>
#include "core/call_handler_base.h"
#include "defines.h"
#include "util/signal.h"
#include "message.h"
#include <atomic>

namespace msgrpc::core 
{

template<typename param_type, typename rtn_type>
class call_handler: public call_handler_base 
{
public:
    call_handler(const std::string& name, std::function<rtn_type(param_type&)>&& server_call)
    :   m_name(name),
        m_server_call(server_call)
    {

    }

    call_id send_call(const param_type& parameter)
    {
        // serialize the data then send it, do error checking
        std::vector<unsigned char> data = msgpack::pack(parameter);
        
        call_id id = m_local_id++;

        messages::Request request {
            .name = m_name,
            .id = id,
            .data = data
        };

        handle_send_payload(data);
        
        return id; 
    }

private:
    bool handle_received_payload_typed(std::vector<unsigned char>& data) override 
    {
        // deserialize the data then send it, do error checking
        std::error_code ec;
        rtn_type object = msgpack::pack<rtn_type>(data, ec);

        if (ec)
        {
            return false;
        }
    }

    util::signal<rtn_type> handle_received_message;
    std::atomic<call_id> m_local_id;
    std::function<rtn_type(param_type&)> m_server_call;
    std::string m_name;
};
    

}
