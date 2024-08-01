#pragma once

#include <util/expected.h>
#include <util/error.h>

#include <functional>
#include <vector>

namespace msgrpc
{
    template<typename result_type>
    using msg_result = util::expected<result_type, error::err>;

    template<typename param_type, typename rtn_type>
    using external_call_callback = std::function<rtn_type(param_type&)>;

    template<typename rtn_type>
    using listener_callback = std::function<void(msg_result<rtn_type>)>;

    using data_buffer = std::vector<unsigned char>;
}
