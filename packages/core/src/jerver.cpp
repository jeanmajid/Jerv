#include "jerv/core/jerver.hpp"

namespace jerv::core {
    Jerver::Jerver() {
    }

    void Jerver::bindV4(const uint16_t port) {
        raknetServer.bindV4(port);
    }

    void Jerver::bindV6(const uint16_t port) {
        raknetServer.bindV6(port);
    }

    void Jerver::start() {
        raknetServer.setCallback(this, &handleDataStatic);
        raknetServer.start();
    }

    void Jerver::handleDataStatic(void *ctx, raknet::ServerConnection &serverConnection, binary::Cursor &cursor) {
        static_cast<Jerver *>(ctx)->handleData(serverConnection, cursor);
    }

    void Jerver::handleData(raknet::ServerConnection &serverConnection, binary::Cursor &cursor) {

    }
}
