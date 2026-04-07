#include "jerv/core/jerver.hpp"

#include "jerv/protocol/packetIds.hpp"

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
        raknetServer.setCallback(this, &handleDataStatic);
        raknetServer.start();
    }

    void Jerver::handleDataStatic(void *ctx, raknet::ServerConnection &connection, const std::span<uint8_t> data) {
        static_cast<Jerver *>(ctx)->handleData(connection, data);
    }

    void Jerver::handleData(raknet::ServerConnection &connection, const std::span<uint8_t> data) {
        // TODO Handle compression

        binary::Cursor cursor(data);
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

    void Jerver::handlePacket(raknet::ServerConnection &connection, std::span<uint8_t> data) {
        binary::Cursor cursor(data);
        const uint8_t packetId = cursor.readUint8();

        switch (packetId) {
            case protocol::PacketId::RequestNetworkSettings: {}
        }
    }
}
