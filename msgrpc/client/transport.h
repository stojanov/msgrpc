#pragma once

#include <vector>
#include <functional>
#include "util/signal.h"

namespace msgrpc::client
{

class transport
{
public:
    transport() = default;
    virtual ~transport() = default;

    virtual void send(std::vector<unsigned char>) = 0; 
    util::signal<std::vector<unsigned char>> received;
};

}
