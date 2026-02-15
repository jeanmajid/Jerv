#include <jerv/core/network_connection.hpp>
#include <jerv/core/network_server.hpp>
#include <jerv/core/jerver.hpp>
#include <jerv/core/events.hpp>
#include <jerv/core/world/world.hpp>
#include <jerv/binary/cursor.hpp>
#include <jerv/common/logger.hpp>
#include <jerv/protocol/packets/network_settings.hpp>
#include <jerv/protocol/enums.hpp>
#include <zlib.h>
#include <cstring>
#include <cmath>
#include <thread>
#include <future>
#include <vector>
#include <algorithm>
#include <execution>

#include "jerv/raknet/enums.hpp"
#include "spdlog/fmt/bin_to_hex.h"

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif

namespace jerv::core {
    const NetworkConnection::HandlerTable &NetworkConnection::handlerTable() {
        using protocol::PacketId;

        static constexpr HandlerTable table = [] {
            HandlerTable t{};
            t.fill(nullptr);

#define JERV_HANDLER(pid, fn) t[static_cast<size_t>(pid)] = &NetworkConnection::fn

            JERV_HANDLER(PacketId::RequestNetworkSettings, handleRequestNetworkSettings);
            JERV_HANDLER(PacketId::Login, handleLogin);
            JERV_HANDLER(PacketId::ResourcePackClientResponse, handleResourcePackClientResponse);
            JERV_HANDLER(PacketId::ClientCacheStatus, handleClientCacheStatus);
            JERV_HANDLER(PacketId::SetLocalPlayerAsInitialized, handleSetLocalPlayerAsInitialized);
            JERV_HANDLER(PacketId::RequestChunkRadius, handleRequestChunkRadius);
            JERV_HANDLER(PacketId::PlayerAuthInput, handlePlayerAuthInput);
            JERV_HANDLER(PacketId::PacketViolationWarning, handlePacketViolationWarning);
            JERV_HANDLER(PacketId::ServerboundLoadingScreen, handleServerboundLoadingScreen);
            JERV_HANDLER(PacketId::ServerboundDiagnostics, handleServerboundDiagnostics);
            JERV_HANDLER(PacketId::Text, handleText);
            JERV_HANDLER(PacketId::Interact, handleInteract);
            JERV_HANDLER(PacketId::CommandRequest, handleCommandRequest);
            JERV_HANDLER(PacketId::SubChunkRequest, handleSubChunkRequest);

#undef JERV_HANDLER

            return t;
        }();

        return table;
    }

    NetworkConnection::NetworkConnection(NetworkServer &server, raknet::ServerConnection &connection)
        : server_(server)
          , connection_(connection) {
        connection_.onGamePacket = [this](const std::span<uint8_t> message) {
            handlePayload(message);
        };

        networkSettings_.compressionAlgorithm = protocol::CompressionAlgorithm::Zlib;
        networkSettings_.compressionThreshold = 256;
    }

    Jerver &NetworkConnection::jerver() {
        return server_.jerver();
    }

    void NetworkConnection::disconnect() {
        connection_.disconnect();
    }

