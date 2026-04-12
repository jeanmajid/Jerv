#pragma once
#include <cstdint>
#include <vector>

class SubChunk {
public:
    SubChunk() = default;


private:
    int8_t index;
    std::vector<BlockStorage> layers;
};
