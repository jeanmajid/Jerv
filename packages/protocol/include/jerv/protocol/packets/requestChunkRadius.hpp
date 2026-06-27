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

namespace jerv::protocol {
    class RequestChunkRadiusPacket : public PacketType {
    public:
        int32_t chunkRadius;
        uint8_t maxChunkRadius;

        static constexpr auto ID = PacketId::RequestChunkRadius;
        PacketId getPacketId() const override {
            return PacketId::RequestChunkRadius;
        }

        void serialize(binary::Cursor &cursor) const override {
        }

        void deserialize(binary::Cursor &cursor) override {
            chunkRadius = cursor.readZigZag32();
            maxChunkRadius = cursor.readUint8();
        }
    };
}
