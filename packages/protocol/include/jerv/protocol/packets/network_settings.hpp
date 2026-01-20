#pragma once

#include <jerv/protocol/packet.hpp>


namespace jerv::protocol {
    enum class CompressionAlgorithm : uint8_t {
        Zlib = 0,
        Snappy = 1,
        NoCompression = 0xFF
    };

    class NetworkSettingsPacket : public PacketType {
    public:
        uint16_t compressionThreshold = 1;
        CompressionAlgorithm compressionAlgorithm = CompressionAlgorithm::Zlib;
        bool clientThrottleEnabled = false;
        uint8_t clientThrottleThreshold = 0;
        float clientThrottleScalar = 0.0f;

        PacketId getPacketId() const override {
            return PacketId::NetworkSettings;
        }

        void serialize(binary::cursor &cursor) const override {
            cursor.writeUint16<true>(compressionThreshold);
            cursor.writeUint16<true>(static_cast<uint16_t>(compressionAlgorithm));
            cursor.writeUint8(clientThrottleEnabled ? 1 : 0);
            cursor.writeUint8(clientThrottleThreshold);
            cursor.writeFloat32<true>(clientThrottleScalar);
        }

        void deserialize(binary::cursor &cursor) override {
            compressionThreshold = cursor.readUint16<true>();
            compressionAlgorithm = static_cast<CompressionAlgorithm>(cursor.readUint16<true>());
            clientThrottleEnabled = cursor.readUint8() != 0;
            clientThrottleThreshold = cursor.readUint8();
            clientThrottleScalar = cursor.readFloat32<true>();
        }
    };
}
