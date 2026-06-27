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

namespace jerv::protocol {
    class PlayerAuthInputPacket : public PacketType {
    public:
        // TODO: rest of the packet not implemented yet, obviously for perfomance reasons, not bcs im lazy :)
        float pitch;
        float yaw;
        Vec3f position;

        static constexpr auto ID = PacketId::PlayerAuthInput;
        PacketId getPacketId() const override {
            return PacketId::PlayerAuthInput;
        }

        void serialize(binary::Cursor &cursor) const override {
        }

        void deserialize(binary::Cursor &cursor) override {
            pitch = cursor.readFloat32<true>();
            yaw = cursor.readFloat32<true>();
            position.deserialize(cursor);
        }
    };
}