    void NetworkConnection::handlePayload(std::span<uint8_t> message) {
        if (message[0] == 0xFE) {
            message = message.subspan(1);
        }

        std::vector<uint8_t> decompressedBuffer;
        if (networkSettingsSet_) {
            auto compressionMethod = static_cast<protocol::CompressionAlgorithm>(message[0]);

            if (compressionMethod == protocol::CompressionAlgorithm::Snappy) {
                JERV_LOG_ERROR("snappy not supported");
                return;
            } else if (compressionMethod == protocol::CompressionAlgorithm::Zlib) {
                z_stream strm = {};
                strm.next_in = message.data() + 1;
                strm.avail_in = static_cast<uInt>(message.size() - 1);

                if (inflateInit2(&strm, -MAX_WBITS) != Z_OK) {
                    JERV_LOG_ERROR("zlib init failed");
                    return;
                }

                decompressedBuffer.resize(std::min<size_t>(message.size() * 4, MAX_DECOMPRESSED_SIZE));
                size_t totalOut = 0;
                int ret;

                do {
                    if (strm.avail_out == 0) {
                        const size_t currentSize = decompressedBuffer.size();
                        if (currentSize >= MAX_DECOMPRESSED_SIZE) {
                            JERV_LOG_WARN("decompressed buffer full");
                            inflateEnd(&strm);
                            return;
                        }

                        size_t newSize = std::min(currentSize * 2, MAX_DECOMPRESSED_SIZE);
                        decompressedBuffer.resize(newSize);
                    }

                    strm.next_out = decompressedBuffer.data() + totalOut;
                    strm.avail_out = static_cast<uInt>(decompressedBuffer.size() - totalOut);

                    ret = inflate(&strm, Z_NO_FLUSH);
                    totalOut = strm.total_out;

                    if (totalOut > MAX_DECOMPRESSED_SIZE) {
                        JERV_LOG_WARN("decompressed size too large: {}", totalOut);
                        inflateEnd(&strm);
                        return;
                    }

                    if (ret == Z_DATA_ERROR) {
                        JERV_LOG_ERROR("zlib error: {}", strm.msg ? strm.msg : "?");
                        inflateEnd(&strm);
                        return;
                    }
                } while (ret != Z_STREAM_END && ret != Z_BUF_ERROR);

                inflateEnd(&strm);

                if (ret != Z_STREAM_END) {
                    JERV_LOG_ERROR("decompress failed: {}", ret);
                    return;
                }

                decompressedBuffer.resize(totalOut);
                message = std::span<uint8_t>(decompressedBuffer);
            } else {
                message = message.subspan(1);
            }
        }

        binary::Cursor cursor = binary::Cursor::create(
            std::span(message.data(), message.size())
        );

        while (!cursor.isEndOfStream()) {
            int32_t length = cursor.readVarInt32();
            if (length <= 0 || static_cast<size_t>(length) > cursor.getRemainingBytes().size()) {
                JERV_LOG_WARN("bad packet length: {}", length);
                return;
            }

            const auto buffer = cursor.getSliceSpan(static_cast<size_t>(length));
            cursor.setPointer(cursor.pointer() + length);
            handlePacket(buffer);
        }
    }

    void NetworkConnection::handlePacket(const std::span<uint8_t> data) {
        binary::Cursor cursor = binary::Cursor::create(data);
        int32_t packetId = cursor.readVarInt32();

        auto id = static_cast<protocol::PacketId>(packetId);

        PacketEvent event(*this, id, data);

        event.cursor.readVarInt32();

        if (!jerver().dispatchPacket(event)) {
            return;
        }

        const auto &table = NetworkConnection::handlerTable();
        const auto idx = static_cast<size_t>(id);
        if (idx >= table.size() || table[idx] == nullptr) {
            JERV_LOG_WARN("no handler: 0x{:X}", packetId);
            return;
        }

        const auto handler = table[idx];
        (this->*handler)(cursor);
    }

    void NetworkConnection::send(const std::vector<std::unique_ptr<protocol::PacketType> > &packets) const {
        std::vector<protocol::PacketType *> ptrs;
        ptrs.reserve(packets.size());
        for (const auto &packet: packets) {
            ptrs.push_back(packet.get());
        }

        sendPacketSpan(ptrs);
    }

    void NetworkConnection::send(protocol::PacketType &packet) const {
        std::array arr{&packet};
        sendPacketSpan(arr);
    }

    void NetworkConnection::sendMultipleImpl(std::initializer_list<protocol::PacketType *> packets) const {
        std::vector ptrs(packets.begin(), packets.end());
        sendPacketSpan(ptrs);
    }

    void NetworkConnection::sendBatch(const std::vector<protocol::PacketType *> &packets) const {
        sendPacketSpan(packets);
    }

