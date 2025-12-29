#include <jerv/core/network_connection.hpp>
#include <jerv/protocol/packets/command_request.hpp>
#include <jerv/protocol/packets/server_script_debug_drawer.hpp>
#include <jerv/protocol/packets/toast_request.hpp>
#include <jerv/protocol/packets/set_time.hpp>
#include <jerv/protocol/packets/on_screen_texture_animation.hpp>

namespace jerv::core {
    void NetworkConnection::handleCommandRequest(binary::cursor &cursor) {
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
            try {
                protocol::OnScreenTextureAnimationPacket onScreenPacket;
                const std::string arg = packet.command.substr(6, packet.command.length());
                const int effect = std::stoi(arg);
                onScreenPacket.effectId = effect;

                this->send(onScreenPacket);
            } catch (...) {

            }
        }
    }
}
