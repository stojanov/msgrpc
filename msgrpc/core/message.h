#pragma once

#include "msgpack/msgpack.hpp"
#include "defines.h"
#include <vector>

namespace msgrpc::core::messages {

struct Message 
{
    std::string name;
    call_id id;
    std::vector<unsigned char> data;
    bool isRequest;
};

}
