#include <jerv/raknet/proto.hpp>
#include <jerv/raknet/constants.hpp>

namespace jerv::raknet::proto {
    CapsuleFrameData readCapsuleFrameData(std::span<uint8_t> data, size_t offset) {
        CapsuleFrameData result;

        const uint8_t flags = data[offset++];
        const uint8_t reliability = (flags & RELIABILITY_BIT_MASK) >> 5;
        const bool isFragmented = (flags & IS_FRAGMENTED_BIT) != 0;

        const uint16_t bodyLengthBits = (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
        offset += 2;
        const size_t bodyLength = (bodyLengthBits + 7) / 8;

        if (IS_RELIABLE_LOOKUP[reliability]) {
            result.reliableIndex = readUint24(data.data(), offset);
            offset += 3;
        }

        if (IS_SEQUENCED_LOOKUP[reliability]) {
            result.sequenceIndex = readUint24(data.data(), offset);
            offset += 3;
        }

        if (IS_ORDERED_LOOKUP[reliability]) {
            result.orderIndex = readUint24(data.data(), offset);
            offset += 3;
            result.orderChannel = data[offset++];
        }

        if (isFragmented) {
            auto *fragInfo = new FrameDescriptor::FragmentInfo();

            fragInfo->length = (static_cast<uint32_t>(data[offset]) << 24) |
                               (static_cast<uint32_t>(data[offset + 1]) << 16) |
                               (static_cast<uint32_t>(data[offset + 2]) << 8) |
                               static_cast<uint32_t>(data[offset + 3]);
            offset += 4;

            fragInfo->id = (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
            offset += 2;

            fragInfo->index = (static_cast<uint32_t>(data[offset]) << 24) |
                              (static_cast<uint32_t>(data[offset + 1]) << 16) |
                              (static_cast<uint32_t>(data[offset + 2]) << 8) |
                              static_cast<uint32_t>(data[offset + 3]);
            offset += 4;

            result.fragment = fragInfo;
        }

        result.body = data.subspan(offset, bodyLength);
        result.offset = offset + bodyLength;

        return result;
    }

    size_t writeCapsuleFrameHeader(
        size_t offset,
        uint8_t *buffer,
        const FrameDescriptor &frame,
        const size_t bodyLength,
        const uint8_t reliability
    ) {
        uint8_t flags = (reliability << 5);
        if (frame.fragment) {
            flags |= IS_FRAGMENTED_BIT;
        }
        buffer[offset++] = flags;

        const auto bodyLengthBits = static_cast<uint16_t>(bodyLength * 8);
        buffer[offset++] = static_cast<uint8_t>(bodyLengthBits >> 8);
        buffer[offset++] = static_cast<uint8_t>(bodyLengthBits);

        if (IS_RELIABLE_LOOKUP[reliability]) {
            writeUint24(buffer, offset, frame.reliableIndex);
            offset += 3;
        }

        if (IS_SEQUENCED_LOOKUP[reliability]) {
            writeUint24(buffer, offset, frame.sequenceIndex);
            offset += 3;
        }

        if (IS_ORDERED_LOOKUP[reliability]) {
            writeUint24(buffer, offset, frame.orderIndex);
            offset += 3;
            buffer[offset++] = frame.orderChannel;
        }

        if (frame.fragment) {
            buffer[offset++] = static_cast<uint8_t>(frame.fragment->length >> 24);
            buffer[offset++] = static_cast<uint8_t>(frame.fragment->length >> 16);
            buffer[offset++] = static_cast<uint8_t>(frame.fragment->length >> 8);
            buffer[offset++] = static_cast<uint8_t>(frame.fragment->length);

            buffer[offset++] = static_cast<uint8_t>(frame.fragment->id >> 8);
            buffer[offset++] = static_cast<uint8_t>(frame.fragment->id);

            buffer[offset++] = static_cast<uint8_t>(frame.fragment->index >> 24);
            buffer[offset++] = static_cast<uint8_t>(frame.fragment->index >> 16);
            buffer[offset++] = static_cast<uint8_t>(frame.fragment->index >> 8);
            buffer[offset++] = static_cast<uint8_t>(frame.fragment->index);
        }

        return offset;
    }

    std::vector<std::span<const uint8_t> > getChunks(std::span<const uint8_t> data, size_t chunkSize) {
        std::vector<std::span<const uint8_t> > chunks;

        size_t offset = 0;
        while (offset < data.size()) {
            size_t remaining = data.size() - offset;
            const size_t currentChunkSize = std::min(remaining, chunkSize);
            chunks.push_back(data.subspan(offset, currentChunkSize));
            offset += currentChunkSize;
        }

        return chunks;
    }
}