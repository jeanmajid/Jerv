#include <jerv/core/network_connection.hpp>
#include <jerv/core/jerver.hpp>

namespace jerv::core {
    void NetworkConnection::handleSubChunkRequest(binary::Cursor &cursor) {
        protocol::SubChunkRequestPacket req;
        req.deserialize(cursor);

        for (const auto &entry: req.requests) {
            auto &chunk = jerver().world().getChunk(entry.coords.x, entry.coords.z);
            auto subPackets = chunk.serializeSubChunks();

            std::vector<protocol::PacketType *> chunkPacketPtrs;
            chunkPacketPtrs.reserve(24);

            for (auto &pkt: subPackets) {
                uint8_t idx = static_cast<uint8_t>(pkt.subChunkIndex);
                if (idx >= entry.minIndex && idx <= entry.maxIndex) {
                    chunkPacketPtrs.push_back(&pkt);
                }
            }

            if (!chunkPacketPtrs.empty()) {
                sendBatch(chunkPacketPtrs);
            }
        }
    }
}