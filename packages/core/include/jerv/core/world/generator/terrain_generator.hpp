#pragma once

#include <jerv/core/world/chunk.hpp>
#include <string>
#include <cstdint>
#include <memory>

namespace jerv::core {
    struct TerrainGeneratorProperties {
        uint64_t seed = 0;
        int32_t seaLevel = 63;
        int32_t baseHeight = 64;
        int32_t heightVariation = 32;
    };

    class TerrainGenerator {
    public:
        virtual ~TerrainGenerator() = default;

        virtual const std::string &identifier() const = 0;
        virtual std::unique_ptr<Chunk> generate(int32_t cx, int32_t cz) = 0;
        const TerrainGeneratorProperties &properties() const { return properties_; }
        uint64_t seed() const { return properties_.seed; }
    protected:
        TerrainGeneratorProperties properties_;

        TerrainGenerator(const TerrainGeneratorProperties &props = {})
            : properties_(props) {
        }
    };
}