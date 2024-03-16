#pragma once

#include "defines.h"
#include "util/error.h"
#include <string>
#include <vector>

namespace msgrpc::core::messages {

    // this isn't an enum class
    // because we want to serialze the MessageType
    enum MessageType: std::size_t
    {
        Request,
        Response,
        Error,
    };

    struct Message
    {
        std::string name;
        call_id id;
        std::size_t type;
        std::vector<unsigned char> data;

        template<typename T>
        void pack(T& packer)
        {
            packer(name, id, type, data);
        }
    };
}
