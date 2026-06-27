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
