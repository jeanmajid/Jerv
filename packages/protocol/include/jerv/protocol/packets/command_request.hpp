#pragma once

#include <jerv/protocol/packet.hpp>
#include <string_view>

#include "jerv/common/logger.hpp"

namespace jerv::protocol {
    class CommandRequestPacket : public PacketType {
    public:
        enum CommandOriginType {
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

            void deserialize(binary::cursor &cursor) {
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

        PacketId getPacketId() const override {
            return PacketId::CommandRequest;
        }

        void serialize(binary::cursor &cursor) const override {
        }

        void deserialize(binary::cursor &cursor) override {
            command = cursor.readString();
            origin.deserialize(cursor);
            internal = cursor.readBool();
            version = cursor.readString();
        }
    };
}
