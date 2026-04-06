#pragma once
#include "jerv/raknet/protocol/raknetBasePacket.hpp"

namespace jerv::raknet {
    class ConnectedPongPacket : public RaknetBasePacket {
    public:
        uint64_t timeSinceStartClient;
        uint64_t timeSinceStartServer;

        RaknetPacketId getPacketId() const override {
            return RaknetPacketId::ConnectedPing;
        }

        void serialize(jerv::binary::Cursor &cursor) const override {
            cursor.writeInt64(timeSinceStartClient);
            cursor.writeInt64(timeSinceStartServer);
        }

        void deserialize(binary::Cursor &cursor) override {
            (void) cursor;
        }
    };
}
