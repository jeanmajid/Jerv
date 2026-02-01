#pragma once

#include <jerv/protocol/packet.hpp>
#include <vector>
#include <cstdint>

namespace jerv::protocol {
    class CraftingDataPacket : public PacketType {
    public:
        std::vector<uint8_t> recipes;
        std::vector<uint8_t> potionMixData;
        std::vector<uint8_t> containerMixData;
        std::vector<uint8_t> materialReducers;
        bool clearRecipes = true;

        PacketId getPacketId() const override {
            return PacketId::CraftingData;
        }

        void serialize(binary::cursor &cursor) const override {
            cursor.writeVarInt32(0);
            cursor.writeVarInt32(0);
            cursor.writeVarInt32(0);
            cursor.writeVarInt32(0);
            cursor.writeBool(clearRecipes);
        }

        void deserialize(binary::cursor &cursor) override {
            int32_t recipeCount = cursor.readVarInt32();
            int32_t potionCount = cursor.readVarInt32();
            int32_t containerCount = cursor.readVarInt32();
            int32_t reducerCount = cursor.readVarInt32();

            clearRecipes = cursor.readBool();
        }
    };
}