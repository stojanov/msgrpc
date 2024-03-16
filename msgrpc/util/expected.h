#pragma once

#include <type_traits>
#include <utility>
#include <variant>
#include <stdexcept>

namespace msgrpc::util {

template<typename value_type, typename error_type>
class expected 
{
public:
    template<typename ...Args,
        typename = std::enable_if_t<std::is_constructible_v<value_type, Args...>>>
    expected(Args&& ...args)
        : m_variant(std::in_place_index<0>, std::forward<Args>(args)...)
    {

    }

    expected(const error_type& error)
        : m_variant(error)
    {
    }

    expected(error_type&& error)
        : m_variant(std::move(error))
    {
    }

    expected(const expected& other) = default;
    expected(expected&& other) = default;

    expected& operator=(const expected& other) = default;
    expected& operator=(expected&& other) = default;

    bool has_value()
    {
        return std::holds_alternative<value_type>(m_variant);
    }

    value_type value()
    {
        if (!has_value())
        {
            throw std::runtime_error("Expected isn't holding value");
        }
        return std::get<value_type>(m_variant);
    }

    error_type error()
    {
        if (has_value()) 
        {
            throw std::runtime_error("Expected isn't holding an error type");
        }
        return std::get<error_type>(m_variant);
    }

    value_type* operator->()
    {
        return &std::get<value_type>(m_variant);
    }

    value_type& operator*()
    {
        return std::get<value_type>(m_variant);
    }

    constexpr explicit operator bool()
    {
        return has_value();
    }

private:
    std::variant<value_type, error_type> m_variant;
};

}
