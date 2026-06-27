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
#include <vector>
#include <cstdint>

namespace jerv::protocol {
    class CraftingDataPacket : public PacketType {
    public:
        std::vector<uint8_t> recipes;
        std::vector<uint8_t> potionMixData;
        std::vector<uint8_t> containerMixData;
        std::vector<uint8_t> materialReducers;
        bool clearRecipes = true;

        static constexpr auto ID = PacketId::CraftingData;
        PacketId getPacketId() const override {
            return PacketId::CraftingData;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeVarInt32(0);
            cursor.writeVarInt32(0);
            cursor.writeVarInt32(0);
            cursor.writeVarInt32(0);
            cursor.writeBool(clearRecipes);
        }

        void deserialize(binary::Cursor &cursor) override {
            int32_t recipeCount = cursor.readVarInt32();
            int32_t potionCount = cursor.readVarInt32();
            int32_t containerCount = cursor.readVarInt32();
            int32_t reducerCount = cursor.readVarInt32();

            clearRecipes = cursor.readBool();
        }
    };
}