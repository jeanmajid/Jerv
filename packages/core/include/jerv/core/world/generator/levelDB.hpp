#pragma once

#include "chunk.hpp"
#include "leveldb/db.h"

namespace jerv::core::world::generator {
    class LevelDB {
    public:
        LevelDB();

        Chunk &readChunk(Chunk &chunk);

    private:
        leveldb::DB *db = nullptr;
    };
}
