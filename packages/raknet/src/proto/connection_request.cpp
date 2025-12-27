#include <jerv/raknet/proto.hpp>
#include <jerv/raknet/constants.hpp>
#include <cstring>

namespace jerv::raknet::proto {
    ConnectionRequestInfo getConnectionRequestInfo(std::span<const uint8_t> data) {
        ConnectionRequestInfo info;
        size_t offset = 1;

        info.guid = 0;
        for (int i = 0; i < 8; i++) {
            info.guid = (info.guid << 8) | data[offset++];
        }

        info.time = 0;
        for (int i = 0; i < 8; i++) {
            info.time = (info.time << 8) | data[offset++];
        }

        return info;
    }

    OpenConnectionRequestTwoInfo getOpenConnectionRequestTwoInfo(std::span<const uint8_t> data) {
        OpenConnectionRequestTwoInfo info;
        size_t offset = 1 + 16;

        info.serverAddress = readAddress(data.data(), offset);

        info.mtu = (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
        offset += 2;

        info.guid = 0;
        for (int i = 0; i < 8; i++) {
            info.guid = (info.guid << 8) | data[offset++];
        }

        return info;
    }

    std::vector<uint8_t> rentOpenConnectionReplyOneBufferWith(uint64_t guid, uint16_t mtu) {
        std::vector<uint8_t> buffer(28);
        size_t offset = 0;

        buffer[offset++] = 0x06;

        std::memcpy(buffer.data() + offset, MAGIC.data(), 16);
        offset += 16;

        for (int i = 7; i >= 0; i--) {
            buffer[offset++] = static_cast<uint8_t>(guid >> (i * 8));
        }

        buffer[offset++] = 0;
        buffer[offset++] = static_cast<uint8_t>(mtu >> 8);
        buffer[offset++] = static_cast<uint8_t>(mtu);

        return buffer;
    }

    std::vector<uint8_t> rentOpenConnectionReplyTwoBufferWith(const uint64_t guid, const AddressInfo &clientAddress,
                                                              const uint16_t mtu) {
        std::vector<uint8_t> buffer(35);
        size_t offset = 0;

        buffer[offset++] = 0x08;
        std::memcpy(buffer.data() + offset, MAGIC.data(), 16);
        offset += 16;

        for (int i = 7; i >= 0; i--) {
            buffer[offset++] = static_cast<uint8_t>(guid >> (i * 8));
        }

        writeAddress(buffer.data(), offset, clientAddress);

        buffer[offset++] = static_cast<uint8_t>(mtu >> 8);
        buffer[offset++] = static_cast<uint8_t>(mtu);

        buffer[offset++] = 0;
        buffer.resize(offset);
        return buffer;
    }

    std::vector<uint8_t> rentConnectionRequestAcceptPacketWith(
        const AddressInfo &clientAddress,
        const AddressInfo &serverAddress,
        const uint64_t requestTime,
        const uint64_t acceptTime
    ) {
        std::vector<uint8_t> buffer(256);
        size_t offset = 0;

        buffer[offset++] = 0x10;

        writeAddress(buffer.data(), offset, clientAddress);

        buffer[offset++] = 0;
        buffer[offset++] = 0;

        AddressInfo localAddr{"127.0.0.1", 0, "IPv4"};
        for (int i = 0; i < 10; i++) {
            writeAddress(buffer.data(), offset, localAddr);
        }

        for (int i = 7; i >= 0; i--) {
            buffer[offset++] = static_cast<uint8_t>(requestTime >> (i * 8));
        }

        for (int i = 7; i >= 0; i--) {
            buffer[offset++] = static_cast<uint8_t>(acceptTime >> (i * 8));
        }

        buffer.resize(offset);
        return buffer;
    }
}