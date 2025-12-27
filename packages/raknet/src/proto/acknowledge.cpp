#include <jerv/raknet/proto.hpp>
#include <jerv/raknet/constants.hpp>

namespace jerv::raknet::proto {
    std::vector<AckRange> readACKLikePacket(std::span<const uint8_t> data) {
        std::vector<AckRange> ranges;

        size_t offset = 1;
        const uint16_t rangeCount = (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
        offset += 2;

        for (uint16_t i = 0; i < rangeCount; i++) {
            const bool isSingle = data[offset++] != 0;

            const uint32_t min = readUint24(data.data(), offset);
            offset += 3;

            uint32_t max = min;
            if (!isSingle) {
                max = readUint24(data.data(), offset);
                offset += 3;
            }

            ranges.push_back({min, max});
        }

        return ranges;
    }

    std::vector<uint8_t> rentAcknowledgePacketWith(const uint8_t packetId,
                                                   const std::vector<std::pair<uint32_t, uint32_t> > &ranges) {
        size_t size = 1 + 2;
        for (const auto &range: ranges) {
            size += 1 + 3;
            if (range.first != range.second) {
                size += 3;
            }
        }

        std::vector<uint8_t> buffer(size);
        size_t offset = 0;

        buffer[offset++] = packetId;


        const auto rangeCount = static_cast<uint16_t>(ranges.size());
        buffer[offset++] = static_cast<uint8_t>(rangeCount >> 8);
        buffer[offset++] = static_cast<uint8_t>(rangeCount);

        for (const auto &range: ranges) {
            bool isSingle = (range.first == range.second);
            buffer[offset++] = isSingle ? 1 : 0;

            writeUint24(buffer.data(), offset, range.first);
            offset += 3;

            if (!isSingle) {
                writeUint24(buffer.data(), offset, range.second);
                offset += 3;
            }
        }

        return buffer;
    }
}