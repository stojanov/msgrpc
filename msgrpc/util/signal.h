#pragma once

#include <type_traits>
#include <vector>
#include <functional>

namespace msgrpc::util
{

    template<typename T>
    class single_signal
    {
        using func = std::function<T>;
    public:
        ~single_signal()
        {
            disconnect_slot();
        }

        void disconnect_slot()
        {
            m_func = nullptr;
        }

        void connect(func&& callback)
        {
            m_func = std::move(callback);
        }
        
        auto operator()()
        {
            return m_func();
        }

        template<typename ...Args>
        auto operator()(Args&... args)
        {
            return m_func(std::forward<Args>(args)...);
        }

        template<typename ...Args>
        auto operator()(Args&&... args)
        {
            return m_func(std::forward<Args>(args)...);
        }
    private:
        func m_func;
    };

}
