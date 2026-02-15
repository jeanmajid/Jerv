#pragma once

#include <jerv/protocol/packet.hpp>
#include <vector>
#include <string>

namespace jerv::protocol {
    class AvailableActorIdentifiersPacket : public PacketType {
    public:
        PacketId getPacketId() const override {
            return PacketId::AvailableActorIdentifiers;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeUint8(0x0A);
            cursor.writeUint8(0x00);
            cursor.writeUint8(0x09);
            cursor.writeUint8(0x06);

            cursor.writeUint8('i');
            cursor.writeUint8('d');
            cursor.writeUint8('l');
            cursor.writeUint8('i');
            cursor.writeUint8('s');
            cursor.writeUint8('t');

            cursor.writeUint8(0x0A);
            cursor.writeZigZag32(0);

            cursor.writeUint8(0x00);
        }

        void deserialize(binary::Cursor &cursor) override {
        }
    };
}