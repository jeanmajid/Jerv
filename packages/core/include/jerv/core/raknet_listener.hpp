#pragma once

#include <jerv/raknet/server_connection_listener.hpp>
#include <string>

namespace jerv {
    class Jerver;
}

namespace jerv::core {
    struct MotdInformation {
        std::string provider;
        int protoVersion;
        std::string engineVersion;
        int onlinePlayers;
        int maxPlayers;
        uint64_t serverGuid;
        std::string worldName;
        std::string gameMode;
    };

    class RakNetListener : public raknet::ServerConnectionListener {
    public:
        explicit RakNetListener(Jerver &jerver);
        std::vector<uint8_t> getMOTD(const raknet::AddressInfo &receiver) override;
        void setDiscoveryStats(const MotdInformation &info);
        const MotdInformation &discoveryStats() const { return discoveryStats_; }
    private:
        Jerver &jerver_;
        MotdInformation discoveryStats_;
        std::vector<uint8_t> temporaryMOTD_;
    };
}