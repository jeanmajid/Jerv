#pragma once

#include <cstdint>
#include <string>

namespace jerv::core {
    constexpr uint16_t NETWORK_LAN_DISCOVERY_PORT4 = 19132;
    constexpr uint16_t NETWORK_LAN_DISCOVERY_PORT6 = 19133;
    constexpr const char *NETWORK_ANY_ADDRESS4 = "0.0.0.0";
    constexpr const char *NETWORK_LOOPBACK_ADDRESS4 = "127.0.0.1";
    constexpr const char *NETWORK_ANY_ADDRESS6 = "::0";
    constexpr const char *NETWORK_LOOPBACK_ADDRESS6 = "::1";
}