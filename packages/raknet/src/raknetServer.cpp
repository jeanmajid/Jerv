#include "jerv/raknet/raknetServer.hpp"

#include <iostream>
#include <random>
#include <ranges>

#include "jerv/binary/nbt.hpp"
#include "jerv/common/logger.hpp"
#include "jerv/raknet/constants.hpp"
#include "jerv/raknet/frameCapsule.hpp"
#include "jerv/raknet/protocol/packetIds.hpp"
#include "jerv/raknet/protocol/packets/connectionRequest.hpp"
#include "jerv/raknet/protocol/packets/connectionRequestAccepted.hpp"
#include "jerv/raknet/protocol/packets/openConnectionReply1.hpp"
#include "jerv/raknet/protocol/packets/openConnectionReply2Packet.hpp"
#include "jerv/raknet/protocol/packets/openConnectionRequest1.hpp"
#include "jerv/raknet/protocol/packets/openConnectionRequest2.hpp"
#include "jerv/raknet/protocol/packets/unconnectPing.hpp"
#include "jerv/raknet/protocol/packets/unconnectPong.hpp"

namespace jerv::raknet {
    void RaknetServer::bindV4(uint16_t port, const std::string &address) {
        socket4 = std::make_unique<asio::ip::udp::socket>(_ioContext);
        socket4->open(asio::ip::udp::v4());

#ifdef _WIN32
        constexpr BOOL opt = TRUE;
        setsockopt(
            socket4->native_handle(),
            IPPROTO_IP,
            IP_DONTFRAGMENT,
            reinterpret_cast<const char *>(&opt),
            sizeof(opt)
        );
#endif

        socket4->bind(asio::ip::udp::endpoint(asio::ip::make_address(address), port));

        JERV_LOG_INFO("listening on {}:{}", address, port);

        startReceive(*socket4);
    }

    void RaknetServer::bindV6() {
    }

    void RaknetServer::start() {
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution dist(
            std::numeric_limits<int64_t>::min(),
            std::numeric_limits<int64_t>::max()
        );
        serverGuid = -dist(gen);

        _ioContext.run();
    }

    template<size_t BufferSize>
    void RaknetServer::sendPacket(const asio::ip::udp::endpoint &endpoint, const RaknetBasePacket &packet) {
        std::array<uint8_t, BufferSize + 1> data;
        binary::Cursor cursor(data);
        cursor.writeUint8(static_cast<uint8_t>(packet.getPacketId()));
        packet.serialize(cursor);

        socket4->send_to(asio::buffer(data, cursor.processedBytesSize()),
                         endpoint);
    }

    void RaknetServer::startReceive(asio::ip::udp::socket &socket) {
        socket.async_receive_from(
            asio::buffer(receiveBuffer),
            remoteEndpoint,
            [&socket, this](const asio::error_code &error, const size_t bytesReceived) {
                if (!error && bytesReceived > 0) {
                    const std::span data(receiveBuffer.data(), bytesReceived);
                    onMessage(remoteEndpoint, data);
                }

                startReceive(socket);
            });
    }

    void RaknetServer::onMessage(const asio::ip::udp::endpoint &endpoint, const std::span<uint8_t> data) {
        binary::Cursor cursor(data);
        const uint8_t packetId = cursor.readUint8();
        if (packetId != 1) {
            JERV_LOG_DEBUG("packet id: {}", packetId);
        }

        if (packetId < 0x80) {
            handleOffline(endpoint, packetId, cursor);
        } else {
            handleOnline(endpoint, cursor);
        }
    }

    void RaknetServer::handleOnline(const asio::ip::udp::endpoint &endpoint,
                                    binary::Cursor &cursor) {
        cursor.reset();
        const uint8_t firstByte = cursor.buffer()[0];
        const auto it = connections.find(endpointToString(endpoint));
        if (it == connections.end()) {
            return;
        }
        ServerConnection connection = it->second;
        connection.incomingLastActivity = std::chrono::steady_clock::now();

        const uint8_t mask = firstByte & ONLINE_DATAGRAM_BIT_MASK;
        if (mask == VALID_DATAGRAM_BIT) {
            handleFrameSet(connection, cursor);
            return;
        }

        if ((mask & ACK_DATAGRAM_BIT) == ACK_DATAGRAM_BIT) {
            handleAck(connection, cursor);
            return;
        }

        if ((mask & NACK_DATAGRAM_BIT) == NACK_DATAGRAM_BIT) {
            handleNack(connection, cursor);
            return;
        }
    }

