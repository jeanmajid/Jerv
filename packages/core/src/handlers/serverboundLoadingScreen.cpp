#include "jerv/core/jerver.hpp"
#include "jerv/core/packetHandler.hpp"
#include "jerv/protocol/packets/playStatus.hpp"
#include "jerv/protocol/packets/serverBoundLoadingScreen.hpp"

namespace jerv::core {
    void handleServerboundLoadingScreenPacket(Jerver &server, raknet::ServerConnection &connection,
                                              binary::Cursor &cursor) {
        // TODO: check if this actually can be send multiple time, I guess it can
        if (!connection.playerSpawned) {
            connection.playerSpawned = true;
            protocol::PlayStatusPacket spawnStatus;
            spawnStatus.status = protocol::PlayStatus::PlayerSpawn;
            server.send(connection, spawnStatus);
            JERV_LOG_INFO("spawned {}", connection.playerName);
        }
    }

    static PacketRegistrar<protocol::ServerBoundLoadingScreenPacket> regServerBoundLoadingScreen{&handleServerboundLoadingScreenPacket};
}
