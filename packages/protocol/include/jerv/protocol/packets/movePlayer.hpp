/* SPDX-License-Identifier: LGPL-3.0-or-later
 * ============================================================================
 *  Jerv - Minecraft Bedrock Server Software
 *  Copyright (C) 2025-2026 jeanmajid
 *  https://github.com/jeanmajid/Jerv
 * ============================================================================
 *
 * This file is part of Jerv.
 *
 * Jerv is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Jerv is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Jerv. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <jerv/protocol/packet.hpp>
#include <jerv/protocol/enums.hpp>
#include <vector>
#include <cstdint>

namespace jerv::protocol {
    enum class MovePlayerMode : uint8_t {
        Normal = 0,
        Reset = 1,
        Teleport = 2,
        Rotation = 3
    };

    enum class MovePlayerTeleportCause : int32_t {
        Unknown = 0,
        Projectile = 1,
        ChorusFruit = 2,
        Command = 3,
        Behaviour = 4
    };

    class MovePlayerPacket : public PacketType {
    public:
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

        static constexpr auto ID = PacketId::MovePlayer;
        PacketId getPacketId() const override {
            return PacketId::MovePlayer;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeVarInt32(runtimeId);

            position.serialize(cursor);
            cursor.writeFloat32<true>(pitch);
            cursor.writeFloat32<true>(yaw);
            cursor.writeFloat32<true>(headYaw);

            cursor.writeUint8(static_cast<uint8_t>(mode));
            cursor.writeBool(onGround);

            cursor.writeVarInt32(riddenRuntimeId);

            if (mode == MovePlayerMode::Teleport) {
                cursor.writeInt32<true>(static_cast<int32_t>(teleportCause));
                cursor.writeInt32<true>(sourceEntityType);
            }

            cursor.writeVarInt32(tick);
        }

        void deserialize(binary::Cursor &cursor) override {
            
        }
    };
}
