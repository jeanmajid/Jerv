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
#include <vector>

namespace jerv::protocol {
    enum class ResourcePackResponse : uint8_t {
        None = 0,
        Refused = 1,
        SendPacks = 2,
        HaveAllPacks = 3,
        Completed = 4
    };

    struct RequestedResourcePack {
        std::string uuid;
        std::string version;

        void serialize(binary::Cursor &cursor) const {
            cursor.writeString(uuid + "_" + version);
        }

        void deserialize(binary::Cursor &cursor) {
            const std::string combined = cursor.readString();

            size_t underscorePos = combined.rfind('_');
            if (underscorePos != std::string::npos) {
                uuid = combined.substr(0, underscorePos);
                version = combined.substr(underscorePos + 1);
            } else {
                uuid = combined;
                version = "";
            }
        }
    };

    class ResourcePackClientResponsePacket : public PacketType {
    public:
        ResourcePackResponse response = ResourcePackResponse::None;
        std::vector<RequestedResourcePack> packs;

        static constexpr auto ID = PacketId::ResourcePackClientResponse;
        PacketId getPacketId() const override {
            return PacketId::ResourcePackClientResponse;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeUint8(static_cast<uint8_t>(response));
            cursor.writeUint16<true>(static_cast<uint16_t>(packs.size()));

            for (const auto &pack: packs) {
                pack.serialize(cursor);
            }
        }

        void deserialize(binary::Cursor &cursor) override {
            response = static_cast<ResourcePackResponse>(cursor.readUint8());
            uint16_t packCount = cursor.readUint16<true>();
            packs.clear();
            packs.reserve(packCount);

            for (uint16_t i = 0; i < packCount; i++) {
                RequestedResourcePack pack;
                pack.deserialize(cursor);
                packs.push_back(pack);
            }
        }
    };
}