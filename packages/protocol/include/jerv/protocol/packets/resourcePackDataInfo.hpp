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
#include <string>

namespace jerv::protocol {
    class ResourcePackDataInfoPacket : public PacketType {
    public:
        std::string resourceName;
        uint32_t chunkSize = 0;
        uint32_t numberOfChunks = 0;
        uint64_t fileSize = 0;
        std::string fileHash;
        bool isPremiumPack = false;
        uint8_t packType = 0;

        static constexpr auto ID = PacketId::ResourcePackDataInfo;
        PacketId getPacketId() const override {
            return PacketId::ResourcePackDataInfo;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeString(resourceName);
            cursor.writeUint32<true>(chunkSize);
            cursor.writeUint32<true>(numberOfChunks);
            cursor.writeUint64<true>(fileSize);
            cursor.writeString(fileHash);
            cursor.writeBool(isPremiumPack);
            cursor.writeUint8(packType);
        }

        void deserialize(binary::Cursor &cursor) override {
            resourceName = cursor.readString();
            chunkSize = cursor.readUint32<true>();
            numberOfChunks = cursor.readUint32<true>();
            fileSize = cursor.readUint64<true>();
            fileHash = cursor.readString();
            isPremiumPack = cursor.readBool();
            packType = cursor.readUint8();
        }
    };
}