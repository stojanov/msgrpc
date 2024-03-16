#pragma once

#include <type_traits>
#include <vector>
#include <functional>

namespace msgrpc::util
{
template<typename ...Args>
class signal
{
    using func = std::function<void(Args...)>;
public:
    void connect(func&& callback)
    {
        m_funcs.push_back(std::move(callback));
    }
    
    void operator()(Args&... args)
    {
        for (const auto& func : m_funcs)
        {
             func(std::forward<Args>(args)...);
        }
    }

    void operator()(Args&&... args)
    {
        for (const auto& func : m_funcs)
        {
             func(std::forward<Args>(args)...);
        }
    }
private:
   std::vector<func> m_funcs; 
};

}
