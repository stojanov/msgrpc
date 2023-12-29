#include "call_handler_base.h"

namespace msgrpc::core {

bool call_handler_base::handle_received_payload(std::vector<unsigned char>& data)
{
   return handle_received_payload_typed(data); 
}

}
