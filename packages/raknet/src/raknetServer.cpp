#include "jerv/raknet/raknetServer.hpp"

#include <random>
#include <ranges>

#include "jerv/common/logger.hpp"
#include "jerv/raknet/constants.hpp"
#include "jerv/raknet/frameCapsule.hpp"
#include "jerv/raknet/reliability.hpp"
#include "jerv/raknet/protocol/packetIds.hpp"
#include "jerv/raknet/protocol/packets/connectedPing.hpp"
#include "jerv/raknet/protocol/packets/connectedPong.hpp"
#include "jerv/raknet/protocol/packets/connectionRequest.hpp"
#include "jerv/raknet/protocol/packets/connectionRequestAccepted.hpp"
#include "jerv/raknet/protocol/packets/openConnectionReply1.hpp"
#include "jerv/raknet/protocol/packets/openConnectionReply2Packet.hpp"
#include "jerv/raknet/protocol/packets/openConnectionRequest1.hpp"
#include "jerv/raknet/protocol/packets/openConnectionRequest2.hpp"
#include "jerv/raknet/protocol/packets/unconnectPing.hpp"
#include "jerv/raknet/protocol/packets/unconnectPong.hpp"
#include "jerv/raknet/serverConnection.hpp"

namespace jerv::raknet {
    void RaknetServer::bindV4(uint16_t port, const std::string &address) {
        socket4 = std::make_unique<asio::ip::udp::socket>(ioContext);
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

    void RaknetServer::bindV6(uint16_t port, const std::string &address) {
        (void) port;
        (void) address;
    }

    void RaknetServer::start() {
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution dist(
            std::numeric_limits<int64_t>::min(),
            std::numeric_limits<int64_t>::max()
        );
        serverGuid = -dist(gen);
        serverStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

        ioContext.run();
    }

    template<size_t BufferSize>
    void RaknetServer::sendPacketOffline(const asio::ip::udp::endpoint &endpoint, const RaknetBasePacket &packet) {
        std::array<uint8_t, BufferSize + 1> data;
        binary::Cursor cursor(data);
        cursor.writeUint8(static_cast<uint8_t>(packet.getPacketId()));
        packet.serialize(cursor);

        socket4->send_to(asio::buffer(data, cursor.processedBytesSize()),
                         endpoint);
    }

    void RaknetServer::sendPacketOnline(ServerConnection &connection, const RaknetBasePacket &packet,
                                        const Reliability reliability) {
        // TODO: better buffer management
        std::array<uint8_t, 2000> buffer{};
        binary::Cursor cursor(buffer);

        cursor.writeUint8(static_cast<uint8_t>(packet.getPacketId()));
        packet.serialize(cursor);

        sendFrame(connection, cursor.getProcessedBytes(), reliability);
    }

    void RaknetServer::sendData(const asio::ip::udp::endpoint &endpoint, const std::span<uint8_t> buffer) {
        socket4->send_to(asio::buffer(buffer.data(), buffer.size()),
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

        if (packetId & 0x80) {
            handleOnline(endpoint, cursor);
        } else {
            handleOffline(endpoint, packetId, cursor);
        }
    }

    void RaknetServer::handleOnline(const asio::ip::udp::endpoint &endpoint,
                                    binary::Cursor &cursor) {
        cursor.reset();
        const uint8_t firstByte = cursor.readUint8();
        const auto it = connections.find(endpointToString(endpoint));
        if (it == connections.end()) {
            return;
        }
        ServerConnection &connection = it->second;
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

                sendPacketOffline<34 + 69>(endpoint, unconnectPongPacket);
                break;
            }
            case RaknetPacketId::OpenConnectionRequest1: {
                OpenConnectionRequest1Packet connectionRequest1;
                connectionRequest1.deserialize(cursor);

                OpenConnectionReply1Packet connectionReply1;
                connectionReply1.serverGuid = serverGuid;
                connectionReply1.serverHasSecurity = false;

                const size_t Mtu = cursor.buffer().size() + UDP_HEADER_SIZE;
                JERV_LOG_INFO(Mtu);
                connectionReply1.mtuSize = Mtu > IDEAL_MAX_MTU_SIZE ? IDEAL_MAX_MTU_SIZE : Mtu;

                sendPacketOffline<31>(endpoint, connectionReply1);
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

                sendPacketOffline<34>(endpoint, connectionReply2);

                connections.try_emplace(
                    endpointToString(endpoint),
                    endpoint,
                    socket4.get(),
                    connectionRequest2.mtuSize,
                    connectionRequest2.clientGuid
                );
                break;
            }
            default: {
                JERV_LOG_DEBUG("Unhandled raknet offline packet: 0x{:X}", packetId);
                break;
            };
        }
    }

