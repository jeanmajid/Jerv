/* SPDX-License-Identifier: LGPL-3.0-or-later
 * ============================================================================
 *  Jerv - Minecraft Bedrock Server Software
 *  Copyright (C) 2025-2026 jeanmajid
 *  https://github.com/jeanmajid/Jerv
 * ============================================================================
 *
 * This file is part of Jerv.
 *
 * Jerv is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Jerv is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Jerv. If not, see <https://www.gnu.org/licenses/>.
 */

#include "jerv/core/world/generator/levelDB.hpp"
#include <string>

#include "jerv/binary/nbt.hpp"

#include <map>

namespace jerv::core::world::generator {
    class Chunk;

    LevelDB::LevelDB() {
        leveldb::Options options;
        options.block_size = 64 * 1024;
        options.create_if_missing = false;

        leveldb::Status status = leveldb::DB::Open(options,
                                                   "C:/Users/jeanh/AppData/Roaming/Minecraft Bedrock/Users/17010935870061832014/games/com.mojang/minecraftWorlds/ma6/db",
                                                   &db);

        if (!status.ok()) {
            JERV_LOG_ERROR(status.ToString());
        }
    }

    Chunk &LevelDB::readChunk(Chunk &chunk) {
        std::string chunkIndex;
        chunkIndex.resize(8);
        binary::Cursor chunkIndexCursor(
            std::span(
                reinterpret_cast<uint8_t *>(chunkIndex.data()),
                chunkIndex.size()
            )
        );

        chunkIndexCursor.writeInt32<true>(chunk.chunkX);
        chunkIndexCursor.writeInt32<true>(chunk.chunkZ);

        std::string chunkVersion;
        leveldb::ReadOptions readOptions;
        leveldb::Status s = db->Get(readOptions, chunkIndex + '\x2c', &chunkVersion);
        if (!s.ok()) {
            return chunk;
        }

        for (int subChunkY = -4; subChunkY <= 19; ++subChunkY) {
            std::string subChunkData;
            leveldb::Status s2 = db->Get(readOptions, chunkIndex + '\x2f' + static_cast<char>(subChunkY & 0xff),
                                         &subChunkData);
            if (!s2.ok()) {
                continue;
            }
            binary::Cursor subChunkCursor(
                std::span(
                    reinterpret_cast<uint8_t *>(subChunkData.data()),
                    subChunkData.size()
                )
            );
            uint8_t subChunkVersion = subChunkCursor.readUint8();
            uint8_t storageCount = subChunkCursor.readUint8();

            if (subChunkVersion >= 9) {
                subChunkCursor.readUint8();
            }

            for (int storageIndex = 0; storageIndex < storageCount; ++storageIndex) {
                uint8_t bitsPerBlock = subChunkCursor.readUint8() >> 1;

                size_t packedBytes = 0;
                int32_t blocksPerWord = 0;
                int32_t paletteSize = 1;

                if (bitsPerBlock != 0) {
                    blocksPerWord = 32 / bitsPerBlock;
                    const int32_t wordCount = (4096 + blocksPerWord - 1) / blocksPerWord;
                    packedBytes = static_cast<size_t>(wordCount) * 4;
                }

                auto packedSpan = subChunkCursor.readSliceSpan(packedBytes);

                if (bitsPerBlock != 0) {
                    paletteSize = subChunkCursor.readInt32<true>();
                }

                std::vector<int32_t> paletteStates;
                paletteStates.resize(paletteSize);

                for (int blockIndex = 0; blockIndex < paletteSize; ++blockIndex) {
                    binary::NBT nbt(subChunkCursor);
                    auto rootValue = nbt.next();

                    uint32_t hash = 0x811c9dc5u;

                    // TODO: don't live calculate all the hashes
                    auto &rootMap = std::get<std::unordered_map<std::string, binary::NBTData> >(rootValue.value);

                    std::vector<uint8_t> nbtBytes;
                    auto writeString = [&](const std::string &str) {
                        nbtBytes.push_back(str.length() & 0xFF);
                        nbtBytes.push_back((str.length() >> 8) & 0xFF);
                        for (char c: str) nbtBytes.push_back(c);
                    };

                    nbtBytes.push_back(0x0a);
                    writeString("");

                    if (rootMap.contains("name")) {
                        nbtBytes.push_back(0x08);
                        writeString("name");
                        writeString(std::get<std::string>(rootMap.at("name").value));
                    }

                    nbtBytes.push_back(0x0a);
                    writeString("states");

                    if (rootMap.contains("states")) {
                        auto &statesMap = std::get<std::unordered_map<std::string, binary::NBTData> >(
                            rootMap.at("states").value);
                        std::map<std::string, const binary::NBTData *> orderedStates;
                        for (const auto &kv: statesMap) {
                            orderedStates[kv.first] = &kv.second;
                        }

                        for (const auto &kv: orderedStates) {
                            const auto &k = kv.first;
                            const auto *v = kv.second;
                            nbtBytes.push_back(v->type);
                            writeString(k);
                            if (v->type == binary::NBTDataType::Int8) {
                                nbtBytes.push_back(std::get<int8_t>(v->value));
                            } else if (v->type == binary::NBTDataType::Int32) {
                                int32_t val = std::get<int32_t>(v->value);
                                nbtBytes.push_back(val & 0xFF);
                                nbtBytes.push_back((val >> 8) & 0xFF);
                                nbtBytes.push_back((val >> 16) & 0xFF);
                                nbtBytes.push_back((val >> 24) & 0xFF);
                            } else if (v->type == binary::NBTDataType::String) {
                                writeString(std::get<std::string>(v->value));
                            }
                        }
                    }
                    nbtBytes.push_back(0x00);
                    nbtBytes.push_back(0x00);

                    for (uint8_t byte: nbtBytes) {
                        hash ^= byte;
                        hash = hash + (hash << 1) + (hash << 4) + (hash << 7) + (hash << 8) + (hash << 24);
                    }

                    paletteStates[blockIndex] = static_cast<int32_t>(hash);
                }

                for (int32_t blockIndex = 0; blockIndex < 4096; ++blockIndex) {
                    uint32_t paletteIndex = 0;

                    if (bitsPerBlock != 0) {
                        const int32_t wordIndex = blockIndex / blocksPerWord;
                        const int32_t bitOffset = (blockIndex % blocksPerWord) * bitsPerBlock;

                        const uint32_t word = static_cast<uint32_t>(packedSpan[wordIndex * 4]) |
                                              (static_cast<uint32_t>(packedSpan[wordIndex * 4 + 1]) << 8) |
                                              (static_cast<uint32_t>(packedSpan[wordIndex * 4 + 2]) << 16) |
                                              (static_cast<uint32_t>(packedSpan[wordIndex * 4 + 3]) << 24);

                        paletteIndex = (word >> bitOffset) & ((1u << bitsPerBlock) - 1);
                    }

                    const int32_t state = paletteStates[paletteIndex];

                    const int32_t x = (blockIndex >> 8) & 0xF;
                    const int32_t z = (blockIndex >> 4) & 0xF;
                    const int32_t y = blockIndex & 0xF;

                    chunk.setBlock(x, (subChunkY << 4) + y, z, state, storageIndex);
                }
            }
        }

        return chunk;
    }
}
