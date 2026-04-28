#include "jerv/core/jerver.hpp"
#include "jerv/core/packetHandler.hpp"
#include "jerv/protocol/packets/requestChunkRadius.hpp"

namespace jerv::core::handler {
    void handleRequestChunkRadiusPacket(Jerver &server, raknet::ServerConnection &connection,
                                  binary::Cursor &cursor) {
        protocol::RequestChunkRadiusPacket packet;
        packet.deserialize(cursor);
        int32_t viewRadius = std::min(packet.chunkRadius, 120); // TODO: max render distance from some config

        bool isFirstRequest = !connection.playerSpawned;
        bool viewDistanceChanged = connection.playerViewDistance != viewRadius;

        if (isFirstRequest || viewDistanceChanged) {
            JERV_LOG_INFO("view radius {} -> {}", packet.chunkRadius, viewRadius);
        }

        connection.playerViewDistance = viewRadius;
    }

    static PacketRegistrar<protocol::RequestChunkRadiusPacket> regRequestChunkRadius{&handleRequestChunkRadiusPacket};
}
