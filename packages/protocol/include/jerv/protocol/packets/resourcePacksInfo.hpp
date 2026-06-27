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
    struct ResourcePackDescriptor {
        std::string uuid;
        std::string version;
        uint64_t size = 0;
        std::string contentKey;
        std::string subpackName;
        std::string contentIdentity;
        bool hasScripts = false;
        bool isAddonPack = false;
        bool hasRtxCapabilities = false;
        std::string cdnUrl;

        void serialize(binary::Cursor &cursor) const {
            cursor.writeUuid(uuid);
            cursor.writeString(version);
            cursor.writeUint64<true>(size);
            cursor.writeString(contentKey);
            cursor.writeString(subpackName);
            cursor.writeString(contentIdentity);
            cursor.writeBool(hasScripts);
            cursor.writeBool(isAddonPack);
            cursor.writeBool(hasRtxCapabilities);
            cursor.writeString(cdnUrl);
        }

        void deserialize(binary::Cursor &cursor) {
            uuid = cursor.readUuid();
            version = cursor.readString();
            size = cursor.readUint64<true>();
            contentKey = cursor.readString();
            subpackName = cursor.readString();
            contentIdentity = cursor.readString();
            hasScripts = cursor.readBool();
            isAddonPack = cursor.readBool();
            hasRtxCapabilities = cursor.readBool();
            cdnUrl = cursor.readString();
        }
    };


    class ResourcePacksInfoPacket : public PacketType {
    public:
        bool mustAccept = false;
        bool hasAddons = false;
        bool hasScripts = false;
        bool forceDisableVibrantVisuals = false;
        std::string worldTemplateUuid = "00000000-0000-0000-0000-000000000000";
        std::string worldTemplateVersion;
        std::vector<ResourcePackDescriptor> packs;

        static constexpr auto ID = PacketId::ResourcePacksInfo;
        PacketId getPacketId() const override {
            return PacketId::ResourcePacksInfo;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeBool(mustAccept);
            cursor.writeBool(hasAddons);
            cursor.writeBool(hasScripts);
            cursor.writeBool(forceDisableVibrantVisuals);
            cursor.writeUuid(worldTemplateUuid);
            cursor.writeString(worldTemplateVersion);
            cursor.writeUint16<true>(static_cast<uint16_t>(packs.size()));
            for (const auto &pack: packs) {
                pack.serialize(cursor);
            }
        }

        void deserialize(binary::Cursor &cursor) override {
            mustAccept = cursor.readBool();
            hasAddons = cursor.readBool();
            hasScripts = cursor.readBool();
            forceDisableVibrantVisuals = cursor.readBool();
            worldTemplateUuid = cursor.readUuid();
            worldTemplateVersion = cursor.readString();
            const uint16_t packCount = cursor.readUint16<true>();
            packs.clear();
            packs.reserve(packCount);
            for (uint16_t i = 0; i < packCount; i++) {
                ResourcePackDescriptor pack;
                pack.deserialize(cursor);
                packs.push_back(pack);
            }
        }
    };
}