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

#pragma once
#include <cstdint>
#include <map>
#include <ranges>
#include <span>
#include <vector>

namespace jerv::raknet {
    class FragmentMeta {
    public:
        void set(uint32_t index, std::span<uint8_t> data) {
            if (fragments.contains(index)) {
                return;
            }
            fragments[index] = std::vector(data.begin(), data.end());
            totalSize += data.size();
        }

        size_t length() const {
            return fragments.size();
        }

        std::vector<uint8_t> build() const {
            std::vector<uint8_t> result;
            result.reserve(totalSize);

            for (const auto &fragment: fragments | std::views::values) {
                result.insert(result.end(), fragment.begin(), fragment.end());
            }

            return result;
        }

    private:
        std::map<uint32_t, std::vector<uint8_t> > fragments;
        size_t totalSize = 0;
    };
}