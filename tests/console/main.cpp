#include <bits/types/struct_sched_param.h>
#include <memory>
#include "client/client.h"
#include "client/transport.h"


class test: public msgrpc::client::transport
{
public:
    void send(std::vector<unsigned char>) override
    {

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
    auto transport = std::make_shared<test>();

    msgrpc::client::client client(transport);

    auto handle = client.register_function_call<Param, Response>("AddGame", nullptr);
}
