#pragma once

#include <condition_variable>
#include <mutex>
#include <string>
#include "core/call_handler_base.h"
#include "defines.h"
#include "util/expected.h"
#include "util/signal.h"
#include "message.h"
#include <atomic>
#include <system_error>

namespace msgrpc::core 
{

template<typename param_type, typename rtn_type>
class call_handler: public call_handler_base 
{
public:
    call_handler(const std::string& name, std::function<rtn_type(param_type&)>&& external_call)
    :   m_name(name),
        m_external_call(external_call)
    {

    }

    call_id send_call(const param_type& parameter)
    {
        // serialize the data then send it, do error checking
        std::vector<unsigned char> data = msgpack::pack(parameter);
        
        call_id id = m_local_id++;

        messages::Message request {
            .name = m_name,
            .id = id,
            .data = data,
            .isRequest = true
        };

        handle_send_payload(data);
        
        return id; 
    }
    // TODO: send the error here
    void call_failed(call_id) override
    {

    }

    template<typename rep, typename period>
    util::expected<rtn_type, int> wait_for(call_id id, std::chrono::duration<rep, period> duration)
    {
        std::unique_lock lck(m_result_mutex);

        bool found_result = m_result_signal.wait_for(lck, duration, [&]()
                                                     {
                                                        return m_call_results.find(id) != m_call_results.end();
                                                     });

        if (!found_result)
        {
            return -1;
        }
    }

    util::expected<rtn_type, int> query_result(call_id id)
    {
        std::unique_lock lck(m_result_mutex);
    }
private:
    bool handle_received_payload_typed(const messages::Message& message) override 
    {
        if (message.isRequest)
        {
            handle_external_call(message);
        }
        else 
        {
            handle_call_response(message);
        }
        return false;
    }
    
    void handle_call_response(const messages::Message& message)
    {
    }

    void handle_external_call(const messages::Message& message)
    {
        std::error_code ec;
        
        auto parameter = msgpack::unpack<param_type>(message.data, ec);

        if (ec)
        {
            // TODO: handle the error here
            return;
        }
        // try here in case external_call throws
        try 
        {
            auto result = msgpack::pack(m_external_call(parameter));
            
            messages::Message response {
                .id = message.id,
                .data = result,
                .isRequest = false,
                .name = message.name
            };

            handle_send_payload(response);
        }
        catch (...)
        {
            
        }
    }

    std::string m_name;
    util::signal<rtn_type> handle_received_message;
    std::atomic<call_id> m_local_id;
    std::function<rtn_type(param_type&)> m_external_call;

    std::mutex m_result_mutex;
    std::condition_variable m_result_signal;
    std::unordered_map<call_id, rtn_type> m_call_results;
};
    

}
