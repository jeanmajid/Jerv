#include "jerv/core/jerver.hpp"
#include "jerv/core/packetHandler.hpp"
#include "jerv/protocol/packets/playerAuthInput.hpp"

namespace jerv::core::handler {
    void handlePlayerAuthInputPacket(Jerver &server, raknet::ServerConnection &connection,
                                     binary::Cursor &cursor) {
        protocol::PlayerAuthInputPacket playerAuthInput;
        playerAuthInput.deserialize(cursor);

        connection.playerLocationX = playerAuthInput.position.x;
        connection.playerLocationY = playerAuthInput.position.y;
        connection.playerLocationZ = playerAuthInput.position.z;
    }

    static PacketRegistrar<protocol::PlayerAuthInputPacket> regAuthInput{&handlePlayerAuthInputPacket};
}
