#pragma once
#include <cstdint>
#include <span>

namespace jerv::raknet {
    struct FragmentInfo {
        uint16_t id;
        uint32_t index;
        uint32_t length;
    };

    struct FrameCapsule {
        std::span<uint8_t> body;
        FragmentInfo fragment;
        bool hasFragment = false;
        uint8_t orderChannel = 0;
        uint32_t orderIndex = 0;
        uint32_t reliableIndex = 0;
        uint32_t sequenceIndex = 0;
        size_t offset = 0;
    };

    struct CapsuleCache {
        FrameCapsule frame;
        uint8_t reliability;
    };
}