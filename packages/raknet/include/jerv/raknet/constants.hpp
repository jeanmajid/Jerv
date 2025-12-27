#pragma once

#include <cstdint>
#include <array>
#include <random>

namespace jerv::raknet {
    constexpr std::array<uint8_t, 16> MAGIC = {
        0x00, 0xFF, 0xFF, 0x00, 0xFE, 0xFE, 0xFE, 0xFE,
        0xFD, 0xFD, 0xFD, 0xFD, 0x12, 0x34, 0x56, 0x78
    };

    constexpr uint8_t ONLINE_DATAGRAM_BIT_MASK = 0b1110'0000;
    constexpr uint8_t VALID_DATAGRAM_BIT = 0b1000'0000;
    constexpr uint8_t ACK_DATAGRAM_BIT = 0b0100'0000;
    constexpr uint8_t NACK_DATAGRAM_BIT = 0b0010'0000;
    constexpr uint8_t RELIABILITY_BIT_MASK = 0b1110'0000;
    constexpr uint8_t IS_FRAGMENTED_BIT = 0b0001'0000;

    constexpr size_t IDEAL_MAX_MTU_SIZE = 1432;
    constexpr size_t UDP_HEADER_SIZE = 68;
    constexpr size_t CAPSULE_FRAGMENT_META_SIZE = 10;
    constexpr size_t MAX_CAPSULE_HEADER_SIZE = CAPSULE_FRAGMENT_META_SIZE + 13;
    constexpr size_t MAX_FRAME_SET_HEADER_SIZE = MAX_CAPSULE_HEADER_SIZE + 4;

    constexpr bool IS_RELIABLE_LOOKUP[] = {false, false, true, true, true};
    constexpr bool IS_SEQUENCED_LOOKUP[] = {false, true, false, false, true};
    constexpr bool IS_ORDERED_LOOKUP[] = {false, true, false, true, true};
    constexpr bool IS_ORDERED_EXCLUSIVE_LOOKUP[] = {false, false, false, true, false, false, false, true};

    inline uint64_t random64() {
        static std::random_device rd;
        static std::mt19937_64 gen(rd());
        static std::uniform_int_distribution<uint64_t> dist;
        return dist(gen);
    }
}