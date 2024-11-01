
#include <client/client.h>
#include <transport/transport.h>

#include "core/common.h"

class tcp_transport : public msgrpc::transport {
   public:
    void connect();

    std::optional<msgrpc::error::err> send(msgrpc::data_buffer data) override {
        return {};
    }

    msgrpc::msg_result<msgrpc::data_buffer> receive(
        std::chrono::milliseconds timeout) override {
        return msgrpc::data_buffer();
    }
};

struct Param {
    int a;

    template <typename T>
    void pack(T& packer) {
        packer(a);
    }
};

struct Response {
    int b;

    template <typename T>
    void pack(T& packer) {
        packer(b);
    }
};

int main() {
    auto transport = std::make_shared<tcp_transport>();

    msgrpc::client::client client(transport);

    auto AddGame =
        client.register_function_call<Param, Response>("AddGame", nullptr);

    Param p{1};
    const auto response = AddGame(p);
}
