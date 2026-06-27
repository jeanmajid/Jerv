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
#include <string_view>

namespace jerv::protocol {
    class CommandRequestPacket : public PacketType {
    public:
        enum class CommandOriginType {
            Player = 0,
            DevConsole = 1,
            Test = 2,
            AutomationPlayer = 3
        };

        struct CommandOriginTypeHelper {
            static constexpr std::string_view to_string(const CommandOriginType v) {
                switch (v) {
                    case CommandOriginType::Player: return "player";
                    case CommandOriginType::DevConsole: return "devConsole";
                    case CommandOriginType::Test: return "test";
                    case CommandOriginType::AutomationPlayer: return "automationPlayer";
                    default: return "Unknown";
                }
            }

            static CommandOriginType from_string(const std::string_view s) {
                if (s == "player") return CommandOriginType::Player;
                if (s == "devConsole") return CommandOriginType::DevConsole;
                if (s == "test") return CommandOriginType::Test;
                if (s == "automationPlayer") return CommandOriginType::AutomationPlayer;
                // will have to figure out what the other strings are, bcs I have no idea
                throw std::runtime_error("Unknown CommandOriginType " + std::string(s));
            }
        };

        struct CommandOrigin {
            CommandOriginType type;
            std::string uuid;
            std::string requestId;
            int64_t playerEntityId;

            void deserialize(binary::Cursor &cursor) {
                type = CommandOriginTypeHelper::from_string(cursor.readString());
                uuid = cursor.readUuid();
                requestId = cursor.readString();

                if (type == CommandOriginType::DevConsole || type == CommandOriginType::Test) {
                    playerEntityId = cursor.readZigZag64();
                }
            }
        };

        std::string command;
        CommandOrigin origin;
        bool internal;
        std::string version;

        static constexpr auto ID = PacketId::CommandRequest;
        PacketId getPacketId() const override {
            return PacketId::CommandRequest;
        }

        void serialize(binary::Cursor &cursor) const override {
        }

        void deserialize(binary::Cursor &cursor) override {
            command = cursor.readString();
            origin.deserialize(cursor);
            internal = cursor.readBool();
            version = cursor.readString();
        }
    };
}
