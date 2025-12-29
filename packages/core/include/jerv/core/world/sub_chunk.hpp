#pragma once

#include <jerv/core/world/block_storage.hpp>
#include <jerv/binary/cursor.hpp>
#include <vector>
#include <cstdint>

namespace jerv::core {
    class SubChunk {
    public:
        static constexpr uint8_t VERSION = 9;

        SubChunk() : version_(VERSION), index_(0) {
        }

        explicit SubChunk(const int8_t index) : version_(VERSION), index_(index) {
        }

        bool isEmpty() const {
            for (const auto &layer: layers_) {
                if (!layer.isEmpty()) return false;
            }
            return true;
        }

        BlockStorage &getLayer(const size_t index = 0) {
            while (layers_.size() <= index) {
                layers_.emplace_back();
            }
            return layers_[index];
        }

        const BlockStorage &getLayer(const size_t index = 0) const {
            static BlockStorage empty;
            if (index >= layers_.size()) return empty;
            return layers_[index];
        }

        int32_t getState(const int32_t bx, const int32_t by, const int32_t bz, const size_t layer = 0) const {
            if (layer >= layers_.size()) return 0;
            return layers_[layer].getState(bx, by, bz);
        }

        void setState(const int32_t bx, const int32_t by, const int32_t bz, const int32_t state, const size_t layer = 0) {
            getLayer(layer).setState(bx, by, bz, state);
        }

        void serialize(binary::ResizableCursor &cursor) const {
            cursor.growToFit(3);
            cursor.writeUint8(version_);
            cursor.writeUint8(static_cast<uint8_t>(layers_.size()));

            if (version_ == 9) {
                cursor.writeUint8(static_cast<uint8_t>(index_));
            }

            for (const auto &layer: layers_) {
                layer.serialize(cursor);
            }
        }

        int8_t index() const { return index_; }
        void setIndex(const int8_t idx) { index_ = idx; }

        uint8_t version() const { return version_; }
        size_t layerCount() const { return layers_.size(); }
    private:
        uint8_t version_;
        int8_t index_;
        std::vector<BlockStorage> layers_;
    };
}