#pragma once

#include <functional>

namespace msgrpc::util
{
    class defer
    {
        using callback_func = std::function<void()>;
    public:
        defer(callback_func&& callback)
            : m_callback(callback)
        {

        }

        ~defer()
        {
            m_callback();
        }
        
    private:
        callback_func m_callback;
    };

#define DEFER(func) msgrpc::util::defer __DeferedFunction_(func)
}
