#pragma once

#include <optional>
#include <chrono>
#include "core/call_handler.h"
#include "defines.h"
#include "util/expected.h"

namespace msgrpc::core {
// int is just a temporary response for the error type
template<typename parameter_type, typename return_type>
class call_resolver
{
public:
    call_resolver(call_id id, std::shared_ptr<core::call_handler<parameter_type, return_type>> _call_handler)
    :   m_id(id),
        m_call_handler(_call_handler)
    {
        
    }
    
    // will wait for the call to resolve with a given timeout, will remove 
    template<typename T, typename F>
    util::expected<return_type, int> wait_for(std::chrono::duration<T, F> duration)
    {
        return m_call_handler->wait_for(m_id, duration);
    }
    // will give you an imediate result response
    util::expected<return_type, int> result()
    {
        return m_call_handler->query_result(m_id);
    }
private:
    call_id m_id;
    std::shared_ptr<core::call_handler<parameter_type, return_type>> m_call_handler;
};

}