    void RaknetServer::handleFrameSet(ServerConnection &connection, binary::Cursor &cursor) {
        uint32_t sequenceId = cursor.readUint24<true>();

        if (sequenceId > 0xFFFFE0) {
            // TODO: check what you could do here, maybe wrap around back to 0
            disconnectClient(connection);
            return;
        }

        connection.incomingMissingDatagram.erase(sequenceId);

        if (sequenceId - connection.incomingLastDatagramId > 1) {
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

    std::vector<std::pair<uint32_t, uint32_t> > RaknetServer::getRangesFromSequence(
        const std::vector<uint32_t> &sequence) {
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
            cursor.writeUint24<true>(range.first);

            if (!isSingle) {
                cursor.writeUint24<true>(range.second);
            }
        }

        sendData(connection.endpoint, buffer);
    }

    void RaknetServer::handleCapsule(ServerConnection &connection, binary::Cursor &cursor) {
        FrameCapsule capsule;
        const uint8_t flags = cursor.readUint8();
        const uint8_t reliability = flags >> 5;
        const bool isFragmented = (flags & IS_FRAGMENTED_BIT) != 0;

        const size_t bodyLength = cursor.readUint16() >> 3;

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

    void RaknetServer::handleAck(ServerConnection &connection, binary::Cursor &cursor) {
        // skip packet id
        cursor.setPointer(1);
        uint16_t rangeCount = cursor.readUint16();

        for (uint16_t i = 0; i < rangeCount; ++i) {
            uint8_t isSingle = cursor.readUint8();
            uint32_t min = cursor.readUint24<true>();
            uint32_t max = min;
            if (!isSingle) {
                max = cursor.readUint24<true>();
            }

            for (uint32_t j = min; j <= max; ++j) {
                auto it = connection.outgoingUnacknowledgedCache.find(j);
                if (it != connection.outgoingUnacknowledgedCache.end()) {
                    connection.outgoingUnacknowledgedReliableCapsules -= it->second.size();
                    connection.outgoingUnacknowledgedCache.erase(it);
                }
            }
        }
    }

    void RaknetServer::handleNack(ServerConnection &connection, binary::Cursor &cursor) {
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

            for (int32_t j = max; j >= static_cast<int32_t>(min); --j) {
                auto it = connection.outgoingUnacknowledgedCache.find(j);
                if (it != connection.outgoingUnacknowledgedCache.end()) {
                    for (auto &capsuleIt: std::ranges::reverse_view(it->second)) {
                        connection.outgoingToSendStack.reverseEnqueue(capsuleIt);
                    }
                    connection.outgoingUnacknowledgedCache.erase(it);
                }
            }
        }
    }

    void RaknetServer::handleFrame(ServerConnection &connection, const std::span<uint8_t> &span) {
        binary::Cursor cursor(span);
        uint8_t packetId = cursor.readUint8();

        switch (static_cast<RaknetPacketId>(packetId)) {
            case RaknetPacketId::ConnectedPing: {
                ConnectedPingPacket connectedPing;
                connectedPing.deserialize(cursor);

                ConnectedPongPacket connectedPong;
                connectedPong.timeSinceStartClient = connectedPing.timeSinceStart;
                connectedPong.timeSinceStartServer = serverStartTime;

                sendPacketOnline(connection, connectedPong, Reliability::Unreliable);
                break;
            }
            case RaknetPacketId::ConnectionRequest: {
                ConnectionRequestPacket connectionRequest;
                connectionRequest.deserialize(cursor);

                if (connectionRequest.clientGuid != connection.guid) {
                    JERV_LOG_DEBUG("guid mismatch");
                    return;
                }

                ConnectionRequestAcceptedPacket connectionRequestAccepted;
                connectionRequestAccepted.clientAddress = {
                    .family = static_cast<uint8_t>(connection.endpoint.protocol().family() == 2 ? 4 : 6),
                    .ip = connection.endpoint.address().to_v4().to_uint(),
                    .port = connection.endpoint.port()
                };
                connectionRequestAccepted.systemIndex = 0;
                connectionRequestAccepted.pingTime = connectionRequest.requestTimestamp;
                connectionRequestAccepted.pongTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()
                ).count();

                while (connectionRequestAccepted.systemAddresses.size() < 10) {
                    // 2130706433 is apparently 127.0.0.1, im too lazy to calculate
                    connectionRequestAccepted.systemAddresses.emplace_back(4, 2130706433, 0);
                }
                sendPacketOnline(connection, connectionRequestAccepted, Reliability::ReliableOrdered);
                break;
            }
            case RaknetPacketId::NewIncomingConnection: {
                // don't need to handle
                break;
            }
            case RaknetPacketId::Disconnect: {
                disconnectClient(connection);
                // TODO: send disconnect packet
                break;
            }
            case RaknetPacketId::GameData: {
                callback(context, connection, cursor.getRemainingBytes());
                break;
            }
            default:
                JERV_LOG_DEBUG("unhandled raknet online packet: 0x{:X}", packetId);
        }

        processQueue(connection);
    }

