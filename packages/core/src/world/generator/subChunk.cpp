#include "jerv/core/world/generator/subChunk.hpp"

namespace jerv::core::world {
    BlockStorage& SubChunk::getLayer(const size_t index) {
        if (index >= layers.size()) {
            layers.resize(index + 1);
        }
        return layers[index];
    }

    int32_t SubChunk::getState(const int32_t x, const int32_t y, const int32_t z, const size_t layer) {
        if (layer >= layers.size()) return 0;
        return layers[layer].getState(x, y, z);
    }

    void SubChunk::setState(const int32_t x, const int32_t y, const int32_t z, const int32_t state,
                            const size_t layer) {
        getLayer(layer).setState(x, y, z, state);
    }

    bool SubChunk::isEmpty() {
        for (auto &layer: layers) {
            if (!layer.isEmpty()) return false;
        }
        return true;
    }

    void SubChunk::serialize(jerv::binary::ResizableCursor &cursor) {
        cursor.growToFit(2);
        cursor.writeUint8(VERSION);
        cursor.writeUint8(static_cast<uint8_t>(layers.size()));

        // if (version_ == 9) {
        //     cursor.writeUint8(static_cast<uint8_t>(index));
        // }

        for (auto &layer: layers) {
            layer.serialize(cursor);
        }
    }
}
