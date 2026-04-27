#pragma once
#include <map>
#include <set>
#include <asio/ip/udp.hpp>
#include <utility>

#include "circularBufferQueue.hpp"
#include "fragmentMeta.hpp"
#include "frameCapsule.hpp"
#include "constants.hpp"

namespace jerv::raknet {
    class RaknetBasePacket;

    class ServerConnection {
    public:
        ServerConnection(asio::ip::udp::endpoint endpoint, asio::ip::udp::socket *socket,
                         const uint16_t mtu, const int64_t guid) : guid(guid), mtu(mtu),
                                                                   outgoingMtu(mtu - UDP_HEADER_SIZE),
                                                                   outgoingBuffer(IDEAL_MAX_MTU_SIZE),
                                                                   endpoint(std::move(endpoint)), socket(socket) {
        }

        // TODO: Temporary data store here, player stuff later goes into a seperate player class
        std::string playerName;
        std::string playerXuid;
        bool playerSpawned = false;

        float playerLocationX = 0;
        float playerLocationY = 0;
        float playerLocationZ = 0;

        int32_t playerViewDistance = 0;

        int64_t guid;
        uint16_t mtu;
        uint16_t outgoingMtu;
        std::chrono::steady_clock::time_point incomingLastActivity;
        std::map<uint32_t, std::vector<CapsuleCache> > outgoingUnacknowledgedCache;
        size_t outgoingUnacknowledgedReliableCapsules = 0;
        uint32_t incomingLastDatagramId = -1;
        std::set<uint32_t> incomingMissingDatagram;
        std::vector<uint32_t> incomingReceivedDatagramAcknowledgeStack;
        std::map<uint16_t, FragmentMeta> incomingFragmentRebuildTable;

        std::map<uint8_t, uint32_t> outgoingOrderChannels;
        std::map<uint8_t, uint32_t> outgoingSequenceChannels;
        uint8_t outgoingChannelIndex = 0;
        uint16_t outgoingNextFragmentId = 0;
        uint32_t outgoingReliableIndex = 0;
        CircularBufferQueue<CapsuleCache> outgoingToSendStack{1024};
        size_t unacknowledgedWindowSize = 512;

        size_t outgoingBufferCursor = 4;
        std::vector<uint8_t> outgoingBuffer;
        uint32_t outgoingFrameSetId = 0;
        std::vector<CapsuleCache> outgoingUnacknowledgedStack;

        asio::ip::udp::endpoint endpoint;
        asio::ip::udp::socket *socket = nullptr;

        bool networkSettingsSent = false;
    };
}
