#include <jerv/raknet/proto.hpp>
#include <jerv/raknet/constants.hpp>
#include <cstring>

namespace jerv::raknet::proto {
    uint32_t readUint24(const uint8_t *data, const size_t offset) {
        return static_cast<uint32_t>(data[offset]) |
               (static_cast<uint32_t>(data[offset + 1]) << 8) |
               (static_cast<uint32_t>(data[offset + 2]) << 16);
    }

    void writeUint24(uint8_t *data, const size_t offset, const uint32_t value) {
        data[offset] = static_cast<uint8_t>(value);
        data[offset + 1] = static_cast<uint8_t>(value >> 8);
        data[offset + 2] = static_cast<uint8_t>(value >> 16);
    }

    uint64_t getUnconnectedPingTime(const std::span<const uint8_t> data) {
        uint64_t value = 0;
        for (int i = 0; i < 8; i++) {
            value = (value << 8) | data[1 + i];
        }
        return value;
    }

    std::vector<uint8_t>
    rentUnconnectedPongBufferWith(const uint64_t pingTime, const uint64_t guid, const std::span<const uint8_t> motd) {
        std::vector<uint8_t> buffer(1 + 8 + 8 + 16 + 2 + motd.size());
        size_t offset = 0;

        buffer[offset++] = 0x1C;

        for (int i = 7; i >= 0; i--) {
            buffer[offset++] = static_cast<uint8_t>(pingTime >> (i * 8));
        }

        for (int i = 7; i >= 0; i--) {
            buffer[offset++] = static_cast<uint8_t>(guid >> (i * 8));
        }

        std::memcpy(buffer.data() + offset, MAGIC.data(), 16);
        offset += 16;

        buffer[offset++] = static_cast<uint8_t>(motd.size() >> 8);
        buffer[offset++] = static_cast<uint8_t>(motd.size());

        std::memcpy(buffer.data() + offset, motd.data(), motd.size());

        return buffer;
    }

    uint64_t getConnectedPingTime(std::span<const uint8_t> data) {
        uint64_t value = 0;
        for (int i = 0; i < 8; i++) {
            value = (value << 8) | data[1 + i];
        }
        return value;
    }

    std::vector<uint8_t> rentConnectedPongBufferWith(uint64_t pingTime, uint64_t pongTime) {
        std::vector<uint8_t> buffer(17);
        size_t offset = 0;

        buffer[offset++] = 0x03;

        for (int i = 7; i >= 0; i--) {
            buffer[offset++] = static_cast<uint8_t>(pingTime >> (i * 8));
        }

        for (int i = 7; i >= 0; i--) {
            buffer[offset++] = static_cast<uint8_t>(pongTime >> (i * 8));
        }

        return buffer;
    }
}