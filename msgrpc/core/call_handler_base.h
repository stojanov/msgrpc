#pragma once

#include <core/common.h>
#include <core/message.h>
#include <core/defines.h>

#include <util/signal.h>

#include <optional>

namespace msgrpc::core {

    class call_handler_base {
    public:
        call_handler_base() = default;
        virtual ~call_handler_base() = default;
        
        // virtual void call_failed(call_id id, std::optional<error::err> = std::nullopt) = 0; 
        virtual bool on_receive_payload(const messages::Message& message) = 0;

        util::single_signal<std::optional<error::err>(data_buffer)> send_payload;
        util::single_signal<call_id()> generate_call_id;
    };

}
