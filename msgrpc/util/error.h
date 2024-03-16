#pragma once

#include <string>

namespace msgrpc::error
{
    enum error_code
    {
        cannot_parse_response = 0x5000,
        invalid_call_handler,
        result_not_available, 
    };

    class err
    {
    public:
        err(error_code _code, const std::string& what)
            : m_code(_code),
              m_message(what)
        {

        }

        [[nodiscard]] const error_code code() const
        {
            return m_code;
        }

        [[nodiscard]] const std::string& what() const
        {
            return m_message;
        }

        template<typename T>
        void pack(T& packer)
        {
            packer(m_code, m_message);
        }
    private:
        enum error_code m_code;
        std::string m_message;
    };
}
