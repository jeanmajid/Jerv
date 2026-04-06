#pragma once
#include <map>
#include <set>
#include <asio/ip/udp.hpp>

#include "fragmentMeta.hpp"

namespace jerv::raknet {
    class RaknetBasePacket;
    class ServerConnection {
    public:
        ServerConnection(const asio::ip::udp::endpoint& endpoint, asio::ip::udp::socket* socket) : endpoint(endpoint), socket(socket) {}

        void send(const RaknetBasePacket& packet);
        void send(const std::vector<uint8_t>& data);

        std::chrono::steady_clock::time_point incomingLastActivity;
        std::map<uint32_t, std::vector<CapsuleCache> > outgoingUnacknowledgedCache;
        size_t outgoingUnacknowledgedReliableCapsules = 0;
        int32_t incomingLastDatagramId;
        std::set<uint32_t> incomingMissingDatagram;
        std::vector<uint32_t> incomingReceivedDatagramAcknowledgeStack;
        std::map<uint16_t, FragmentMeta> incomingFragmentRebuildTable;
    private:
        asio::ip::udp::endpoint endpoint;
        asio::ip::udp::socket* socket = nullptr;
    };
}
