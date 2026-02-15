#include <jerv/core/network_connection.hpp>
#include <jerv/common/logger.hpp>

namespace jerv::core {
    void NetworkConnection::handleSetLocalPlayerAsInitialized(binary::Cursor &cursor) {
        JERV_LOG_INFO("{} initialized", playerName_);
    }
}