    void NetworkConnection::sendPacketSpan(const std::span<protocol::PacketType * const> packets) const {
        if (packets.empty()) {
            return;
        }

        auto &singlePacketCursor = server_.singlePacketCursor();
        auto &multiPacketCursor = server_.multiPacketCursor();

        multiPacketCursor.reset();

        for (const auto *packet: packets) {
            if (packet == nullptr) {
                continue;
            }

            singlePacketCursor.reset();
            singlePacketCursor.growToFit(65536);
            singlePacketCursor.writeVarInt32(static_cast<int32_t>(packet->getPacketId()));
            packet->serialize(singlePacketCursor);

            auto buffer = singlePacketCursor.getProcessedBytes();
            multiPacketCursor.growToFit(5 + buffer.size());
            multiPacketCursor.writeVarInt32(static_cast<int32_t>(buffer.size()));
            multiPacketCursor.writeSliceSpan(buffer);
        }

        const auto message = multiPacketCursor.getProcessedBytes();
        sendRaknetGamePacket(message);
    }

    void NetworkConnection::sendRaknetGamePacket(std::span<const uint8_t> message) const {
        const size_t messageSize = message.size();
        std::vector<uint8_t> sendBuffer(messageSize + 18);
        size_t offset = 0;

        sendBuffer[offset++] = 0xFE;
        if (networkSettingsSet_) {
            auto compression = (messageSize >= networkSettings_.compressionThreshold)
                                   ? networkSettings_.compressionAlgorithm
                                   : protocol::CompressionAlgorithm::NoCompression;

            sendBuffer[offset++] = static_cast<uint8_t>(compression);

            if (compression == protocol::CompressionAlgorithm::Zlib) {
                z_stream strm = {};
                strm.next_in = const_cast<Bytef *>(message.data());
                strm.avail_in = static_cast<uInt>(messageSize);

                if (deflateInit2(&strm, 1, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
                    JERV_LOG_ERROR("Failed to initialize zlib compression");
                    return;
                }

                std::vector<uint8_t> compressed(messageSize + 128);
                strm.next_out = compressed.data();
                strm.avail_out = static_cast<uInt>(compressed.size());

                deflate(&strm, Z_FINISH);
                deflateEnd(&strm);

                compressed.resize(strm.total_out);
                std::memcpy(sendBuffer.data() + offset, compressed.data(), compressed.size());
                offset += compressed.size();
            } else {
                std::memcpy(sendBuffer.data() + offset, message.data(), messageSize);
                offset += messageSize;
            }
        } else {
            std::memcpy(sendBuffer.data() + offset, message.data(), messageSize);
            offset += messageSize;
        }

        sendBuffer.resize(offset);
        connection_.send(sendBuffer, static_cast<uint8_t>(raknet::Reliability::Reliable));
    }

    void NetworkConnection::tick(World &world) {
        if (!playerSpawned_) {
            return;
        }

        updateChunks(world);
    }

    void NetworkConnection::updateChunks(World &world) {
        int32_t playerChunkX = chunkManager_.chunkX();
        int32_t playerChunkZ = chunkManager_.chunkZ();

        auto chunksToUnload = chunkManager_.getChunksToUnload();
        if (!chunksToUnload.empty()) {
            static thread_local std::vector<uint8_t> emptyChunkData;
            static thread_local uint32_t emptySubChunkCount = 0;
            if (emptyChunkData.empty()) {
                Chunk emptyChunk(0, 0);
                emptyChunkData = emptyChunk.serialize();
                emptySubChunkCount = emptyChunk.getSubChunkSendCount();
            }

            std::vector<protocol::LevelChunkPacket> unloadPackets;
            unloadPackets.reserve(chunksToUnload.size());

            for (int64_t hash: chunksToUnload) {
                auto coords = ChunkCoords::unhash(hash);

                protocol::LevelChunkPacket emptyPacket;
                emptyPacket.x = coords.x;
                emptyPacket.z = coords.z;
                emptyPacket.dimension = protocol::DimensionId::Overworld;
                emptyPacket.subChunkCount = emptySubChunkCount;
                emptyPacket.cacheEnabled = false;
                emptyPacket.data = emptyChunkData;
                unloadPackets.push_back(std::move(emptyPacket));

                chunkManager_.markUnloaded(hash);
            }

            std::vector<protocol::PacketType *> unloadPtrs;
            unloadPtrs.reserve(unloadPackets.size());
            for (auto &pkt: unloadPackets) {
                unloadPtrs.push_back(&pkt);
            }
            sendBatch(unloadPtrs);
        }

        auto chunksToLoad = chunkManager_.getChunksToLoad(128);

        if (chunksToLoad.empty()) {
            return;
        }

        constexpr size_t batchSize = 16;

        int32_t playerBlockX = static_cast<int32_t>(std::floor(playerX_));
        int32_t playerBlockY = static_cast<int32_t>(std::floor(playerY_));
        int32_t playerBlockZ = static_cast<int32_t>(std::floor(playerZ_));
        uint32_t radiusBlocks = static_cast<uint32_t>(chunkManager_.viewDistance()) << 4;

        size_t numChunks = chunksToLoad.size();
        std::vector<std::pair<int32_t, int32_t> > chunkCoords(numChunks);
        std::vector<Chunk *> chunkPtrs(numChunks);
        std::vector<std::vector<uint8_t> > chunkDataList(numChunks);
        std::vector<uint32_t> subChunkCounts(numChunks);

        for (size_t i = 0; i < numChunks; i++) {
            auto [cx, cz] = chunksToLoad[i];
            chunkCoords[i] = {cx, cz};
            chunkPtrs[i] = &world.getChunk(cx, cz);
            chunkManager_.markLoaded(cx, cz);
        }

         size_t numThreads = std::max(static_cast<size_t>(1), std::min(static_cast<size_t>(std::thread::hardware_concurrency()), numChunks));

        size_t chunksPerThread = (numChunks + numThreads - 1) / numThreads;
        std::vector<std::future<void> > futures;
        futures.reserve(numThreads);

        for (size_t t = 0; t < numThreads; t++) {
            size_t start = t * chunksPerThread;
            size_t end = std::min(start + chunksPerThread, numChunks);

            if (start >= numChunks) break;

            futures.push_back(std::async(std::launch::async, [&, start, end]() {
                for (size_t i = start; i < end; i++) {
                    chunkDataList[i] = chunkPtrs[i]->serialize();
                    subChunkCounts[i] = chunkPtrs[i]->getSubChunkSendCount();
                }
            }));
        }

        for (auto &future: futures) {
            future.get();
        }

        for (size_t batchStart = 0; batchStart < chunkCoords.size(); batchStart += batchSize) {
            size_t batchEnd = std::min(batchStart + batchSize, chunkCoords.size());

            protocol::NetworkChunkPublisherUpdatePacket update;
            update.coordinate = {playerBlockX, playerBlockY, playerBlockZ};
            update.radius = radiusBlocks;
            update.savedChunks.clear();
            update.savedChunks.reserve(batchEnd - batchStart);

            std::vector<protocol::LevelChunkPacket> chunkPackets;
            chunkPackets.reserve(batchEnd - batchStart);

            for (size_t i = batchStart; i < batchEnd; i++) {
                auto [cx, cz] = chunkCoords[i];
                update.savedChunks.push_back({cx, cz});

                protocol::LevelChunkPacket levelChunk;
                levelChunk.x = cx;
                levelChunk.z = cz;
                levelChunk.dimension = protocol::DimensionId::Overworld;
                levelChunk.subChunkCount = subChunkCounts[i];
                levelChunk.cacheEnabled = false;
                levelChunk.data = std::move(chunkDataList[i]);

                chunkPackets.push_back(std::move(levelChunk));
            }

            std::vector<protocol::PacketType *> packetPtrs;
            packetPtrs.reserve(chunkPackets.size() + 1);
            for (auto &pkt: chunkPackets) {
                packetPtrs.push_back(&pkt);
            }
            packetPtrs.push_back(&update);

            sendBatch(packetPtrs);
        }

        world.unloadChunksOutsideRadius(playerChunkX, playerChunkZ, chunkManager_.viewDistance() + 4);
    }
} 
