#pragma once

#include <vector>
#include <optional>
#include "core/common.h"
#include "util/signal.h"
#include "util/error.h"

namespace msgrpc::client
{
    class transport
    {
    public:
        
    public:
        transport() = default;
        virtual ~transport() = default;

        // sync version for sending
        virtual std::optional<error::err> send(core::data_buffer& data) = 0; 
        util::single_signal<void(core::data_buffer)> received;
    };

}
