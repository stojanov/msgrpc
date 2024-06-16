#pragma once

#include <string>
#include <cstdint>
#include <format>

namespace msgrpc::error
{

    enum error_code: std::uint32_t
    {
        cannot_parse_response = 0x5000,
        invalid_call_handler,
        result_not_available, 
        not_connected,
        thread_already_running,
        cannot_connect,
        cannot_find_address,
        cannot_send,
        send_error, // yeah i know, pretty general
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
            return static_cast<error_code>(m_code);
        }

        [[nodiscard]] const std::string& what() const
        {
            return m_message;
        }

        // template<typename ...Args>
        // static err create(error_code code, const std::string& format_string, Args&& ...args)
        // {
        //     auto message = std::format(format_string, std::forward<Args>(args)...);
        //     return err(code, message);
        // }

        template<typename T>
        void pack(T& packer)
        {
            packer(m_code, m_message);
        }
    private:
        // this is done as a std::uint32_t just so we can serialize the error_code
        std::uint32_t m_code;
        std::string m_message;
    };

}
