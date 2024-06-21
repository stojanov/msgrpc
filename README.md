# MessagePack based Rpc for C++
![build status](https://github.com/stojanov/msgrpc/actions/workflows/main.yml/badge.svg)
![last commit](https://img.shields.io/github/last-commit/stojanov/msgrpc)
![commit activity](https://img.shields.io/github/commit-activity/m/stojanov/msgrpc)


`msgrpc` is an experimental library i am working on to learn more about how a simple rpc works,
the idea is to have a very straightforward api

### Building instructions

Should have [vcpkg](https://github.com/microsoft/vcpkg) installed and setup up in a way that you have `VCPKG_ROOT` environment variable set up.
[Cmake](https://cmake.org) is needed.


Next step would be to clone the repo, generate cmake, and build the project:

```bash
git clone https://github.com/stojanov/msgrpc.git
cd msgrpc
./cmake_generate.sh
./cmake_build.sh
```

No executables, no tests... nothing.

For now i am very much into experimenting with this but the basic api/idea can be seen below

## Overview on how the API is supposed to look
#### Create the basic transport
```C++
class tcp_transport: public msgrpc::client::transport
{
    // implement the transport here

    virtual std::optional<msgrpc::error::err> send(core::data_buffer& data) override;

    // also received messages are passed as a signal/callback
}
```

#### Instantiate the client with a given transport
```C++
auto transport = std::make_shared<tcp_transport>();
msgrpc::client::client client(transport);
```

#### Create a function call along with a Parameter and Response types


```C++
struct Param
{
    int a;

    // main entry point for seralizing
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
}

// you can also attach a external callback which will be invoked if the client/server on 
// the other end of the rpc invoked it, also you can attach a listener to this to not have
// the polling/waiting of the result showcased below
auto AddGameRpcHandle = client.register_function_call<Param, Response>("AddGame", nullptr);
```

#### Call the function and let the library do it's magic

```C++
Param p{ 1 };
// will send it async so it won't block
const auto response = AddGameRpcHandle(p);

if (response)
{
    const auto result = reponse->wait_for(std::chrono::milliseconds(100));

    if (result)
    {
        Response gotResponse = result.value();
    }
}
```
