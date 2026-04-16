#pragma once

#include <jerv/protocol/packet.hpp>

#include "jerv/protocol/enums.hpp"

namespace jerv::protocol {
    class LevelSoundEventPacket : public PacketType {
    public:
        int32_t eventId;
        Vec3f position;
        int32_t data;
        std::string actorIdentifier;
        bool isBaby;
        bool isGlobal;
        int64_t actorUniqueId;

        static constexpr auto ID = PacketId::LevelSoundEvent;
        PacketId getPacketId() const override {
            return PacketId::LevelSoundEvent;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeVarInt32(eventId);
            position.serialize(cursor);
            cursor.writeZigZag32(data);
            cursor.writeString(actorIdentifier);
            cursor.writeBool(isBaby);
            cursor.writeBool(isGlobal);
            cursor.writeInt64<true>(actorUniqueId);
        }

        void deserialize(binary::Cursor &cursor) override {
        }
    };
}
