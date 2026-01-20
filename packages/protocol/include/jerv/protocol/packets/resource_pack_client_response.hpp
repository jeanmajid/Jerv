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

        void serialize(binary::cursor &cursor) const {
            cursor.writeString(uuid + "_" + version);
        }

        void deserialize(binary::cursor &cursor) {
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

        PacketId getPacketId() const override {
            return PacketId::ResourcePackClientResponse;
        }

        void serialize(binary::cursor &cursor) const override {
            cursor.writeUint8(static_cast<uint8_t>(response));
            cursor.writeUint16<true>(static_cast<uint16_t>(packs.size()));

            for (const auto &pack: packs) {
                pack.serialize(cursor);
            }
        }

        void deserialize(binary::cursor &cursor) override {
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