#include <jerv/core/network_connection.hpp>
#include <jerv/core/jerver.hpp>
#include <jerv/common/logger.hpp>
#include <cmath>
#include <algorithm>

namespace jerv::core {
    void NetworkConnection::handleRequestChunkRadius(binary::Cursor &cursor) {
        protocol::RequestChunkRadiusPacket packet;
        packet.deserialize(cursor);

        int32_t viewRadius = std::min(packet.chunkRadius, MAX_VIEW_RADIUS);

        bool isFirstRequest = !playerSpawned_;
        bool viewDistanceChanged = chunkManager_.viewDistance() != viewRadius;

        if (isFirstRequest || viewDistanceChanged) {
            JERV_LOG_INFO("view radius {} -> {}", packet.chunkRadius, viewRadius);
            chunkManager_.setViewDistance(viewRadius);
        }

        const int32_t currentChunkX = static_cast<int32_t>(std::floor(playerX_)) >> 4;
        const int32_t currentChunkZ = static_cast<int32_t>(std::floor(playerZ_)) >> 4;
        chunkManager_.setPosition(currentChunkX, currentChunkZ);

        protocol::ChunkRadiusUpdatePacket radiusUpdate;
        radiusUpdate.radius = viewRadius;
        send(radiusUpdate);

        try {
            updateChunks(jerver().world());
        } catch (const std::exception &e) {
            JERV_LOG_ERROR("chunk gen error: {}", e.what());
        }

        if (!playerSpawned_) {
            playerSpawned_ = true;
            protocol::PlayStatusPacket spawnStatus;
            spawnStatus.status = protocol::PlayStatus::PlayerSpawn;
            send(spawnStatus);
            JERV_LOG_INFO("spawned {}", playerName_);
        }
    }
}