    void RaknetServer::sendFrame(ServerConnection &connection, const std::span<uint8_t> data,
                                 const Reliability reliability) {
        if (!connection.outgoingOrderChannels.contains(connection.outgoingChannelIndex)) {
            connection.outgoingOrderChannels[connection.outgoingChannelIndex] = 0;
            connection.outgoingSequenceChannels[connection.outgoingChannelIndex] = 0;
        }

        FrameCapsule meta;
        meta.orderChannel = connection.outgoingChannelIndex;

        if (IS_SEQUENCED_LOOKUP[reliability]) {
            meta.orderIndex = connection.outgoingOrderChannels[connection.outgoingChannelIndex];
            meta.sequenceIndex = connection.outgoingSequenceChannels[connection.outgoingChannelIndex]++;
        }

        if (IS_ORDERED_EXCLUSIVE_LOOKUP[reliability]) {
            meta.orderIndex = connection.outgoingOrderChannels[connection.outgoingChannelIndex]++;
            connection.outgoingSequenceChannels[connection.outgoingChannelIndex] = 0;
        }

        if (data.size() >= connection.outgoingMtu - MAX_FRAME_SET_HEADER_SIZE) {
            const size_t chunkSize = connection.outgoingMtu - MAX_FRAME_SET_HEADER_SIZE;
            const size_t fragmentCount = (data.size() + chunkSize - 1) / chunkSize;
            const uint16_t id = connection.outgoingNextFragmentId++;

            std::vector<std::span<const uint8_t> > chunks;

            size_t offset = 0;
            while (offset < data.size()) {
                size_t remaining = data.size() - offset;
                const size_t currentChunkSize = std::min(remaining, chunkSize);
                chunks.emplace_back(data.subspan(offset, currentChunkSize));
                offset += currentChunkSize;
            }

            uint32_t index = 0;

            for (const auto &chunk: chunks) {
                FrameCapsule fragMeta = meta;
                const FragmentInfo fragInfo = {id, index, static_cast<uint32_t>(fragmentCount)};
                fragMeta.fragment = fragInfo;
                fragMeta.body = std::span(const_cast<uint8_t *>(chunk.data()), chunk.size());
                fragMeta.reliableIndex = connection.outgoingReliableIndex++;

                sendCapsule(connection, fragMeta, reliability);
                index++;
            }
            return;
        }

        meta.reliableIndex = connection.outgoingReliableIndex++;
        meta.body = std::span(data.data(), data.size());
        sendCapsule(connection, meta, reliability);

        // TODO: Have this here, bcs buffer die from jerv.send. Find a better way to handle it and save performance
        processQueue(connection);
    }

