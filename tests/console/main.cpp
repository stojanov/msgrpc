#include <bits/types/struct_sched_param.h>
#include <memory>
#include "client/client.h"
#include "client/transport/transport.h"
#include "core/common.h"
#include "util/signal.h"

#include <iostream>

class tcp_transport: public msgrpc::client::transport
{
public:
    void connect();

    std::optional<msgrpc::error::err> send(msgrpc::core::data_buffer& data) override
    {
        return {};
    }
};

struct Param
{
    int a;

    template<typename T>
    void pack(T& packer)
    {
        packer(a);
    }
};

struct Response
{
    int b;

    template<typename T>
    void pack(T& packer)
    {
        packer(b);
    }
};

int main()
{
    auto transport = std::make_shared<tcp_transport>();

    // transport->connect();

    msgrpc::util::single_signal<void(int)> signal;
    

    signal.connect([](int a)
            {
                std::cout << a << "\n";
            });
    
    msgrpc::client::client client(transport);

    auto AddGame = client.register_function_call<Param, Response>("AddGame", nullptr);

    signal(1);

    std::cout << "Got to here\n";
    Param p{ 1 };
    const auto response = AddGame(p);
}
