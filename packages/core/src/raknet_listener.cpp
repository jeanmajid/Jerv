#include <jerv/core/raknet_listener.hpp>
#include <jerv/core/jerver.hpp>

namespace jerv::core {
    RakNetListener::RakNetListener(Jerver &jerver)
        : jerver_(jerver) {
        setDiscoveryStats({
            .provider = jerver.providerName_,
            .protoVersion = jerv::protocol::PROTOCOL_VERSION,
            .engineVersion = "1.0.0.1",
            .onlinePlayers = static_cast<int>(connections().size()),
            .maxPlayers = 50,
            .serverGuid = guid(),
            .worldName = "World Name",
            .gameMode = "Adventure"
        });
    }

    std::vector<uint8_t> RakNetListener::getMOTD(const raknet::AddressInfo &receiver) {
        return temporaryMOTD_;
    }

    void RakNetListener::setDiscoveryStats(const MotdInformation &info) {
        discoveryStats_ = info;

        std::string motd = "MCPE;" + info.provider + ";" +
                           std::to_string(info.protoVersion) + ";" +
                           info.engineVersion + ";" +
                           std::to_string(info.onlinePlayers) + ";" +
                           std::to_string(info.maxPlayers) + ";" +
                           std::to_string(info.serverGuid) + ";" +
                           info.worldName + ";" +
                           info.gameMode + ";";

        temporaryMOTD_ = std::vector<uint8_t>(motd.begin(), motd.end());
    }
}