    void RaknetServer::handleOffline(const asio::ip::udp::endpoint &endpoint, const uint8_t packetId,
                                     binary::Cursor &cursor) {
        switch (static_cast<RaknetPacketId>(packetId)) {
            case RaknetPacketId::UnconnectPing: {
                UnconnectPingPacket unconnectPingPacket;
                unconnectPingPacket.deserialize(cursor);

                UnconnectPongPacket unconnectPongPacket;
                unconnectPongPacket.clientAliveTimeMs = unconnectPingPacket.clientAliveTimeMs;
                unconnectPongPacket.serverGuid = serverGuid;
                unconnectPongPacket.motd = "MCPE;JERVER;975;1.0.0;100;200;" + std::to_string(serverGuid) +
                                           ";JERVER;Survival;1;";

                sendPacket<34 + 69>(endpoint, unconnectPongPacket);
                break;
            }
            case RaknetPacketId::OpenConnectionRequest1: {
                OpenConnectionRequest1Packet connectionRequest1;
                connectionRequest1.deserialize(cursor);

                OpenConnectionReply1Packet connectionReply1;
                connectionReply1.serverGuid = serverGuid;
                connectionReply1.serverHasSecurity = false;
                connectionReply1.mtuSize = IDEAL_MAX_MTU_SIZE;

                sendPacket<31>(endpoint, connectionReply1);
                break;
            }
            case RaknetPacketId::OpenConnectionRequest2: {
                OpenConnectionRequest2Packet connectionRequest2;
                connectionRequest2.deserialize(cursor);

                OpenConnectionReply2 connectionReply2;
                connectionReply2.serverGuid = serverGuid;
                connectionReply2.clientAddress = {
                    .family = static_cast<uint8_t>(endpoint.protocol().family() == 2 ? 4 : 6),
                    .ip = endpoint.address().to_v4().to_uint(),
                    .port = endpoint.port()
                };

                sendPacket<34>(endpoint, connectionReply2);

                connections.try_emplace(
                    endpointToString(endpoint),
                    endpoint,
                    socket4.get()
                );
                break;
            }
            default: {
                JERV_LOG_DEBUG("Unhandled raknet packet id: {}", packetId);
                break;
            };
        }
    }

    void RaknetServer::handleFrameSet(ServerConnection &connection, binary::Cursor &cursor) {
        uint32_t sequenceId = cursor.readUint24<true>();

        if (sequenceId > 0xFFFFE0) {
            // TODO DISCONNECT
            return;
        }

        // const uint32_t frameIndex = sequenceId & 0xFF;
        // const uint32_t correctionIndex = (sequenceId + 512) & 0xFF;

        // connection.incomingReceivedDatagram.erase(correctionIndex);
        //
        // // TODO: can get rid of this
        // if (connection.incomingReceivedDatagram[frameIndex]) {
        //     JERV_LOG_DEBUG("Duplicate Frame");
        // }
        //
        // connection.incomingReceivedDatagram[frameIndex] = true;
        connection.incomingMissingDatagram.erase(sequenceId);

        if (connection.incomingLastDatagramId != static_cast<uint32_t>(-1) && sequenceId - connection.
            incomingLastDatagramId > 1) {
            for (uint32_t i = connection.incomingLastDatagramId + 1; i < sequenceId; i++) {
                connection.incomingMissingDatagram.insert(i);
            }
        }

        connection.incomingReceivedDatagramAcknowledgeStack.push_back(sequenceId);
        connection.incomingLastDatagramId = sequenceId;

        while (cursor.pointer() < cursor.buffer().size()) {
            handleCapsule(connection, cursor);
        }

        if (!connection.incomingReceivedDatagramAcknowledgeStack.empty()) {
            const auto ranges = getRangesFromSequence(connection.incomingReceivedDatagramAcknowledgeStack);
            sendAck(connection, RaknetPacketId::AckDatagram, ranges);
            connection.incomingReceivedDatagramAcknowledgeStack.clear();
        }

        if (!connection.incomingMissingDatagram.empty()) {
            const std::vector missing(connection.incomingMissingDatagram.begin(),
                                      connection.incomingMissingDatagram.end());
            const auto ranges = getRangesFromSequence(missing);
            sendAck(connection, RaknetPacketId::NackDatagram, ranges);
            connection.incomingMissingDatagram.clear();
        }
    }

    std::vector<std::pair<uint32_t, uint32_t> > RaknetServer::getRangesFromSequence(const std::vector<uint32_t> &sequence) {
        std::vector<std::pair<uint32_t, uint32_t> > ranges;
        if (!sequence.empty()) {
            uint32_t min = sequence[0];
            uint32_t max = min;

            for (size_t i = 1; i < sequence.size(); i++) {
                uint32_t current = sequence[i];
                if (current == max + 1) {
                    max = current;
                } else {
                    ranges.emplace_back(min, max);
                    min = max = current;
                }
            }
            ranges.emplace_back(min, max);
        };

        return ranges;
    }

    void RaknetServer::sendAck(ServerConnection &connection, RaknetPacketId type,
                               const std::vector<std::pair<uint32_t, uint32_t> > &ranges) {
        size_t size = 1 + 2;
        for (const auto &range: ranges) {
            size += 1 + 3;
            if (range.first != range.second) {
                size += 3;
            }
        }

        std::vector<uint8_t> buffer(size);
        binary::Cursor cursor(buffer);

        cursor.writeUint8(static_cast<uint8_t>(type));
        cursor.writeUint16(ranges.size());

        for (const auto &range: ranges) {
            bool isSingle = (range.first == range.second);
            cursor.writeUint8(isSingle ? 1 : 0);
            cursor.writeUint24(range.first);

            if (!isSingle) {
                cursor.writeUint24(range.second);
            }
        }

        connection.send(buffer);
    }

