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
    class NetworkChunkPublisherUpdatePacket : public PacketType {
    public:
        BlockLocation coordinate;
        uint32_t radius = 0;
        std::vector<ChunkCoords> savedChunks;

        static constexpr auto ID = PacketId::NetworkChunkPublisherUpdate;
        PacketId getPacketId() const override {
            return PacketId::NetworkChunkPublisherUpdate;
        }

        void serialize(binary::Cursor &cursor) const override {
            coordinate.serialize(cursor);
            cursor.writeVarInt32(static_cast<int32_t>(radius));
            cursor.writeUint32<true>(static_cast<uint32_t>(savedChunks.size()));

            for (const auto &chunk: savedChunks) {
                chunk.serialize(cursor);
            }
        }

        void deserialize(binary::Cursor &cursor) override {
            coordinate.deserialize(cursor);
            radius = static_cast<uint32_t>(cursor.readVarInt32());
            const uint32_t count = cursor.readUint32<true>();
            savedChunks.clear();
            savedChunks.reserve(count);

            for (uint32_t i = 0; i < count; i++) {
                ChunkCoords coords;
                coords.deserialize(cursor);
                savedChunks.push_back(coords);
            }
        }
    };
}
