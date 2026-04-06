#pragma once
#include "jerv/raknet/protocol/raknetBasePacket.hpp"

namespace jerv::raknet {
    class ConnectedPingPacket : public RaknetBasePacket {
    public:
        uint64_t timeSinceStart;

        RaknetPacketId getPacketId() const override {
            return RaknetPacketId::ConnectedPing;
        }

        void serialize(jerv::binary::Cursor &cursor) const override {
            (void) cursor;
        }

        void deserialize(binary::Cursor &cursor) override {
            timeSinceStart = cursor.readUint64();
        }
    };
}
