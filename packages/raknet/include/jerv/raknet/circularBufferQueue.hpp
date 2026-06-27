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

#include <vector>

namespace jerv::raknet {
    template<typename T>
    class CircularBufferQueue {
    public:
        explicit CircularBufferQueue(size_t capacity)
            : buffer_(capacity)
              , headCursor_(0)
              , tailCursor_(0)
              , size_(0) {
        }

        bool reverseEnqueue(T item) {
            if (size_ == buffer_.size()) return false;
            tailCursor_ = (buffer_.size() + tailCursor_ - 1) % buffer_.size();
            buffer_[tailCursor_] = std::move(item);
            size_++;
            return true;
        }

        bool enqueue(T item) {
            if (size_ == buffer_.size()) return false;
            buffer_[headCursor_] = std::move(item);
            headCursor_ = (headCursor_ + 1) % buffer_.size();
            size_++;
            return true;
        }

        T dequeue() {
            if (size_ == 0) return T{};
            T item = std::move(buffer_[tailCursor_]);
            tailCursor_ = (tailCursor_ + 1) % buffer_.size();
            size_--;
            return item;
        }

        T peek() const {
            return buffer_[tailCursor_];
        }

        bool isEmpty() const {
            return size_ == 0;
        }

        size_t getSize() const {
            return size_;
        }
    private:
        std::vector<T> buffer_;
        size_t headCursor_;
        size_t tailCursor_;
        size_t size_;
    };
}