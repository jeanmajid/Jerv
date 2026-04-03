#pragma once
#include <asio.hpp>

#include "constants.hpp"

namespace jerv::raknet {
    class RaknetServer {
    public:
        void bindV4(uint16_t port = NETWORK_LAN_DISCOVERY_PORT4, const std::string &address = NETWORK_ANY_ADDRESS4);
        void bindV6();

        void start();
    private:
        void startReceive(asio::ip::udp::socket &socket);

        void onMessage(asio::ip::udp::endpoint &endpoint, std::span<uint8_t> data);

        asio::io_context ioContext_;
        std::unique_ptr<asio::ip::udp::socket> _socket4;
        std::unique_ptr<asio::ip::udp::socket> _socket6;

        std::array<uint8_t, 1500> _receiveBuffer = {};
        asio::ip::udp::endpoint remoteEndpoint;
    };
}
