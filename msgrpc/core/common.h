#pragma once

#include <functional>
#include <util/expected.h>
#include <util/error.h>

namespace msgrpc::core
{
    template<typename result_type>
    using call_result = util::expected<result_type, error::err>;

    template<typename param_type, typename rtn_type>
    // using external_call_callback = std::function<call_result<rtn_type>(param_type&)>;
    using external_call_callback = std::function<rtn_type(param_type&)>;

    template<typename rtn_type>
    using listener_callback = std::function<void(call_result<rtn_type>)>;
}
