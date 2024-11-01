#pragma once

#include <core/call_handler.h>
#include <core/call_handler_base.h>
#include <core/call_resolver.h>
#include <core/common.h>
#include <core/defines.h>
#include <system/async_pool.h>
#include <transport/transport.h>
#include <util/error.h>

#include <atomic>
#include <memory>
#include <unordered_map>

namespace msgrpc::client {
class client {
   private:
    template <typename param_type, typename rtn_type>
    using call_resolver_expected = util::expected<
        std::shared_ptr<core::call_resolver<param_type, rtn_type>>, error::err>;

    template <typename param_type, typename rtn_type>
    using call_func_resolover =
        std::function<call_resolver_expected<param_type, rtn_type>(
            param_type&)>;

   public:
    // add support for async_transport
    // also if normal transport is provided provide async functionalities
    // create a wrapper around it to make it async
    client(std::shared_ptr<transport> _transport);

    /* This is the main function to attach a function call to the rpc
     * if there is a listener attached then the data won't be available
     * in the provided methods when calling the function that's returned
     * from this method, might be changed in the future
     */
    template <typename param_type, typename rtn_type>
    call_func_resolover<param_type, rtn_type> register_function_call(
        const std::string& name,
        external_call_callback<param_type, rtn_type> server_call = nullptr,
        listener_callback<rtn_type> listener = nullptr) {
        // create the shared_ptr with new so it can be deallocated when ref
        // count goes to zero not also when the weak_ptr goes out of scope
        auto call = std::shared_ptr<core::call_handler<param_type, rtn_type>>(
            new core::call_handler<param_type, rtn_type>(
                name, std::move(server_call), std::move(listener)));

        // we override if there is already a call handler attach, shouldn't
        // change muc*h
        m_calls[name] = call;

        call->send_payload.connect([this](std::vector<unsigned char> payload,
                                          call_id id,
                                          core::call_handler_base& handler) {
            return send_data(std::move(payload), id, handler);
        });

        call->generate_call_id.connect([this]() { return create_call_id(); });

        return [call_handler_ptr =
                    std::weak_ptr<core::call_handler<param_type, rtn_type>>(
                        call)](param_type& parameter)
                   -> call_resolver_expected<param_type, rtn_type> {
            return client::call_function<param_type, rtn_type>(call_handler_ptr,
                                                               parameter);
        };
    }

   private:
    // no need to be coping the vector here
    // TODO: make this get a const ref or maybe a span
    std::optional<error::err> send_data(std::vector<unsigned char>&& data,
                                        call_id id,
                                        core::call_handler_base& handler);

    void on_received_data(std::vector<unsigned char> data);

    call_id create_call_id() { return m_id_counter.fetch_add(1); }

    template <typename param_type, typename rtn_type>
    static call_resolver_expected<param_type, rtn_type> call_function(
        std::weak_ptr<core::call_handler<param_type, rtn_type>>
            call_handler_ptr,
        param_type& parameter) {
        auto _call_handler = call_handler_ptr.lock();

        if (!_call_handler) {
            return error::err(
                error::invalid_call_handler,
                "Invalid call handler, must have been destroyed!");
        }

        auto id = _call_handler->send_call(parameter);

        auto call_resolver =
            std::make_shared<core::call_resolver<param_type, rtn_type>>(
                id, _call_handler);

        return call_resolver;
    }

   private:
    system::async_pool m_async_pool;
    std::atomic<call_id> m_id_counter;
    std::shared_ptr<transport> m_transport;
    std::unordered_map<std::string, std::shared_ptr<core::call_handler_base>>
        m_calls;
};

}  // namespace msgrpc::client
