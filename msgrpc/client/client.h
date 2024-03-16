#pragma once

#include "core/call_handler_base.h"
#include "core/call_resolver.h"
#include "core/defines.h"
#include "core/call_handler.h"
#include "transport.h"
#include <atomic>
#include <memory>
#include <unordered_map>
#include "core/common.h"
#include "util/error.h"
namespace msgrpc::client
{

class client
{

public:
    template<typename param_type, typename rtn_type>
    using call_resolver_expected = util::expected<std::shared_ptr<core::call_resolver<param_type, rtn_type>>, error::err>;

    template<typename param_type, typename rtn_type>
    using call_func_resolover = std::function<call_resolver_expected<param_type, rtn_type>(param_type&)>;

    client(std::shared_ptr<transport> _transport);
    
    template<typename param_type, typename rtn_type>
    call_func_resolover<param_type, rtn_type>  register_function_call(const std::string& name,
                                                                      core::external_call_callback<param_type, rtn_type> server_call,
                                                                      core::listener_callback<rtn_type> listener = nullptr)
    {
        auto call = std::make_shared<core::call_handler<param_type, rtn_type>>(name, server_call);

        m_calls[name] = call;

        call->handle_send_payload.connect([this](std::vector<unsigned char> payload)
        {
            handle_send_data(payload);
        });

        return [call_handler_ptr = std::weak_ptr<core::call_handler<param_type, rtn_type>>(call)]
        (param_type& parameter)
        {
            return client::call_function<param_type, rtn_type>(call_handler_ptr, parameter);
        };
    }

 private:
    template<typename param_type, typename rtn_type>
    static call_resolver_expected<param_type, rtn_type> call_function(std::weak_ptr<core::call_handler<param_type, rtn_type>> call_handler_ptr, param_type& parameter)
    {
        auto _call_handler = call_handler_ptr.lock();
        
        if (!_call_handler)
        {
            return error::err(error::invalid_call_handler, "Invalid call handler, must have been destroyed!");
        }

        call_id id = _call_handler->send_call(parameter);

        auto call_resolver = std::make_shared<core::call_resolver<param_type, rtn_type>>(id, _call_handler);
        
        return call_resolver;
    }


    // no need to be coping the vector here
    // TODO: make this get a const ref
    void handle_send_data(std::vector<unsigned char> data);
    
    void handle_received_data(std::vector<unsigned char> data);

    std::atomic<call_id> m_id_counter;
    std::shared_ptr<transport> m_transport;
    std::unordered_map<std::string, std::shared_ptr<core::call_handler_base>> m_calls;
};

}
