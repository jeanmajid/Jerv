#pragma once

#include <jerv/protocol/packet.hpp>
#include <string>
#include <vector>

namespace jerv::protocol {
    struct NetworkItemInstanceDescriptor {
        int32_t networkId = 0;
        uint16_t stackSize = 0;
        int32_t metadata = 0;
        int32_t networkBlockId = 0;
        std::vector<uint8_t> extras;

        void serialize(binary::cursor &cursor) const {
            cursor.writeZigZag32(networkId);

            if (networkId == 0) return;

            cursor.writeUint16<true>(stackSize);
            cursor.writeVarInt(metadata);
            cursor.writeZigZag32(networkBlockId);
            cursor.writeVarInt(static_cast<int32_t>(extras.size()));
            if (!extras.empty()) {
                cursor.writeSliceSpan(extras);
            }
        }

        void deserialize(binary::cursor &cursor) {
            networkId = cursor.readZigZag32();

            if (networkId == 0) {
                stackSize = 0;
                metadata = 0;
                networkBlockId = 0;
                extras.clear();
                return;
            }

            stackSize = cursor.readUint16<true>();
            metadata = cursor.readVarInt();
            networkBlockId = cursor.readZigZag32();

            int32_t extrasLength = cursor.readVarInt();
            if (extrasLength > 0) {
                auto span = cursor.readSliceSpan(static_cast<size_t>(extrasLength));
                extras.assign(span.begin(), span.end());
            } else {
                extras.clear();
            }
        }
    };

    enum class CreativeItemCategory : int32_t {
        All = 0,
        Construction = 1,
        Nature = 2,
        Equipment = 3,
        Items = 4,
        ItemCommandOnly = 5,
        Undefined = 6
    };

    struct CreativeGroup {
        CreativeItemCategory category = CreativeItemCategory::All;
        std::string name;
        NetworkItemInstanceDescriptor icon;

        void serialize(binary::cursor &cursor) const {
            cursor.writeUint32<true>(static_cast<uint32_t>(category));
            cursor.writeString(name);
            icon.serialize(cursor);
        }

        void deserialize(binary::cursor &cursor) {
            category = static_cast<CreativeItemCategory>(cursor.readUint32<true>());
            name = cursor.readString();
            icon.deserialize(cursor);
        }
    };

    struct CreativeItem {
        int32_t itemIndex = 0;
        NetworkItemInstanceDescriptor itemInstance;
        int32_t groupIndex = 0;

        void serialize(binary::cursor &cursor) const {
            cursor.writeVarInt(itemIndex);
            itemInstance.serialize(cursor);
            cursor.writeVarInt(groupIndex);
        }

        void deserialize(binary::cursor &cursor) {
            itemIndex = cursor.readVarInt();
            itemInstance.deserialize(cursor);
            groupIndex = cursor.readVarInt();
        }
    };


    class CreativeContentPacket : public PacketType {
    public:
        std::vector<CreativeGroup> groups;
        std::vector<CreativeItem> items;

        PacketId getPacketId() const override {
            return PacketId::CreativeContent;
        }

        void serialize(binary::cursor &cursor) const override {
            cursor.writeVarInt(static_cast<int32_t>(groups.size()));
            for (const auto &group: groups) {
                group.serialize(cursor);
            }

            cursor.writeVarInt(static_cast<int32_t>(items.size()));
            for (const auto &item: items) {
                item.serialize(cursor);
            }
        }

        void deserialize(binary::cursor &cursor) override {
            const int32_t groupCount = cursor.readVarInt();
            groups.clear();
            groups.reserve(groupCount);
            for (int32_t i = 0; i < groupCount; i++) {
                CreativeGroup group;
                group.deserialize(cursor);
                groups.push_back(group);
            }

            const int32_t itemCount = cursor.readVarInt();
            items.clear();
            items.reserve(itemCount);
            for (int32_t i = 0; i < itemCount; i++) {
                CreativeItem item;
                item.deserialize(cursor);
                items.push_back(item);
            }
        }
    };
}