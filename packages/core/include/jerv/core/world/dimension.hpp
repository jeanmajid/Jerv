#pragma once
#include <string>
#include "generator/generator.hpp"


namespace jerv::core::world {
    class Dimension {
    public:
        Dimension(std::string id);

        void tick(uint64_t tick);
        generator::ChunkGenerator generator;
    };
}
