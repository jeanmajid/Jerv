#pragma once

#include <jerv/protocol/packet.hpp>
#include <string>

namespace jerv::protocol {
    class ToastRequestPacket : public PacketType {
    public:
        std::string title;
        std::string content;

        PacketId getPacketId() const override {
            return PacketId::ToastRequest;
        }

        void serialize(binary::cursor &cursor) const override {
            cursor.writeString(title);
            cursor.writeString(content);
        }

        void deserialize(binary::cursor &cursor) override {
            title = cursor.readString();
            content = cursor.readString();
        }
    };
}


