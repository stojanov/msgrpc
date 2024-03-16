#pragma once

#include <condition_variable>
#include <locale>
#include <mutex>
#include <string>
#include <core/call_handler_base.h>
#include "defines.h"
#include "util/expected.h"
#include "util/signal.h"
#include "message.h"
#include <atomic>
#include <system_error>
#include <util/error.h>
#include <msgpack/msgpack.hpp>
#include "common.h"

namespace msgrpc::core 
{

template<typename param_type, typename rtn_type>
class call_handler: public call_handler_base 
{
public:
    call_handler(const std::string& name, external_call_callback<param_type, rtn_type> external_call, listener_callback<rtn_type> listener = nullptr)
    :   m_name(name),
        m_external_call(external_call),
        m_listener(listener)
    {

    }

    call_id send_call(param_type& parameter)
    {
        // serialize the data then send it, do error checking
        // if this fails, some nasty template erros will occur
        // at least it will be on compile time
        std::vector<unsigned char> data;// = msgpack::pack(parameter);

        call_id id = m_local_id.fetch_add(1);

        messages::Message request {
            .name = m_name,
            .id = id,
            .type = messages::MessageType::Request,
            .data = data
        };

        std::vector<unsigned char> payload = msgpack::pack(request);
        handle_send_payload(payload);
        
        return id; 
    }
    // TODO: send the error here
    void call_failed(call_id) override
    {

    }

    template<typename rep, typename period>
    call_result<rtn_type> wait_for(call_id id, std::chrono::duration<rep, period> duration)
    {
        std::unique_lock lck(m_result_mutex);

        bool found_result = m_result_signal.wait_for(lck, duration, [&]()
                                                     {
                                                        return m_call_results.find(id) != m_call_results.end();
                                                     });

        if (!found_result)
        {
            return error::err(error::result_not_available, "Cannot find result, must have not yet resolved/finished");
        }

        return m_call_results[id];
    }

    call_result<rtn_type> query_result(call_id id)
    {
        std::unique_lock lck(m_result_mutex);

        if (auto i = m_call_results.find(id); i != m_call_results.end())
        {
            return i->second;
        }
        else
        {
            return error::err(error::result_not_available, "Cannot find result, must have not yet resolved/finished");
        }
    }
private:
    bool handle_received_payload_typed(const messages::Message& message) override 
    {
        // TODO: Distingush between message, request, response, external_call
        switch (message.type)
        {
            case messages::Request:
                {
                    handle_external_call(message);
                    break;
                }
            case messages::Response:
                {
                    handle_call_response(message);
                    break;
                }
            case messages::Error:
                {
                    break;
                }
            default:
                // do nothing for now
                break;
        }

        // don't know what to do with this for now
        return true;
    }
    
    void handle_call_response(const messages::Message& message)
    {
        auto resolve_query = [this](call_id id, auto& result)
            {
                {
                    std::unique_lock lck(m_result_mutex);
                    m_call_results[id] = result;
                }

                m_result_signal.notify_all();

                if (m_listener)
                {
                    m_listener(result);
                }
            };

        std::error_code ec;
        auto rtn = msgpack::unpack<rtn_type>(message.data, ec);

        if (ec)
        {
            error::err err(error::cannot_parse_response, "Invalid response payload, cannot parse response");
            resolve_query(message.id, err);
            return;
        }

        resolve_query(message.id, rtn);
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
            auto result = m_external_call(parameter);

            auto data = msgpack::pack(result);

            messages::Message response {
                .name = message.name,
                .id = message.id,
                .type = messages::MessageType::Response,
                .data = data 
            };

            auto response_serialized = msgpack::pack(response);

            handle_send_payload(response_serialized);
        }
        catch (...)
        {
            messages::Message error_response {

            };
        }
    }

    std::string m_name;
    util::signal<rtn_type> handle_received_message;
    std::atomic<call_id> m_local_id;
    external_call_callback<param_type, rtn_type> m_external_call;
    listener_callback<rtn_type> m_listener;

    std::mutex m_result_mutex;
    std::condition_variable m_result_signal;
    std::unordered_map<call_id, util::expected<rtn_type, error::err>> m_call_results;
};
    

}
