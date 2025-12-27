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

        void serialize(binary::cursor &cursor) const override {
            cursor.writeString(resourceName);
            cursor.writeUint32(chunkSize, true);
            cursor.writeUint32(numberOfChunks, true);
            cursor.writeBigUint64(fileSize, true);
            cursor.writeString(fileHash);
            cursor.writeBool(isPremiumPack);
            cursor.writeUint8(packType);
        }

        void deserialize(binary::cursor &cursor) override {
            resourceName = cursor.readString();
            chunkSize = cursor.readUint32(true);
            numberOfChunks = cursor.readUint32(true);
            fileSize = cursor.readBigUint64(true);
            fileHash = cursor.readString();
            isPremiumPack = cursor.readBool();
            packType = cursor.readUint8();
        }
    };
}