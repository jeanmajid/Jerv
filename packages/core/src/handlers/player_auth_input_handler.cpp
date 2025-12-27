#include <jerv/core/network_connection.hpp>
#include <jerv/core/jerver.hpp>
#include <jerv/common/logger.hpp>
#include <cmath>

namespace jerv::core {
    void NetworkConnection::handlePlayerAuthInput(binary::cursor &cursor) {
        protocol::PlayerAuthInputPacket packet;
        packet.deserialize(cursor);

        playerX_ = packet.position.x;
        playerY_ = packet.position.y;
        playerZ_ = packet.position.z;

        const int32_t newChunkX = static_cast<int32_t>(std::floor(packet.position.x)) >> 4;
        const int32_t newChunkZ = static_cast<int32_t>(std::floor(packet.position.z)) >> 4;

        if (newChunkX != chunkManager_.chunkX() || newChunkZ != chunkManager_.chunkZ()) {
            chunkManager_.setPosition(newChunkX, newChunkZ);

            try {
                updateChunks(jerver().world());
            } catch (const std::exception &e) {
                JERV_LOG_ERROR("chunk gen error: ", e.what());
            }
        }
    }
}
