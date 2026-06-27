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
    class SubChunkPacket : public PacketType {
    public:
        int32_t x = 0;
        int32_t z = 0;
        DimensionId dimension = DimensionId::Overworld;
        int8_t subChunkIndex = 0;
        bool cacheEnabled = false;
        std::vector<uint64_t> blobs;
        std::vector<uint8_t> data;

        static constexpr auto ID = PacketId::SubChunk;
        PacketId getPacketId() const override { return PacketId::SubChunk; }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeZigZag32(x);
            cursor.writeZigZag32(z);
            cursor.writeZigZag32(static_cast<int32_t>(dimension));
            cursor.writeUint8(subChunkIndex);
            cursor.writeBool(cacheEnabled);
            if (cacheEnabled) {
                cursor.writeVarInt32(static_cast<int32_t>(blobs.size()));
                for (const uint64_t hash: blobs) {
                    cursor.writeUint64<true>(hash);
                }
            }
            cursor.writeVarInt32(data.size());
            cursor.writeSliceSpan(data);
        }

        void deserialize(binary::Cursor &cursor) override {
            x = cursor.readZigZag32();
            z = cursor.readZigZag32();
            dimension = static_cast<DimensionId>(cursor.readZigZag32());
            subChunkIndex = static_cast<int8_t>(cursor.readUint8());
            cacheEnabled = cursor.readBool();
            blobs.clear();
            if (cacheEnabled) {
                const int32_t blobCount = cursor.readVarInt32();
                if (blobCount > 64) {
                    throw std::runtime_error("Too many blob hashes");
                }
                blobs.reserve(blobCount);
                for (int32_t i = 0; i < blobCount; i++) {
                    blobs.push_back(cursor.readUint64<true>());
                }
            }
            const int32_t dataLength = cursor.readVarInt32();
            auto dataSpan = cursor.readSliceSpan(static_cast<size_t>(dataLength));
            data.assign(dataSpan.begin(), dataSpan.end());
        }
    };
}
