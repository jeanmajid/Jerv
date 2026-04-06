#include "jerv/raknet/serverConnection.hpp"

namespace jerv::raknet {
    void ServerConnection::send(const RaknetBasePacket &packet) {
        // TODO: Frameset handling and all that
    }

    void ServerConnection::send(const std::vector<uint8_t> &data) {
        socket
    }
}
