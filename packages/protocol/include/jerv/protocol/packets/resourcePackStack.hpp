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
    struct ResourceIdVersion {
        std::string uuid;
        std::string version;
        std::string name;

        void serialize(binary::Cursor &cursor) const {
            cursor.writeString(uuid);
            cursor.writeString(version);
            cursor.writeString(name);
        }

        void deserialize(binary::Cursor &cursor) {
            uuid = cursor.readString();
            version = cursor.readString();
            name = cursor.readString();
        }
    };


    struct Experiment {
        std::string name;
        bool enabled = false;

        void serialize(binary::Cursor &cursor) const {
            cursor.writeString(name);
            cursor.writeBool(enabled);
        }

        void deserialize(binary::Cursor &cursor) {
            name = cursor.readString();
            enabled = cursor.readBool();
        }
    };


    class ResourcePackStackPacket : public PacketType {
    public:
        bool mustAccept = false;
        std::vector<ResourceIdVersion> texturePacks;
        std::string gameVersion = "*";
        std::vector<Experiment> experiments;
        bool experimentsPreviouslyToggled = false;
        bool hasEditorPacks = false;

        static constexpr auto ID = PacketId::ResourcePackStack;
        PacketId getPacketId() const override {
            return PacketId::ResourcePackStack;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeBool(mustAccept);

            cursor.writeVarInt32(texturePacks.size());
            for (const auto &pack: texturePacks) {
                pack.serialize(cursor);
            }

            cursor.writeString(gameVersion);

            cursor.writeUint32<true>(experiments.size());
            for (const auto &exp: experiments) {
                exp.serialize(cursor);
            }

            cursor.writeBool(experimentsPreviouslyToggled);
            cursor.writeBool(hasEditorPacks);
        }

        void deserialize(binary::Cursor &cursor) override {
            mustAccept = cursor.readBool();

            const int32_t textureCount = cursor.readVarInt32();
            texturePacks.clear();
            texturePacks.reserve(textureCount);
            for (int32_t i = 0; i < textureCount; i++) {
                ResourceIdVersion pack;
                pack.deserialize(cursor);
                texturePacks.push_back(pack);
            }

            gameVersion = cursor.readString();
            const uint32_t expCount = cursor.readUint32<true>();
            experiments.clear();
            experiments.reserve(expCount);
            for (uint32_t i = 0; i < expCount; i++) {
                Experiment exp;
                exp.deserialize(cursor);
                experiments.push_back(exp);
            }

            experimentsPreviouslyToggled = cursor.readBool();
            hasEditorPacks = cursor.readBool();
        }
    };
}