#include "jerv/core/jerver.hpp"
#include "jerv/core/packetHandler.hpp"
#include "jerv/protocol/packets/login.hpp"

namespace jerv::core {
    void handleLoginPacket(Jerver &server, raknet::ServerConnection &connection,
                      binary::Cursor &cursor) {
        protocol::LoginPacket login;
        login.deserialize(cursor);

        JERV_LOG_INFO("LOGIN");
    }

    static PacketRegistrar<protocol::LoginPacket> reg{&handleLoginPacket};
}
