#pragma once

#include <jerv/protocol/packet.hpp>
#include <jerv/protocol/enums.hpp>
#include <vector>
#include <cstdint>

namespace jerv::protocol {
    class MovePlayerPacket : public PacketType {
    public:
        enum MovePlayerMode : uint8_t {
            Normal = 0,
            Reset = 1,
            Teleport = 2,
            Rotation = 3
        };

        enum MovePlayerTeleportCause : int32_t {
            Unknown = 0,
            Projectile = 1,
            ChorusFruit = 2,
            Command = 3,
            Behaviour = 4
        };
        
        int32_t runtimeId;

        Vec3f position;
        float pitch;
        float yaw;
        float headYaw;

        MovePlayerMode mode;
        bool onGround;

        int32_t riddenRuntimeId;

        MovePlayerTeleportCause teleportCause;
        int32_t sourceEntityType;

        int64_t tick;

        PacketId getPacketId() const override {
            return PacketId::MovePlayer;
        }

        void serialize(binary::cursor &cursor) const override {
            cursor.writeVarInt(runtimeId);

            position.serialize(cursor);
            cursor.writeFloat32(pitch, true);
            cursor.writeFloat32(yaw, true);
            cursor.writeFloat32(headYaw, true);

            cursor.writeUint8(mode);
            cursor.writeBool(onGround);

            cursor.writeVarInt(riddenRuntimeId);

            if (mode == MovePlayerMode::Teleport) {
                cursor.writeInt32(teleportCause, true);
                cursor.writeInt32(sourceEntityType, true);
            }

            cursor.writeVarInt(tick);
        }

        void deserialize(binary::cursor &cursor) override {
            
        }
    };
}
