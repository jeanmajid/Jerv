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
    struct RequestEntry {
        ChunkCoords coords;
        uint8_t minIndex = 0;
        uint8_t maxIndex = 0;

        void serialize(binary::Cursor &cursor) const {
            coords.serialize(cursor);
            cursor.writeUint8(minIndex);
            cursor.writeUint8(maxIndex);
        }

        void deserialize(binary::Cursor &cursor) {
            coords.deserialize(cursor);
            minIndex = cursor.readUint8();
            maxIndex = cursor.readUint8();
        }
    };

    class SubChunkRequestPacket : public PacketType {
    public:
        DimensionId dimension = DimensionId::Overworld;
        std::vector<RequestEntry> requests;

        static constexpr auto ID = PacketId::SubChunkRequest;
        PacketId getPacketId() const override {
            return PacketId::SubChunkRequest;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeZigZag32(static_cast<int32_t>(dimension));
            cursor.writeVarInt32(static_cast<int32_t>(requests.size()));
            for (const auto &req: requests) {
                req.serialize(cursor);
            }
        }

        void deserialize(binary::Cursor &cursor) override {
            dimension = static_cast<DimensionId>(cursor.readZigZag32());
            const int32_t count = cursor.readVarInt32();
            requests.clear();
            requests.reserve(count);
            for (int32_t i = 0; i < count; i++) {
                RequestEntry entry;
                entry.deserialize(cursor);
                requests.push_back(entry);
            }
        }
    };
}
