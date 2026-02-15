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