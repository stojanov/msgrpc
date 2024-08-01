#pragma once

#include <core/common.h>
#include <util/signal.h>
#include <util/error.h>

#include <chrono>
#include <optional>

namespace msgrpc
{
    class transport
    {
    public:
        transport() = default;
        virtual ~transport() = default;

        virtual std::optional<error::err> send(data_buffer& data) = 0; 

        // this has to be able to timeout if no data was present, will be used for polling the data
        virtual msg_result<data_buffer> receive(std::chrono::milliseconds timeout) = 0;
    };
}
