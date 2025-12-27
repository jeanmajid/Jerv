#pragma once

#include <jerv/protocol/packet.hpp>
#include <string>
#include <vector>

namespace jerv::protocol {
    struct ItemData {
        std::string identifier;
        int16_t networkId = 0;
        bool isComponentBased = false;
        int32_t itemVersion = 0;
        std::vector<uint8_t> propertiesNbt;

        void serialize(binary::cursor &cursor) const {
            cursor.writeString(identifier);
            cursor.writeUint16(static_cast<uint16_t>(networkId), true);
            cursor.writeBool(isComponentBased);
            cursor.writeZigZag32(itemVersion);

            if (propertiesNbt.empty()) {
                cursor.writeUint8(0x0A);
                cursor.writeUint8(0x00);
                cursor.writeUint8(0x00);
            } else {
                cursor.writeSliceSpan(propertiesNbt);
            }
        }

        void deserialize(binary::cursor &cursor) {
            identifier = cursor.readString();
            networkId = static_cast<int16_t>(cursor.readUint16(true));
            isComponentBased = cursor.readBool();
            itemVersion = cursor.readZigZag32();
            propertiesNbt.clear();
        }
    };


    class ItemRegistryPacket : public PacketType {
    public:
        std::vector<ItemData> definitions;

        PacketId getPacketId() const override {
            return PacketId::ItemRegistry;
        }

        void serialize(binary::cursor &cursor) const override {
            cursor.writeVarInt(static_cast<int32_t>(definitions.size()));
            for (const auto &item: definitions) {
                item.serialize(cursor);
            }
        }

        void deserialize(binary::cursor &cursor) override {
            const int32_t count = cursor.readVarInt();
            definitions.clear();
            definitions.reserve(count);
            for (int32_t i = 0; i < count; i++) {
                ItemData item;
                item.deserialize(cursor);
                definitions.push_back(item);
            }
        }
    };
}