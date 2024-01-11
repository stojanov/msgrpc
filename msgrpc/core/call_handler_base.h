#pragma once

#include "message.h"
#include "defines.h"
#include "util/signal.h"
#include "util/expected.h"
#include <functional>
#include <iomanip>
#include <vector>


namespace msgrpc::core {

class call_handler_base {
public:
    call_handler_base() = default;
    virtual ~call_handler_base() = default;
    
    bool handle_received_payload(const messages::Message& message); 
    virtual void call_failed(call_id id) = 0;    
    util::signal<std::vector<unsigned char>> handle_send_payload; 
protected:
    // why do we have this function here, it's becase we want to have a call_handler
    // where we have template parameters and we have to forward the call made from this interface 
    // into the typed version of this call_handler
    virtual bool handle_received_payload_typed(const messages::Message& message) = 0;
};

}
