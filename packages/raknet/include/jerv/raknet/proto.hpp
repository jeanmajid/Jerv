#pragma once

#include <cstdint>
#include <span>
#include <vector>
#include <jerv/raknet/interfaces.hpp>

namespace jerv::raknet::proto {
    uint32_t readUint24(const uint8_t *data, size_t offset);
    void writeUint24(uint8_t *data, size_t offset, uint32_t value);
    uint64_t getUnconnectedPingTime(std::span<const uint8_t> data);
    std::vector<uint8_t> rentUnconnectedPongBufferWith(uint64_t pingTime, uint64_t guid, std::span<const uint8_t> motd);
    uint64_t getConnectedPingTime(std::span<const uint8_t> data);
    std::vector<uint8_t> rentConnectedPongBufferWith(uint64_t pingTime, uint64_t pongTime);

    struct ConnectionRequestInfo {
        uint64_t time;
        uint64_t guid;
    };

    ConnectionRequestInfo getConnectionRequestInfo(std::span<const uint8_t> data);

    struct OpenConnectionRequestTwoInfo {
        uint64_t guid;
        uint16_t mtu;
        AddressInfo serverAddress;
    };

    OpenConnectionRequestTwoInfo getOpenConnectionRequestTwoInfo(std::span<const uint8_t> data);

    std::vector<uint8_t> rentOpenConnectionReplyOneBufferWith(uint64_t guid, uint16_t mtu);

    std::vector<uint8_t> rentOpenConnectionReplyTwoBufferWith(uint64_t guid, const AddressInfo &clientAddress,
                                                              uint16_t mtu);

    std::vector<uint8_t> rentConnectionRequestAcceptPacketWith(
        const AddressInfo &clientAddress,
        const AddressInfo &serverAddress,
        uint64_t requestTime,
        uint64_t acceptTime
    );

    struct AckRange {
        uint32_t min;
        uint32_t max;
    };

    std::vector<AckRange> readACKLikePacket(std::span<const uint8_t> data);

    std::vector<uint8_t> rentAcknowledgePacketWith(uint8_t packetId,
                                                   const std::vector<std::pair<uint32_t, uint32_t> > &ranges);

    void writeAddress(uint8_t *data, size_t &offset, const AddressInfo &address);

    AddressInfo readAddress(const uint8_t *data, size_t &offset);

    struct CapsuleFrameData {
        std::span<uint8_t> body;
        FrameDescriptor::FragmentInfo *fragment = nullptr;
        uint8_t orderChannel = 0;
        uint32_t orderIndex = 0;
        uint32_t reliableIndex = 0;
        uint32_t sequenceIndex = 0;
        size_t offset = 0;
    };

    CapsuleFrameData readCapsuleFrameData(std::span<uint8_t> data, size_t offset);

    size_t writeCapsuleFrameHeader(
        size_t offset,
        uint8_t *buffer,
        const FrameDescriptor &frame,
        size_t bodyLength,
        uint8_t reliability
    );

    std::vector<std::span<const uint8_t> > getChunks(std::span<const uint8_t> data, size_t chunkSize);
}