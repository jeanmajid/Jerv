#pragma once

#include <jerv/core/world/generator/terrain_generator.hpp>
#include <jerv/core/world/generator/noise.hpp>
#include <jerv/core/world/chunk.hpp>
#include <memory>
#include <cmath>

namespace jerv::core {
    class OverworldGenerator : public TerrainGenerator {
    public:
        static inline const std::string IDENTIFIER = "overworld";

        explicit OverworldGenerator(const TerrainGeneratorProperties &props = {})
            : TerrainGenerator(props)
              , heightNoise_(props.seed)
              , detailNoise_(props.seed + 1)
              , biomeNoise_(props.seed + 3) {
        }

        const std::string &identifier() const override {
            return IDENTIFIER;
        }

        std::unique_ptr<Chunk> generate(int32_t cx, int32_t cz) override {
            auto chunk = std::make_unique<Chunk>(cx, cz, protocol::DimensionId::Overworld);

            const int32_t worldX = cx * 16;
            const int32_t worldZ = cz * 16;

            std::array<int32_t, 256> heightMap;
            generateHeightmap(worldX, worldZ, heightMap);

            for (int32_t x = 0; x < 16; x++) {
                for (int32_t z = 0; z < 16; z++) {
                    const int32_t surfaceY = heightMap[x * 16 + z];
                    fillColumn(*chunk, x, z, surfaceY);
                }
            }
            return chunk;
        }

    private:
        PerlinNoise heightNoise_;
        PerlinNoise detailNoise_;
        PerlinNoise biomeNoise_;

        static constexpr double HEIGHT_SCALE = 0.005;
        static constexpr double DETAIL_SCALE = 0.02;
        static constexpr double CAVE_THRESHOLD = 0.6;

        void generateHeightmap(const int32_t worldX, const int32_t worldZ, std::array<int32_t, 256> &heightMap) const {
            for (int32_t x = 0; x < 16; x++) {
                for (int32_t z = 0; z < 16; z++) {
                    const double wx = worldX + x;
                    const double wz = worldZ + z;

                    const double continentNoise = heightNoise_.fbm2D(
                        wx * HEIGHT_SCALE * 0.5,
                        wz * HEIGHT_SCALE * 0.5,
                        4, 0.5, 2.0
                    );

                    const double hillNoise = heightNoise_.fbm2D(
                        wx * HEIGHT_SCALE,
                        wz * HEIGHT_SCALE,
                        6, 0.5, 2.0
                    );

                    const double detailNoise = detailNoise_.fbm2D(
                        wx * DETAIL_SCALE,
                        wz * DETAIL_SCALE,
                        4, 0.5, 2.0
                    );

                    const double ridgeNoise = heightNoise_.ridgeNoise2D(
                        wx * HEIGHT_SCALE * 0.7,
                        wz * HEIGHT_SCALE * 0.7,
                        4, 0.5, 2.0
                    );

                    double height = properties_.baseHeight;

                    height += continentNoise * properties_.heightVariation * 0.5;
                    height += hillNoise * properties_.heightVariation * 0.3;

                    const double ridgeWeight = std::max(0.0, (continentNoise + 0.3) * 0.5);
                    height += ridgeNoise * properties_.heightVariation * ridgeWeight;

                    height += detailNoise * 4.0;

                    int32_t finalHeight = static_cast<int32_t>(std::round(height));
                    finalHeight = std::max(-60, std::min(finalHeight, 256));

                    heightMap[x * 16 + z] = finalHeight;
                }
            }
        }

        void fillColumn(Chunk &chunk, const int32_t x, const int32_t z, const int32_t surfaceY) const {
            const int32_t minY = chunk.minY();
            const int32_t seaLevel = properties_.seaLevel;

            for (int32_t y = minY; y <= chunk.maxY(); y++) {
                if (y > surfaceY) {
                    if (y <= seaLevel) {
                        chunk.setBlock(x, y, z, BlockIds::Water);
                    }
                    continue;
                }

                int32_t block;
                int32_t depth = surfaceY - y;
                const bool underwater = surfaceY < seaLevel;

                if (y == minY) {
                    block = BlockIds::Bedrock;
                } else if (y < minY + 4) {
                    const double noise = detailNoise_.noise2D(x * 0.5 + y, z * 0.5);
                    block = (noise > 0.3) ? BlockIds::Bedrock : BlockIds::Stone;
                } else if (y <= surfaceY - 4) {
                    block = BlockIds::Stone;
                } else if (y < surfaceY) {
                    block = underwater ? BlockIds::Sand : BlockIds::Dirt;
                } else {
                    if (underwater) {
                        block = BlockIds::Sand;
                    } else if (surfaceY > properties_.baseHeight + 40) {
                        block = BlockIds::Stone;
                    } else {
                        block = BlockIds::GrassBlock;
                    }
                }

                chunk.setBlock(x, y, z, block);
            }
        }
    };


    class FlatGenerator : public TerrainGenerator {
    public:
        static inline const std::string IDENTIFIER = "flat";

        explicit FlatGenerator(const TerrainGeneratorProperties &props = {})
            : TerrainGenerator(props) {
        }

        const std::string &identifier() const override {
            return IDENTIFIER;
        }

        std::unique_ptr<Chunk> generate(int32_t cx, int32_t cz) override {
            auto chunk = std::make_unique<Chunk>(cx, cz, protocol::DimensionId::Overworld);
            chunk->generateFlatGrass(properties_.seaLevel);
            return chunk;
        }
    };


    class VoidGenerator : public TerrainGenerator {
    public:
        static inline const std::string IDENTIFIER = "void";

        explicit VoidGenerator(const TerrainGeneratorProperties &props = {})
            : TerrainGenerator(props) {
        }

        const std::string &identifier() const override {
            return IDENTIFIER;
        }

        std::unique_ptr<Chunk> generate(int32_t cx, int32_t cz) override {
            return std::make_unique<Chunk>(cx, cz, protocol::DimensionId::Overworld);
        }
    };
}