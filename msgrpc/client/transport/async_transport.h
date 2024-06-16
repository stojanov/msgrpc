#pragma once


#include "core/common.h"
#include "util/signal.h"
namespace msgrpc::client
{
    class async_transport
    {
    public:
        using async_send_callback = std::function<void(std::optional<error::err>)>;
    public:
        async_transport() = default; 
        ~async_transport() = default;

        virtual void async_send(const core::data_buffer&& data, const async_send_callback&& callback) = 0;

        // util::single_signal<void, core::data_buffer> received;
    };
}
