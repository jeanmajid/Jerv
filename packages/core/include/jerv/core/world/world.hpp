#pragma once

#include <jerv/core/world/chunk.hpp>
#include <jerv/core/world/generator/terrain_generator.hpp>
#include <jerv/core/world/generator/overworld_generator.hpp>
#include <unordered_map>
#include <memory>
#include <cstdint>
#include <functional>
#include <utility>

namespace jerv::core {
    struct ChunkCoordHash {
        std::size_t operator()(const std::pair<int32_t, int32_t> &coord) const {
            return std::hash<int64_t>()(
                (static_cast<int64_t>(coord.first) << 32) |
                (static_cast<uint32_t>(coord.second))
            );
        }
    };

    class World {
    public:
        World(std::string name = "world", const uint64_t seed = 12345)
            : name_(std::move(name))
              , seed_(seed) {
            TerrainGeneratorProperties props;
            props.seed = seed;
            props.baseHeight = 64;
            props.heightVariation = 32;
            props.seaLevel = 63;
            generator_ = std::make_unique<OverworldGenerator>(props);
        }

        const std::string &name() const { return name_; }
        uint64_t seed() const { return seed_; }

        template<typename T, typename... Args>
        void setGenerator(Args &&... args) {
            static_assert(std::is_base_of_v<TerrainGenerator, T>,
                          "T must derive from TerrainGenerator");
            generator_ = std::make_unique<T>(std::forward<Args>(args)...);
        }

        TerrainGenerator &generator() const { return *generator_; }

        Chunk &getChunk(int32_t cx, int32_t cz) {
            const auto key = std::make_pair(cx, cz);
            auto it = chunks_.find(key);

            if (it != chunks_.end()) {
                return *it->second;
            }

            auto chunk = generator_->generate(cx, cz);
            auto &ref = *chunk;
            chunks_[key] = std::move(chunk);
            return ref;
        }

        bool hasChunk(int32_t cx, int32_t cz) const {
            return chunks_.contains({cx, cz});
        }

        void unloadChunk(int32_t cx, int32_t cz) {
            chunks_.erase({cx, cz});
        }

        size_t chunkCount() const { return chunks_.size(); }

        template<typename Func>
        void forEachChunk(Func &&func) {
            for (auto &[coord, chunk]: chunks_) {
                func(coord.first, coord.second, *chunk);
            }
        }

        std::vector<std::pair<int32_t, int32_t> > getChunksInRadius(
            int32_t centerX, int32_t centerZ, const int32_t radius, bool onlyNew = false
        ) const {
            std::vector<std::pair<int32_t, int32_t> > result;

            for (int32_t r = 0; r <= radius; r++) {
                if (r == 0) {
                    if (!onlyNew || !hasChunk(centerX, centerZ)) {
                        result.emplace_back(centerX, centerZ);
                    }
                    continue;
                }

                for (int32_t dx = -r; dx <= r; dx++) {
                    if (!onlyNew || !hasChunk(centerX + dx, centerZ - r)) {
                        result.emplace_back(centerX + dx, centerZ - r);
                    }
                    if (!onlyNew || !hasChunk(centerX + dx, centerZ + r)) {
                        result.emplace_back(centerX + dx, centerZ + r);
                    }
                }

                for (int32_t dz = -r + 1; dz <= r - 1; dz++) {
                    if (!onlyNew || !hasChunk(centerX - r, centerZ + dz)) {
                        result.emplace_back(centerX - r, centerZ + dz);
                    }
                    if (!onlyNew || !hasChunk(centerX + r, centerZ + dz)) {
                        result.emplace_back(centerX + r, centerZ + dz);
                    }
                }
            }

            return result;
        }

        void unloadChunksOutsideRadius(const int32_t centerX, const int32_t centerZ, const int32_t radius) {
            std::vector<std::pair<int32_t, int32_t> > toUnload;

            for (const auto &coord: chunks_ | std::views::keys) {
                const int32_t dx = std::abs(coord.first - centerX);
                const int32_t dz = std::abs(coord.second - centerZ);

                if (dx > radius + 2 || dz > radius + 2) {
                    toUnload.push_back(coord);
                }
            }

            for (const auto &coord: toUnload) {
                chunks_.erase(coord);
            }
        }

    private:
        std::string name_;
        uint64_t seed_;
        std::unique_ptr<TerrainGenerator> generator_;
        std::unordered_map<std::pair<int32_t, int32_t>, std::unique_ptr<Chunk>, ChunkCoordHash> chunks_;
    };
}