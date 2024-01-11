#pragma once

#include "core/call_handler_base.h"
#include "core/call_resolver.h"
#include "core/defines.h"
#include "core/call_handler.h"
#include "transport.h"
#include <atomic>
#include <memory>
#include <unordered_map>

namespace msgrpc::client
{

class client
{
    template<typename param_type, typename rtn_type>
    using call_resolver_expected = util::expected<std::shared_ptr<core::call_resolver<param_type, rtn_type>>, int>;
public:
    client(std::shared_ptr<transport> _transport);
    
    template<typename param_type, typename rtn_type>
    std::function<call_resolver_expected<rtn_type, param_type>(param_type&)> register_function_call(const std::string& name, std::function<rtn_type(param_type&)> server_call) 
    {
        auto call = std::make_shared<core::call_handler<param_type, rtn_type>>(name, server_call);

        m_calls[name] = call;

        call->handle_send_data.connect([this](std::vector<unsigned char> payload)
        {
            handle_send_data(payload);
        });

        return [call_handler_ptr = std::weak_ptr<core::call_handler<param_type, rtn_type>>(call)]
        (const param_type& parameter)
        {
            return client::call_function(call_handler_ptr, parameter);
        };
    }

 private:
    template<typename param_type, typename rtn_type>
    static call_resolver_expected<param_type, rtn_type> call_function(std::weak_ptr<core::call_handler<param_type, rtn_type>> call_handler_ptr, param_type& parameter)
    {
        auto _call_handler = call_handler_ptr.lock();
        
        if (!_call_handler)
        {
            return -1;
        }

        std::string id = _call_handler->send_call(parameter);

        auto call_resolver = std::make_shared<core::call_resolver<param_type, rtn_type>>(id, _call_handler);
        
        return call_resolver;
    }

    void handle_send_data(std::vector<unsigned char> data)
    {
    
    }
    
    void handle_received_data(std::vector<unsigned char> data)
    {}

    std::atomic<call_id> m_id_counter;
    std::shared_ptr<transport> m_transport;
    std::unordered_map<std::string, std::shared_ptr<core::call_handler_base>> m_calls;
};

}
