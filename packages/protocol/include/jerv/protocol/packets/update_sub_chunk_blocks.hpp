#pragma once

#include <jerv/protocol/packet.hpp>
#include <jerv/protocol/enums.hpp>
#include <vector>
#include <cstdint>

namespace jerv::protocol {
    class UpdateSubChunkBlocksPacket : public PacketType {
    public:
        struct BlockChange {
            uint8_t bx = 0;
            uint8_t by = 0;
            uint8_t bz = 0;
            int32_t state = 0;

            void serialize(binary::Cursor &cursor) const {
                cursor.writeUint8(bx);
                cursor.writeUint8(by);
                cursor.writeUint8(bz);
                cursor.writeZigZag32(state);
            }

            void deserialize(binary::Cursor &cursor) {
                bx = cursor.readUint8();
                by = cursor.readUint8();
                bz = cursor.readUint8();
                state = cursor.readZigZag32();
            }
        };

        int32_t x = 0;
        int32_t z = 0;
        DimensionId dimension = DimensionId::Overworld;
        int8_t subChunkIndex = 0;

        std::vector<BlockChange> changes;

        PacketId getPacketId() const override {
            return PacketId::UpdateSubChunkBlocks;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeZigZag32(x);
            cursor.writeZigZag32(z);
            cursor.writeZigZag32(static_cast<int32_t>(dimension));
            cursor.writeUint8(static_cast<uint8_t>(subChunkIndex));

            cursor.writeVarInt32(static_cast<int32_t>(changes.size()));
            for (const auto &chg : changes) {
                chg.serialize(cursor);
            }
        }

        void deserialize(binary::Cursor &cursor) override {
            x = cursor.readZigZag32();
            z = cursor.readZigZag32();
            dimension = static_cast<DimensionId>(cursor.readZigZag32());
            subChunkIndex = static_cast<int8_t>(cursor.readUint8());

            const int32_t count = cursor.readVarInt32();
            changes.clear();
            changes.reserve(count);
            for (int32_t i = 0; i < count; i++) {
                BlockChange chg;
                chg.deserialize(cursor);
                changes.push_back(chg);
            }
        }
    };
}
