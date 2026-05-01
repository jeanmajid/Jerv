#include "jerv/core/jerver.hpp"

#include "jerv/common/logger.hpp"
#include "jerv/protocol/packets/networkSettings.hpp"
#include "jerv/protocol/packets/requestNetworkSettings.hpp"

#include "jerv/core/packetHandler.hpp"
#include "jerv/protocol/packets/networkChunkPublisherUpdate.hpp"

namespace jerv::core {
    Jerver::Jerver() {
    }

    void Jerver::bindV4(const uint16_t port) {
        raknetServer.bindV4(port);
    }

    void Jerver::bindV6(const uint16_t port) {
        raknetServer.bindV6(port);
    }

    void Jerver::start() {
        tickManager.setCallback(this, &handleTickStatic);
        tickManager.start();

        raknetServer.setCallback(this, &handleDataStatic);
        raknetServer.start();
    }

    void Jerver::handleDataStatic(void *ctx, raknet::ServerConnection &connection, const std::span<uint8_t> data) {
        static_cast<Jerver *>(ctx)->handleData(connection, data);
    }

    void Jerver::handleTickStatic(void *ctx, const uint64_t tick) {
        static_cast<Jerver *>(ctx)->handleTick(tick);
    }

    void Jerver::handleTick(const uint64_t tick) {
        // TODO: loop causes segfault as this runs on a different thread then the networking one
        for (raknet::ServerConnection &connection: std::views::values(raknetServer.connections)) {
            if (!connection.playerSpawned) {
                continue;
            }
            auto chunks = dimension.generator.generateChunks({
                                                                 connection.playerLocationX, connection.playerLocationY,
                                                                 connection.playerLocationZ
                                                             }, connection.playerViewDistance);

            if (chunks.first.size() == 0) {
                continue;
            }
            for (world::generator::Chunk *chunk: chunks.second) {
                send(connection, chunk->serialize());
            }

            protocol::NetworkChunkPublisherUpdatePacket update;
            update.coordinate = {
                static_cast<int32_t>(std::floor(connection.playerLocationX)),
                static_cast<int32_t>(std::floor(connection.playerLocationY)),
                static_cast<int32_t>(std::floor(connection.playerLocationZ))
            };
            update.radius = connection.playerViewDistance << 4;
            update.savedChunks = std::move(chunks.first);
            send(connection, update);
        }
    }

    void Jerver::handleData(raknet::ServerConnection &connection, const std::span<uint8_t> data) {
        binary::Cursor cursor(data);
        if (connection.networkSettingsSent) {
            protocol::CompressionAlgorithm compression = static_cast<protocol::CompressionAlgorithm>(cursor.
                readUint8());
            // TODO: handle compression
        }

        while (!cursor.isEndOfStream()) {
            const int32_t packetSize = cursor.readVarInt32();
            if (packetSize <= 0 || static_cast<size_t>(packetSize) > cursor.getRemainingBytes().size()) {
                JERV_LOG_WARN("bad packet length: {}", packetSize);
                return;
            }

            const auto buffer = cursor.getSliceSpan(static_cast<size_t>(packetSize));
            cursor.setPointer(cursor.pointer() + packetSize);
            handlePacket(connection, buffer);
        }
    }

    void Jerver::send(raknet::ServerConnection &connection, const protocol::PacketType &packet) {
        // TODO: sendbatch

        // TODO: handle with one global send buffer and optimise this buffer mess
        constexpr size_t MAX_PACKET_BUFFER_SIZE = 524288;
        constexpr size_t MAX_VARINT_SIZE = 5;
        constexpr size_t MAX_RAKNET_HEADER_SIZE = 2;

        binary::ResizableCursor packetBufferCursor(
            MAX_PACKET_BUFFER_SIZE + MAX_VARINT_SIZE,
            MAX_PACKET_BUFFER_SIZE + MAX_VARINT_SIZE
        );
        packetBufferCursor.writeVarInt32(static_cast<int32_t>(packet.getPacketId()));
        packet.serialize(packetBufferCursor);

        binary::ResizableCursor gamePacketBufferCursor(
            MAX_PACKET_BUFFER_SIZE + MAX_VARINT_SIZE + MAX_VARINT_SIZE,
            MAX_PACKET_BUFFER_SIZE + MAX_VARINT_SIZE + MAX_VARINT_SIZE
        );
        gamePacketBufferCursor.writeVarInt32(static_cast<int32_t>(packetBufferCursor.getProcessedBytes().size()));
        gamePacketBufferCursor.writeSliceSpan(packetBufferCursor.getProcessedBytes());

        binary::ResizableCursor sendBufferCursor(
            MAX_PACKET_BUFFER_SIZE + MAX_VARINT_SIZE + MAX_VARINT_SIZE + MAX_RAKNET_HEADER_SIZE,
            MAX_PACKET_BUFFER_SIZE + MAX_VARINT_SIZE + MAX_VARINT_SIZE + MAX_RAKNET_HEADER_SIZE
        );
        sendBufferCursor.writeUint8(static_cast<uint8_t>(raknet::RaknetPacketId::GameData));
        if (connection.networkSettingsSent) {
            sendBufferCursor.writeUint8(static_cast<uint8_t>(protocol::CompressionAlgorithm::NoCompression));
            // TODO handle compression
        }

        sendBufferCursor.writeSliceSpan(gamePacketBufferCursor.getProcessedBytes());

        raknetServer.sendFrame(connection, sendBufferCursor.getProcessedBytes(), raknet::Reliable);
    }

    void Jerver::handlePacket(raknet::ServerConnection &connection, const std::span<uint8_t> data) {
        binary::Cursor cursor(data);
        const int32_t packetId = cursor.readVarInt32();

        const auto &handlers = getHandlers();

        if (handlers[packetId]) {
            handlers[packetId](*this, connection, cursor);
        } else {
            JERV_LOG_DEBUG("unhandled game packet: 0x{:X}", packetId);
        }
    }
}
