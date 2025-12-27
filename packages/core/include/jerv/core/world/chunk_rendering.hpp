#pragma once

#include <jerv/core/world/chunk.hpp>
#include <unordered_set>
#include <vector>
#include <tuple>
#include <algorithm>
#include <cstdint>
#include <cmath>

namespace jerv::core {
    struct ChunkCoords {
        int32_t x;
        int32_t z;

        static int64_t hash(const int32_t x, const int32_t z) {
            return (static_cast<int64_t>(x) << 32) | (static_cast<uint32_t>(z));
        }

        static ChunkCoords unhash(const int64_t hash) {
            return {
                static_cast<int32_t>(hash >> 32),
                static_cast<int32_t>(hash & 0xFFFFFFFF)
            };
        }
    };

    class ChunkRenderingManager {
    public:
        ChunkRenderingManager() = default;

        void setViewDistance(const int32_t distance) {
            if (viewDistance_ != distance) {
                viewDistance_ = distance;
                rebuildOffsets();
            }
        }

        int32_t viewDistance() const { return viewDistance_; }

        void setPosition(const int32_t chunkX, const int32_t chunkZ) {
            chunkX_ = chunkX;
            chunkZ_ = chunkZ;
        }

        int32_t chunkX() const { return chunkX_; }
        int32_t chunkZ() const { return chunkZ_; }

        bool isLoaded(int32_t x, int32_t z) const {
            return loadedChunks_.find(ChunkCoords::hash(x, z)) != loadedChunks_.end();
        }

        bool isLoaded(const int64_t hash) const {
            return loadedChunks_.contains(hash);
        }

        void markLoaded(const int32_t x, const int32_t z) {
            loadedChunks_.insert(ChunkCoords::hash(x, z));
        }

        void markUnloaded(const int32_t x, const int32_t z) {
            loadedChunks_.erase(ChunkCoords::hash(x, z));
        }

        void markUnloaded(const int64_t hash) {
            loadedChunks_.erase(hash);
        }

        double distance(const int32_t x, const int32_t z) const {
            const double dx = static_cast<double>(x) - chunkX_;
            const double dz = static_cast<double>(z) - chunkZ_;
            return std::sqrt(dx * dx + dz * dz);
        }

        double distance(const int64_t hash) const {
            const auto coords = ChunkCoords::unhash(hash);
            return distance(coords.x, coords.z);
        }

        std::vector<int64_t> getChunksToUnload() const {
            std::vector<int64_t> toUnload;
            const double maxDistance = static_cast<double>(viewDistance_) + 0.5;

            for (int64_t hash: loadedChunks_) {
                if (distance(hash) > maxDistance) {
                    toUnload.push_back(hash);
                }
            }

            return toUnload;
        }

        std::vector<std::pair<int32_t, int32_t> > getChunksToLoad(size_t maxCount = 64) const {
            std::vector<std::pair<int32_t, int32_t> > result;
            result.reserve(maxCount);

            for (const auto &[dx, dz]: radialOffsets_) {
                int32_t cx = chunkX_ + dx;
                int32_t cz = chunkZ_ + dz;

                if (!isLoaded(cx, cz)) {
                    result.push_back({cx, cz});
                    if (result.size() >= maxCount) {
                        break;
                    }
                }
            }

            return result;
        }

        void clearAll() {
            loadedChunks_.clear();
        }

        std::vector<int64_t> unloadDistantChunks() {
            auto toUnload = getChunksToUnload();
            for (int64_t hash: toUnload) {
                loadedChunks_.erase(hash);
            }
            return toUnload;
        }

        size_t loadedCount() const { return loadedChunks_.size(); }
        const std::unordered_set<int64_t> &loadedChunks() const { return loadedChunks_; }

    private:
        void rebuildOffsets() {
            radialOffsets_.clear();
            double r2 = (viewDistance_ + 0.5) * (viewDistance_ + 0.5);
            std::vector<std::tuple<double, int32_t, int32_t> > temp;

            for (int32_t dx = -viewDistance_; dx <= viewDistance_; dx++) {
                for (int32_t dz = -viewDistance_; dz <= viewDistance_; dz++) {
                    double dist2 = dx * dx + dz * dz;
                    if (dist2 <= r2) {
                        temp.push_back({dist2, dx, dz});
                    }
                }
            }

            std::ranges::sort(temp, [](const auto &a, const auto &b) {
                return std::get < 0 > (a) < std::get < 0 > (b);
            });

            radialOffsets_.reserve(temp.size());
            for (const auto &[dist2, dx, dz]: temp) {
                radialOffsets_.push_back({dx, dz});
            }
        }

        int32_t viewDistance_ = 0;
        int32_t chunkX_ = 0;
        int32_t chunkZ_ = 0;

        std::unordered_set<int64_t> loadedChunks_;
        std::vector<std::pair<int32_t, int32_t> > radialOffsets_;
    };
}