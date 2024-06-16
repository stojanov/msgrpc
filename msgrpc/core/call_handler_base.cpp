#include "call_handler_base.h"

namespace msgrpc::core {

    bool call_handler_base::handle_received_payload(const messages::Message& message)
    {
       return handle_received_payload_typed(message); 
    }

}
