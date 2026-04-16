#include "jerv/core/jerver.hpp"
#include "jerv/core/packetHandler.hpp"
#include "jerv/protocol/packets/playerAuthInput.hpp"

namespace jerv::core {
    void handlePlayerAuthInputPacket(Jerver &server, raknet::ServerConnection &connection,
                      binary::Cursor &cursor) {
        protocol::PlayerAuthInputPacket loginPacket;
        loginPacket.deserialize(cursor);
    }

    static PacketRegistrar<protocol::PlayerAuthInputPacket> regLogin{&handlePlayerAuthInputPacket};
}
