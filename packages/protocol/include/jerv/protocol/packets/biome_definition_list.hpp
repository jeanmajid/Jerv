#pragma once

#include <jerv/protocol/packet.hpp>
#include <string>
#include <vector>

namespace jerv::protocol {
    struct Color {
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
        uint8_t alpha = 255;

        void serialize(binary::cursor &cursor) const {
            cursor.writeUint8(red);
            cursor.writeUint8(green);
            cursor.writeUint8(blue);
            cursor.writeUint8(alpha);
        }

        void deserialize(binary::cursor &cursor) {
            red = cursor.readUint8();
            green = cursor.readUint8();
            blue = cursor.readUint8();
            alpha = cursor.readUint8();
        }
    };

    struct BiomeDefinitionData {
        uint16_t identifier = 0;
        float temperature = 0.0f;
        float downfall = 0.0f;
        float snowFoliage = 0.0f;
        float depth = 0.0f;
        float scale = 0.0f;
        Color waterColor;
        bool canRain = true;
        std::vector<uint16_t> tagIndices;
        bool hasClientSidedChunkGeneration = false;

        void serialize(binary::cursor &cursor) const {
            cursor.writeUint16<true>(identifier);
            cursor.writeFloat32<true>(temperature);
            cursor.writeFloat32<true>(downfall);
            cursor.writeFloat32<true>(snowFoliage);
            cursor.writeFloat32<true>(depth);
            cursor.writeFloat32<true>(scale);
            waterColor.serialize(cursor);
            cursor.writeBool(canRain);

            if (!tagIndices.empty()) {
                cursor.writeBool(true);
                cursor.writeVarInt(static_cast<int32_t>(tagIndices.size()));
                for (const uint16_t tagIndex: tagIndices) {
                    cursor.writeUint16<true>(tagIndex);
                }
            } else {
                cursor.writeBool(false);
            }

            cursor.writeBool(hasClientSidedChunkGeneration);
        }

        void deserialize(binary::cursor &cursor) {
            identifier = cursor.readUint16<true>();
            temperature = cursor.readFloat32<true>();
            downfall = cursor.readFloat32<true>();
            snowFoliage = cursor.readFloat32<true>();
            depth = cursor.readFloat32<true>();
            scale = cursor.readFloat32<true>();
            waterColor.deserialize(cursor);
            canRain = cursor.readBool();

            tagIndices.clear();
            if (cursor.readBool()) {
                int32_t tagCount = cursor.readVarInt();
                tagIndices.reserve(tagCount);
                for (int32_t i = 0; i < tagCount; i++) {
                    tagIndices.push_back(cursor.readUint16<true>());
                }
            }

            hasClientSidedChunkGeneration = cursor.readBool();
        }
    };


    struct BiomeDefinitionEntry {
        uint16_t identifierIndex = 0;
        BiomeDefinitionData definition;

        void serialize(binary::cursor &cursor) const {
            cursor.writeUint16<true>(identifierIndex);
            definition.serialize(cursor);
        }

        void deserialize(binary::cursor &cursor) {
            identifierIndex = cursor.readUint16<true>();
            definition.deserialize(cursor);
        }
    };

    class BiomeDefinitionListPacket : public PacketType {
    public:
        std::vector<BiomeDefinitionEntry> definitions;
        std::vector<std::string> identifiers;

        PacketId getPacketId() const override {
            return PacketId::BiomeDefinitionList;
        }

        void serialize(binary::cursor &cursor) const override {
            cursor.writeVarInt(static_cast<int32_t>(definitions.size()));
            for (const auto &def: definitions) {
                def.serialize(cursor);
            }

            cursor.writeVarInt(static_cast<int32_t>(identifiers.size()));
            for (const auto &id: identifiers) {
                cursor.writeString(id);
            }
        }

        void deserialize(binary::cursor &cursor) override {
            int32_t defCount = cursor.readVarInt();
            definitions.clear();
            definitions.reserve(defCount);
            for (int32_t i = 0; i < defCount; i++) {
                BiomeDefinitionEntry entry;
                entry.deserialize(cursor);
                definitions.push_back(entry);
            }

            int32_t idCount = cursor.readVarInt();
            identifiers.clear();
            identifiers.reserve(idCount);
            for (int32_t i = 0; i < idCount; i++) {
                identifiers.push_back(cursor.readString());
            }
        }
    };
}