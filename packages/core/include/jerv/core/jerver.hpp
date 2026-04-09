#pragma once
#include "jerv/protocol/packets/networkSettings.hpp"
#include "jerv/raknet/raknetServer.hpp"

namespace jerv::core {
    class Jerver {
    public:
        Jerver();

        void bindV4(uint16_t port = raknet::NETWORK_LAN_DISCOVERY_PORT4);

        void bindV6(uint16_t port = raknet::NETWORK_LAN_DISCOVERY_PORT6);

        void start();

        void send(raknet::ServerConnection &connection, const protocol::PacketType &packet);

    private:
        static void handleDataStatic(void *ctx, raknet::ServerConnection &connection, std::span<uint8_t> data);

        void handleData(raknet::ServerConnection &connection, std::span<uint8_t> data);

        void handlePacket(raknet::ServerConnection &connection, std::span<uint8_t> data);

        raknet::RaknetServer raknetServer;
    };
}
