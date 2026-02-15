#include <jerv/core/network_connection.hpp>
#include <jerv/protocol/packets/command_request.hpp>
#include <jerv/protocol/packets/server_script_debug_drawer.hpp>
#include <jerv/protocol/packets/toast_request.hpp>
#include <jerv/protocol/packets/set_time.hpp>
#include <jerv/protocol/packets/ddui_show_screen_packet.hpp>
#include <jerv/protocol/packets/move_player_packet.hpp>

#include "jerv/protocol/packets/set_actor_data.hpp"

namespace jerv::core {
    void NetworkConnection::handleCommandRequest(binary::Cursor &cursor) {
        protocol::CommandRequestPacket packet;
        packet.deserialize(cursor);

        if (packet.command.starts_with("/test ")) {
            const std::string arg = packet.command.substr(6, packet.command.length());

            protocol::ToastRequestPacket toast;
            toast.title = "Test Toast";
            toast.content = arg;

            this->send(toast);
        } else if (packet.command.starts_with("/draw")) {
            protocol::ServerScriptDebugDrawerPacket drawPacket;

            protocol::ServerScriptDebugDrawerPacket::Shape shape;
            shape.runtimeId = 1;

            shape.type = protocol::ServerScriptDebugDrawerPacket::ShapeType::Line;

            shape.location = {0.0f, 100.0f, 0.0f};
            shape.lineEndLocation = {10.0f, 100.0f, 10.0f};

            shape.color = {1, 1, 1, 1};

            drawPacket.shapes.push_back(shape);
            this->send(drawPacket);
        } else if (packet.command.starts_with("/time")) {
            try {
                protocol::SetTimePacket timePacket;
                const std::string arg = packet.command.substr(6, packet.command.length());
                const int time = std::stoi(arg);
                timePacket.time = time;

                this->send(timePacket);
            } catch (...) {

            }
        } else if (packet.command.starts_with("/osta")) {
            protocol::ClientboundDataDrivenUIShowScreenPacket ddui;
            ddui.screenId = "minecraft:chest_screen";

            this->send(ddui);
        } else if (packet.command.starts_with("/tp")) {
            try {
                protocol::MovePlayerPacket tpPacket;

                const std::string arg = packet.command.substr(4, packet.command.length());
                std::istringstream iss(arg);
                float x, y, z;
                iss >> x >> y >> z;

                tpPacket.runtimeId = 1;
                tpPacket.position = { x, y, z };
                tpPacket.mode = protocol::MovePlayerPacket::MovePlayerMode::Teleport;

                this->send(tpPacket);
            } catch (...) {

            }
        } else if (packet.command.starts_with("/scale")) {
            const std::string arg = packet.command.substr(7, packet.command.length());
            const float size = std::stof(arg);

            protocol::SetActorDataPacket::MetaDataDictionary scale;
            scale.key = 38;
            scale.type = protocol::SetActorDataPacket::MetaDataDictionaryType::Float;
            scale.value = size;

            protocol::SetActorDataPacket setActorData;
            setActorData.runtimeEntityId = 1;
            setActorData.metaData = {scale};

            this->send(setActorData);
        }
    }
}
