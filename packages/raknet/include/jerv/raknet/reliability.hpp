#pragma once
#include <cstdint>

namespace jerv::raknet {
    enum Reliability : uint8_t {
        Unreliable = 0,
        UnreliableSequenced = 1,
        Reliable = 2,
        ReliableOrdered = 3,
        ReliableSequenced = 4
    };
}
