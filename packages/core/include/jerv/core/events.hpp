#pragma once

#include <jerv/protocol/protocol.hpp>
#include <jerv/binary/cursor.hpp>
#include <functional>
#include <span>

namespace jerv::core {
    class NetworkConnection;

    struct PacketEvent {
        NetworkConnection &connection;
        protocol::PacketId packetId;
        std::span<uint8_t> rawData;
        binary::Cursor cursor;
        bool cancelled = false;

        PacketEvent(NetworkConnection &conn, protocol::PacketId id, std::span<uint8_t> data)
            : connection(conn)
              , packetId(id)
              , rawData(data)
              , cursor(binary::Cursor::create(data)) {
        }

        void cancel() { cancelled = true; }
    };

    struct ConnectionEvent {
        NetworkConnection &connection;
        bool cancelled = false;

        explicit ConnectionEvent(NetworkConnection &conn) : connection(conn) {
        }

        void cancel() { cancelled = true; }
    };

    using PacketHandler = std::function<void(PacketEvent &)>;
    using ConnectionHandler = std::function<void(ConnectionEvent &)>;
}