#pragma once
#include "jerv/raknet/raknetServer.hpp"

namespace jerv::core {
    class Jerver {
    public:
        Jerver();

        void bindV4(uint16_t port = raknet::NETWORK_LAN_DISCOVERY_PORT4);

        void bindV6(uint16_t port = raknet::NETWORK_LAN_DISCOVERY_PORT6);

        void start();

    private:
        static void handleDataStatic(void *ctx, raknet::ServerConnection &serverConnection, binary::Cursor &cursor);

        void handleData(raknet::ServerConnection &serverConnection, binary::Cursor &cursor);

        raknet::RaknetServer raknetServer;
    };
}
