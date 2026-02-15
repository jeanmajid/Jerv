#pragma once

#include <jerv/protocol/packetIds.hpp>
#include <jerv/binary/cursor.hpp>
#include <cstdint>

namespace jerv::protocol {
    class PacketType {
    public:
        virtual ~PacketType() = default;

        virtual PacketId getPacketId() const = 0;

        virtual void serialize(binary::Cursor &cursor) const = 0;

        virtual void deserialize(binary::Cursor &cursor) = 0;
    };
} 


