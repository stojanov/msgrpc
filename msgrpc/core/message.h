#pragma once

#include "core/common.h"
#include "defines.h"
#include "util/error.h"
#include <string>
#include <vector>

#include <msgpack/msgpack.hpp>

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
        data_buffer data;

        template<typename T>
        void pack(T& packer)
        {
            packer(name, id, type, data);
        }
    };

    // ahhh this is kinda stupid, but well, it's simple
    // maybe create a function that takes in MessageType and keep it generic 

    template<typename T>
    static inline data_buffer create_and_pack_request(const std::string& name, call_id id, T& data_to_pack)
    {
        data_buffer packed_data = msgpack::pack(data_to_pack);

        auto msg = Message {
            .name = name,
            .id = id, 
            .type = MessageType::Request,
            .data = msgpack::pack(data_to_pack)
        };

        return msgpack::pack(msg);
    }

    template<typename T>
    static inline data_buffer create_and_pack_response(const std::string& name, call_id id, T& data_to_pack)
    {
        data_buffer packed_data = msgpack::pack(data_to_pack);

        auto msg = Message {
            .name = name,
            .id = id,
            .type = MessageType::Response,
            .data = msgpack::pack(data_to_pack)
        };

        return msgpack::pack(msg);
    }

    static inline data_buffer create_and_pack_error(const std::string& name, call_id id, error::err& data_to_pack)
    {
        data_buffer packed_data = msgpack::pack(data_to_pack);

        auto msg = Message {
            .name = name,
            .id = id,
            .type = MessageType::Error,
            .data = msgpack::pack(data_to_pack)
        };

        return msgpack::pack(msg);
    }
}
