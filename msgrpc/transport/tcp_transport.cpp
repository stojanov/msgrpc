#include "util/error.h"

#include <transport/tcp_transport.h>
#include <asio/error_code.hpp>
#include <asio/read.hpp>
#include <asio/system_error.hpp>
#include <asio/write.hpp>

// Initial tcp transport, in the future this should be more robust, add packeting, middleware
// handle fragmented packets and so on
// Increase error coverage, make it show more detail so we can have more controle over what 
// we do with the error

namespace msgrpc::client
{
    tcp_transport::tcp_transport()
        :
        m_socket(m_ctx),
        m_is_running(false)
    {
        // buffer size might need to be tweaked
        m_read_buffer.resize(5120);

        start();
    }

    std::optional<error::err> tcp_transport::send(std::vector<unsigned char>& data)
    {
        asio::error_code ec;

        asio::write(m_socket, asio::buffer(data), ec);

        if (ec)
        {
            return error::err{ error::error_code::cannot_send, ec.message() };
        }

        return std::nullopt;
    }

    std::optional<error::err> tcp_transport::connect(const std::string_view ip, const std::string_view port)
    {
        asio::ip::tcp::resolver resolver(m_ctx);

        asio::error_code ec;
        auto endpoints = resolver.resolve(ip, port, ec);

        if (ec)
        {
            return error::err{ error::error_code::cannot_find_address, ec.message() };
        }

        asio::connect(m_socket, endpoints, ec);

        if (ec)
        {
            return error::err{ error::error_code::cannot_connect, ec.message() };
        }
        
        return {};
    }

    void tcp_transport::disconnect()
    {
        m_socket.close();
    }

    std::optional<error::err> tcp_transport::start()
    {
        bool a_false = false;

        if (!m_is_running.compare_exchange_strong(a_false, true))
        {
            return error::err{ error::error_code::thread_already_running, "tcp thread is already running, maybe try to restart it if needed"};
        }
        
        if (m_thread.joinable())
        {
            // sanity check, in case is running
            // will do a restart
            stop();
        }

        m_thread = std::thread([this]
                               {
                                    m_ctx.run();
                                    m_is_running = false;
                               });


        // initiate reading 
        handle_read();

        return std::nullopt;
    }

    void tcp_transport::stop()
    {
        m_ctx.stop();
        m_thread.join();
    }

    void tcp_transport::handle_read()
    {
        asio::async_read(m_socket, asio::buffer(m_read_buffer),
                         [this](const std::error_code& ec, std::size_t bytes_written)
                         {
                            if (ec)
                            { 
                                // do something 
                                // TODO: add logging 
                                return;
                            }

                            // received(m_read_buffer);

                            handle_read();
                         });
    }

}