    FrameCapsule RaknetServer::handleCapsule(ServerConnection &connection, binary::Cursor &cursor) {
        FrameCapsule capsule;
        const uint8_t flags = cursor.readUint8();
        const uint8_t reliability = (flags & RELIABILITY_BIT_MASK) >> 5;
        const bool isFragmented = (flags & IS_FRAGMENTED_BIT) != 0;

        const uint16_t bodyLengthBits = cursor.readUint16();
        const size_t bodyLength = (bodyLengthBits + 7) / 8;

        if (IS_RELIABLE_LOOKUP[reliability]) {
            capsule.reliableIndex = cursor.readUint24<true>();
        }

        if (IS_SEQUENCED_LOOKUP[reliability]) {
            capsule.sequenceIndex = cursor.readUint24<true>();
        }

        if (IS_ORDERED_LOOKUP[reliability]) {
            capsule.orderIndex = cursor.readUint24<true>();
            capsule.orderChannel = cursor.readUint8();
        }

        if (isFragmented) {
            FragmentInfo fragmentInfo;
            fragmentInfo.length = cursor.readUint32();
            fragmentInfo.id = cursor.readUint16();
            fragmentInfo.index = cursor.readUint32();
            capsule.fragment = fragmentInfo;
            capsule.hasFragment = true;
        }

        capsule.body = cursor.readSliceSpan(bodyLength);
        capsule.offset = cursor.pointer() + bodyLength;

        if (capsule.hasFragment) {
            handleFragment(connection, capsule);
        } else {
            handleFrame(connection, capsule.body);
        }
    }

    void RaknetServer::handleFragment(ServerConnection &connection, const FrameCapsule &capsule) {
        auto &meta = connection.incomingFragmentRebuildTable[capsule.fragment.id];
        meta.set(capsule.fragment.index, capsule.body);

        if (meta.length() >= capsule.fragment.length) {
            auto rebuilt = meta.build();
            connection.incomingFragmentRebuildTable.erase(capsule.fragment.id);

            handleFrame(connection, rebuilt);
        }
    }

    void RaknetServer::handleAck(const ServerConnection &connection, binary::Cursor &cursor) {
        // skip packet id
        cursor.setPointer(1);
        uint16_t rangeCount = cursor.readUint16();

        for (uint16_t i = 0; i < rangeCount; ++i) {
            bool isSingle = cursor.readBool();
            uint32_t min = cursor.readUint24<true>();
            uint32_t max = min;
            if (!isSingle) {
                max = cursor.readUint24<true>();
            }

            for (uint32_t i = min; i <= max; i++) {
                auto it = connection.outgoingUnacknowledgedCache.find(i);
                if (it != connection.outgoingUnacknowledgedCache.end()) {
                    connection.outgoingUnacknowledgedReliableCapsules -= it->second.size();
                    connection.outgoingUnacknowledgedCache.erase(it);
                }
            }
        }
    }

    void RaknetServer::handleNack(const ServerConnection &connection, binary::Cursor &cursor) {
        // skip packet id
        cursor.setPointer(1);
        uint16_t rangeCount = cursor.readUint16();

        for (uint16_t i = 0; i < rangeCount; ++i) {
            bool isSingle = cursor.readBool();
            uint32_t min = cursor.readUint24<true>();
            uint32_t max = min;
            if (!isSingle) {
                max = cursor.readUint24<true>();
            }

            for (int32_t i = max; i >= min; --i) {
                auto it = connection.outgoingUnacknowledgedCache.find(i);
                if (it != connection.outgoingUnacknowledgedCache.end()) {
                    for (auto &capsuleIt: std::ranges::reverse_view(it->second)) {
                        // TODO resend here
                        // connection.outgoingToSendStack.reverseEnqueue(capsuleIt);
                    }
                    connection.outgoingUnacknowledgedCache.erase(it);
                }
            }
        }
    }

    void RaknetServer::handleFrame(const ServerConnection &connection, const std::span<uint8_t>& span) {
        binary::Cursor cursor(span);
        uint8_t packetId = cursor.readUint8();

        switch (static_cast<RaknetPacketId>(packetId)) {
            case RaknetPacketId::ConnectionRequest: {
                ConnectionRequestPacket connectionRequest;
                connectionRequest.deserialize(cursor);

                ConnectionRequestAcceptedPacket connectionRequestAccepted;
                // TODO

                enqueueFrame();
                break;
            }
        }
    }

    std::string RaknetServer::endpointToString(const asio::ip::udp::endpoint &endpoint) {
        return endpoint.address().to_string() + "#" + std::to_string(endpoint.port());
    }
}
