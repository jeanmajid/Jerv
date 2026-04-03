#pragma once
#include "packetIds.hpp"
#include "jerv/binary/cursor.hpp"

namespace jerv::raknet {
    class RaknetBasePacket {
    public:
        virtual ~RaknetBasePacket() = default;

        virtual RaknetPacketId getPacketId() const = 0;

        virtual void serialize(jerv::binary::Cursor &cursor) const = 0;

        virtual void deserialize(jerv::binary::Cursor &cursor) = 0;
    };
}
