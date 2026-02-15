#pragma once

#include <jerv/protocol/packet.hpp>
#include <string>

namespace jerv::protocol {
    class ResourcePackDataInfoPacket : public PacketType {
    public:
        std::string resourceName;
        uint32_t chunkSize = 0;
        uint32_t numberOfChunks = 0;
        uint64_t fileSize = 0;
        std::string fileHash;
        bool isPremiumPack = false;
        uint8_t packType = 0;

        PacketId getPacketId() const override {
            return PacketId::ResourcePackDataInfo;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeString(resourceName);
            cursor.writeUint32<true>(chunkSize);
            cursor.writeUint32<true>(numberOfChunks);
            cursor.writeUint64<true>(fileSize);
            cursor.writeString(fileHash);
            cursor.writeBool(isPremiumPack);
            cursor.writeUint8(packType);
        }

        void deserialize(binary::Cursor &cursor) override {
            resourceName = cursor.readString();
            chunkSize = cursor.readUint32<true>();
            numberOfChunks = cursor.readUint32<true>();
            fileSize = cursor.readUint64<true>();
            fileHash = cursor.readString();
            isPremiumPack = cursor.readBool();
            packType = cursor.readUint8();
        }
    };
}