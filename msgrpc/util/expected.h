#pragma once

#include <variant>
#include <stdexcept>

namespace msgrpc::util {

template<typename value_type, typename error_type>
class expected 
{
public:
    expected(value_type& value)
    {
        m_variant = value;
    }

    expected(error_type& error)
    {
        m_variant = error;
    }

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
