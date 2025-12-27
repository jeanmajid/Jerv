#include <jerv/raknet/server_connection.hpp>
#include <jerv/raknet/proto.hpp>
#include <iostream>

namespace jerv::raknet {
    ServerConnection::ServerConnection(
        SocketSource &source,
        const AddressInfo &endpoint,
        const AddressInfo &serverAddress,
        uint64_t guid,
        size_t mtuSize
    )
        : BaseConnection(source, endpoint, guid)
          , serverAddress_(serverAddress) {
        outgoingMTU_ = mtuSize - UDP_HEADER_SIZE;


        onFrame = [this](FrameDescriptor &desc) {
            onFrameReceived(desc);
        };
    }

    void ServerConnection::disconnect() {
        BaseConnection::disconnect();
        if (onDisconnect) {
            onDisconnect();
        }
    }

    void ServerConnection::onFrameReceived(FrameDescriptor &desc) {
        if (desc.body.empty()) {
            return;
        }

        uint8_t packetId = desc.body[0];

        switch (packetId) {
            case 0x09:
                handleConnectionRequest(desc.body);
                break;
            case 0x15:
                handleDisconnect(desc.body);
                break;
            case 0x16:
                break;
            case 0x00:
                handleConnectedPing(desc.body);
                break;
            case 0x13:
                handleNewIncomingConnection(desc.body);
                break;
            case 0xFE:
                handleGameData(desc.body);
                break;
            default:
                if (onErrorHandle) {
                    onErrorHandle("no handler: 0x" + std::to_string(packetId));
                }
                break;
        }


        processQueue();
    }

    void ServerConnection::handleConnectionRequest(std::span<uint8_t> message) {
        auto info = proto::getConnectionRequestInfo(message);

        if (info.guid != guid_) {
            if (onErrorHandle) {
                onErrorHandle("guid mismatch");
            }
            return;
        }

        auto buffer = proto::rentConnectionRequestAcceptPacketWith(
            endpoint_, serverAddress_, info.time,
            static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count())
        );

        enqueueFrame(buffer, static_cast<uint8_t>(Reliability::ReliableOrdered));
    }

    void ServerConnection::handleDisconnect(std::span<uint8_t> message) {
        close();
        if (onDisconnect) {
            onDisconnect();
        }
    }

    void ServerConnection::handleConnectedPing(std::span<uint8_t> message) {
        uint64_t time = proto::getConnectedPingTime(message);
        auto buffer = proto::rentConnectedPongBufferWith(
            time,
            static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count())
        );
        enqueueFrame(buffer, static_cast<uint8_t>(Reliability::Unreliable));
    }

    void ServerConnection::handleNewIncomingConnection(std::span<uint8_t> message) {
        if (onConnectionEstablishedHandle) {
            onConnectionEstablishedHandle();
        }
    }

    void ServerConnection::handleGameData(std::span<uint8_t> message) {
        if (onGamePacket) {
            onGamePacket(message);
        }
    }
}