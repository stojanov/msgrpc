#pragma once

#include <core/call_handler_base.h>
#include <core/common.h>
#include <core/defines.h>
#include <core/message.h>
#include <util/defer.h>
#include <util/error.h>
#include <util/expected.h>
#include <util/signal.h>

#include <condition_variable>
#include <mutex>
#include <string>
#include <system_error>

namespace msgrpc::core {
template <typename param_type, typename rtn_type>
class call_handler : public call_handler_base {
   public:
    call_handler(
        const std::string& name,
        external_call_callback<param_type, rtn_type> external_call = nullptr,
        listener_callback<rtn_type> listener = nullptr)
        : m_name(name), m_external_call(external_call), m_listener(listener) {}

    call_id send_call(param_type& parameter) {
        call_id id = generate_call_id();

        data_buffer payload =
            messages::create_and_pack_request(m_name, id, parameter);

        send_payload(payload, id, std::ref(*this));

        return id;
    }

    template <typename rep, typename period>
    msg_result<rtn_type> wait_for(call_id id,
                                  std::chrono::duration<rep, period> duration) {
        std::unique_lock lck(m_result_mutex);

        bool found_result = m_result_signal.wait_for(lck, duration, [&]() {
            return m_call_results.find(id) != m_call_results.end();
        });

        if (!found_result) {
            return error::err(
                error::result_not_available,
                "Cannot find result, must have not yet resolved/finished, or "
                "you have already fetched the result");
        }

        DEFER([&]() { clean_map_for_message(id); });

        return m_call_results[id];
    }

    msg_result<rtn_type> query_result(call_id id) {
        std::lock_guard lck(m_result_mutex);

        if (auto i = m_call_results.find(id); i != m_call_results.end()) {
            DEFER([&]() { clean_map_for_message(id); });

            return i->second;
        } else {
            return error::err(
                error::result_not_available,
                "Cannot find result, must have not yet resolved/finished, or "
                "you have already fetched the result");
        }
    }

   private:
    void clean_map_for_message(call_id id) {
        std::lock_guard lck(m_result_mutex);

        // this can possibly be skipped
        if (const auto i = m_call_results.find(id);
            i != std::end(m_call_results)) {
            m_call_results.erase(i);
        }
    }

    void fail_call(call_id id, const error::err& err) override {
        {
            std::lock_guard lck(m_result_mutex);
            m_call_results.insert(std::make_pair(id, err));
        }
        m_result_signal.notify_all();
    }

   private:
    bool on_receive_payload(const messages::Message& message) override {
        switch (message.type) {
            case messages::Request: {
                handle_external_call(message);
                break;
            }
            case messages::Response: {
                handle_call_response(message);
                break;
            }
            case messages::Error: {
                // TODO
                break;
            }
            default:
                // do nothing for now
                break;
        }

        // don't know what to do with this for now
        return true;
    }

    void notify_about_result(call_id id, auto& result) {
        {
            std::unique_lock<std::recursive_mutex> lck(m_result_mutex);
            m_call_results[id] = result;
        }

        m_result_signal.notify_all();

        if (m_listener) {
            m_listener(result);
        }
    };

    void handle_call_response(const messages::Message& message) {
        std::error_code ec;
        auto rtn = msgpack::unpack<rtn_type>(message.data, ec);

        if (ec) {
            // auto err = error::err::create(error::cannot_parse_response,
            //         "Invalid response payload, cannot parse response; {}",
            //         ec.message());
            // resolve_query(message.id, err);
            return;
        }

        notify_about_result(message.id, rtn);
    }

    void handle_external_call(const messages::Message& message) {
        std::error_code ec;

        auto parameter = msgpack::unpack<param_type>(message.data, ec);

        if (ec) {
            // TODO: handle the error here
            // send back the error
            return;
        }

        // try here in case external_call throws
        try {
            if (!m_external_call) {
                // there is not an external_call callback attached
                return;
            }

            auto result = m_external_call(parameter);

            // possible improvement create a class to handle the serialization
            auto response_serialized =
                messages::create_and_pack_response(m_name, message.id, result);

            // send the result/response to the caller

        } catch (...) {
            messages::Message error_response{

            };
        }
    }

    std::string m_name;

    external_call_callback<param_type, rtn_type> m_external_call;
    listener_callback<rtn_type> m_listener;

    std::atomic<std::uint64_t> m_call_id_counter;
    std::recursive_mutex m_result_mutex;
    std::condition_variable m_result_signal;
    std::unordered_map<call_id, util::expected<rtn_type, error::err>>
        m_call_results;
};

}  // namespace msgrpc::core