    void RaknetServer::sendCapsule(ServerConnection &connection, const FrameCapsule &frameCapsule,
                                   const Reliability reliability) {
        CapsuleCache cache;
        cache.frame = frameCapsule;
        cache.reliability = reliability;
        connection.outgoingToSendStack.enqueue(cache);
    }

    void RaknetServer::processQueue(ServerConnection &connection) {
        while (!connection.outgoingToSendStack.isEmpty() &&
               connection.outgoingUnacknowledgedReliableCapsules < connection.unacknowledgedWindowSize) {
            auto capsule = connection.outgoingToSendStack.dequeue();

            size_t availableSize = connection.outgoingMtu - connection.outgoingBufferCursor;
            if (availableSize <= MAX_CAPSULE_HEADER_SIZE + capsule.frame.body.size()) {
                createCurrentConnectionBuffer(connection);
            }

            if (capsule.reliability != static_cast<uint8_t>(Reliability::Unreliable) &&
                capsule.reliability != static_cast<uint8_t>(Reliability::UnreliableSequenced)) {
                connection.outgoingUnacknowledgedStack.push_back(capsule);
                connection.outgoingUnacknowledgedReliableCapsules++;
            }

            binary::Cursor cursor(connection.outgoingBuffer);
            cursor.setPointer(connection.outgoingBufferCursor);

            uint8_t flags = capsule.reliability << 5;
            if (capsule.frame.hasFragment) {
                flags |= IS_FRAGMENTED_BIT;
            }
            cursor.writeUint8(flags);

            const auto bodyLengthBits = static_cast<uint16_t>(capsule.frame.body.size() * 8);
            cursor.writeUint16(bodyLengthBits);

            if (IS_RELIABLE_LOOKUP[capsule.reliability]) {
                cursor.writeUint24<true>(capsule.frame.reliableIndex);
            }

            if (IS_SEQUENCED_LOOKUP[capsule.reliability]) {
                cursor.writeUint24<true>(capsule.frame.sequenceIndex);
            }

            if (IS_ORDERED_LOOKUP[capsule.reliability]) {
                cursor.writeUint24<true>(capsule.frame.orderIndex);
                cursor.writeUint8(capsule.frame.orderChannel);
            }

            if (capsule.frame.hasFragment) {
                cursor.writeUint32(capsule.frame.fragment.length);
                cursor.writeUint16(capsule.frame.fragment.id);
                cursor.writeUint32(capsule.frame.fragment.index);
            }

            connection.outgoingBufferCursor = cursor.pointer();

            std::memcpy(connection.outgoingBuffer.data() + connection.outgoingBufferCursor,
                        capsule.frame.body.data(), capsule.frame.body.size());
            connection.outgoingBufferCursor += capsule.frame.body.size();
        }

        if (connection.outgoingBufferCursor > 4) {
            createCurrentConnectionBuffer(connection);
        }
    }

    void RaknetServer::createCurrentConnectionBuffer(ServerConnection &connection) {
        if (connection.outgoingBufferCursor <= 4) return;

        connection.outgoingBuffer[0] = VALID_DATAGRAM_BIT;
        connection.outgoingUnacknowledgedCache[connection.outgoingFrameSetId] = std::move(
            connection.outgoingUnacknowledgedStack);
        connection.outgoingUnacknowledgedStack.clear();

        binary::Cursor cursor(connection.outgoingBuffer);
        cursor.setPointer(1);
        cursor.writeUint24<true>(connection.outgoingFrameSetId++);
        sendData(connection.endpoint, std::span(connection.outgoingBuffer.data(), connection.outgoingBufferCursor));
        connection.outgoingBufferCursor = 4;
    }

    void RaknetServer::disconnectClient(ServerConnection &connection) {
        connections.erase(endpointToString(connection.endpoint));
    }

    std::string RaknetServer::endpointToString(const asio::ip::udp::endpoint &endpoint) {
        // TODO: use client guid
        return endpoint.address().to_string() + "#" + std::to_string(endpoint.port());
    }
}
