#pragma once
#include <asio/ip/udp.hpp>

namespace jerv::raknet {
    class RaknetServer {
        asio::io_context ioContext;
        asio::ip::udp::socket socket;

        RaknetServer(const int16_t port);
    };
}
