#pragma once
#include <cstdint>

namespace jerv::raknet {
    constexpr uint8_t ACK_BITMASK = 0b01000000;
    constexpr uint8_t NACK_BITMASK = 0b00100000;
}
