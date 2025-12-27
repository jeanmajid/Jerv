#pragma once

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace jerv::binary {
    class cursor {
    public:
        static cursor create(const std::span<uint8_t> buffer) {
            return cursor(buffer);
        }

        static cursor create(std::vector<uint8_t> &buffer) {
            return cursor(std::span(buffer));
        }

        explicit cursor(const std::span<uint8_t> buffer)
            : buffer_(buffer), pointer_(0) {
        }


        cursor getEncapsulation(const size_t length) const {
            return cursor(getSliceSpan(length));
        }

        std::span<uint8_t> getSliceSpan(const size_t length) const {
            return buffer_.subspan(pointer_, length);
        }

        std::span<uint8_t> readSliceSpan(const size_t length) {
            auto span = buffer_.subspan(pointer_, length);
            pointer_ += length;
            return span;
        }

        std::span<uint8_t> getRemainingBytes() const {
            return buffer_.subspan(pointer_);
        }

        std::span<uint8_t> getProcessedBytes() const {
            return buffer_.subspan(0, pointer_);
        }

        size_t processedBytesSize() const { return pointer_; }

        uint8_t readUint8() {
            return buffer_[pointer_++];
        }

        bool readBool() {
            return readUint8() != 0;
        }

        uint16_t readUint16(const bool littleEndian = false) {
            uint16_t value;
            if (littleEndian) {
                value = static_cast<uint16_t>(buffer_[pointer_]) |
                        (static_cast<uint16_t>(buffer_[pointer_ + 1]) << 8);
            } else {
                value = (static_cast<uint16_t>(buffer_[pointer_]) << 8) |
                        static_cast<uint16_t>(buffer_[pointer_ + 1]);
            }
            pointer_ += 2;
            return value;
        }

        uint32_t readUint32(const bool littleEndian = false) {
            uint32_t value;
            if (littleEndian) {
                value = static_cast<uint32_t>(buffer_[pointer_]) |
                        (static_cast<uint32_t>(buffer_[pointer_ + 1]) << 8) |
                        (static_cast<uint32_t>(buffer_[pointer_ + 2]) << 16) |
                        (static_cast<uint32_t>(buffer_[pointer_ + 3]) << 24);
            } else {
                value = (static_cast<uint32_t>(buffer_[pointer_]) << 24) |
                        (static_cast<uint32_t>(buffer_[pointer_ + 1]) << 16) |
                        (static_cast<uint32_t>(buffer_[pointer_ + 2]) << 8) |
                        static_cast<uint32_t>(buffer_[pointer_ + 3]);
            }
            pointer_ += 4;
            return value;
        }

        uint64_t readBigUint64(const bool littleEndian = false) {
            uint64_t value;
            if (littleEndian) {
                value = static_cast<uint64_t>(buffer_[pointer_]) |
                        (static_cast<uint64_t>(buffer_[pointer_ + 1]) << 8) |
                        (static_cast<uint64_t>(buffer_[pointer_ + 2]) << 16) |
                        (static_cast<uint64_t>(buffer_[pointer_ + 3]) << 24) |
                        (static_cast<uint64_t>(buffer_[pointer_ + 4]) << 32) |
                        (static_cast<uint64_t>(buffer_[pointer_ + 5]) << 40) |
                        (static_cast<uint64_t>(buffer_[pointer_ + 6]) << 48) |
                        (static_cast<uint64_t>(buffer_[pointer_ + 7]) << 56);
            } else {
                value = (static_cast<uint64_t>(buffer_[pointer_]) << 56) |
                        (static_cast<uint64_t>(buffer_[pointer_ + 1]) << 48) |
                        (static_cast<uint64_t>(buffer_[pointer_ + 2]) << 40) |
                        (static_cast<uint64_t>(buffer_[pointer_ + 3]) << 32) |
                        (static_cast<uint64_t>(buffer_[pointer_ + 4]) << 24) |
                        (static_cast<uint64_t>(buffer_[pointer_ + 5]) << 16) |
                        (static_cast<uint64_t>(buffer_[pointer_ + 6]) << 8) |
                        static_cast<uint64_t>(buffer_[pointer_ + 7]);
            }
            pointer_ += 8;
            return value;
        }

        float readFloat32(const bool littleEndian = false) {
            const uint32_t bits = readUint32(littleEndian);
            float value;
            std::memcpy(&value, &bits, sizeof(float));
            return value;
        }

        double readFloat64(const bool littleEndian = false) {
            const uint64_t bits = readBigUint64(littleEndian);
            double value;
            std::memcpy(&value, &bits, sizeof(double));
            return value;
        }

        int32_t readZigZag32() {
            uint32_t value = 0;
            for (int i = 0, shift = 0; i < 5; i++, shift += 7) {
                const uint8_t byte = readUint8();
                value |= static_cast<uint32_t>(byte & 0x7F) << shift;
                if ((byte & 0x80) == 0) {
                    return static_cast<int32_t>((value >> 1) ^ -static_cast<int32_t>(value & 1));
                }
            }
            throw std::runtime_error("ZigZag32 too long: exceeds 5 bytes");
        }

        int64_t readZigZag64() {
            uint64_t value = 0;
            for (int i = 0, shift = 0; i < 10; i++, shift += 7) {
                const uint8_t byte = readUint8();
                value |= static_cast<uint64_t>(byte & 0x7F) << shift;
                if ((byte & 0x80) == 0) {
                    return static_cast<int64_t>((value >> 1) ^ -static_cast<int64_t>(value & 1));
                }
            }
            throw std::runtime_error("ZigZag64 too long: exceeds 10 bytes");
        }

        void writeUint8(const uint8_t value) {
            buffer_[pointer_++] = value;
        }

        void writeBool(const bool value) {
            writeUint8(value ? 1 : 0);
        }

        void writeUint16(const uint16_t value, const bool littleEndian = false) {
            if (littleEndian) {
                buffer_[pointer_] = static_cast<uint8_t>(value);
                buffer_[pointer_ + 1] = static_cast<uint8_t>(value >> 8);
            } else {
                buffer_[pointer_] = static_cast<uint8_t>(value >> 8);
                buffer_[pointer_ + 1] = static_cast<uint8_t>(value);
            }
            pointer_ += 2;
        }

        void writeInt16(const int16_t value, const bool littleEndian = false) {
            writeUint16(static_cast<uint16_t>(value), littleEndian);
        }

        void writeUint32(const uint32_t value, const bool littleEndian = false) {
            if (littleEndian) {
                buffer_[pointer_] = static_cast<uint8_t>(value);
                buffer_[pointer_ + 1] = static_cast<uint8_t>(value >> 8);
                buffer_[pointer_ + 2] = static_cast<uint8_t>(value >> 16);
                buffer_[pointer_ + 3] = static_cast<uint8_t>(value >> 24);
            } else {
                buffer_[pointer_] = static_cast<uint8_t>(value >> 24);
                buffer_[pointer_ + 1] = static_cast<uint8_t>(value >> 16);
                buffer_[pointer_ + 2] = static_cast<uint8_t>(value >> 8);
                buffer_[pointer_ + 3] = static_cast<uint8_t>(value);
            }
            pointer_ += 4;
        }

        void writeInt32(const int32_t value, const bool littleEndian = false) {
            writeUint32(static_cast<uint32_t>(value), littleEndian);
        }

        void writeBigUint64(const uint64_t value, const bool littleEndian = false) {
            if (littleEndian) {
                buffer_[pointer_] = static_cast<uint8_t>(value);
                buffer_[pointer_ + 1] = static_cast<uint8_t>(value >> 8);
                buffer_[pointer_ + 2] = static_cast<uint8_t>(value >> 16);
                buffer_[pointer_ + 3] = static_cast<uint8_t>(value >> 24);
                buffer_[pointer_ + 4] = static_cast<uint8_t>(value >> 32);
                buffer_[pointer_ + 5] = static_cast<uint8_t>(value >> 40);
                buffer_[pointer_ + 6] = static_cast<uint8_t>(value >> 48);
                buffer_[pointer_ + 7] = static_cast<uint8_t>(value >> 56);
            } else {
                buffer_[pointer_] = static_cast<uint8_t>(value >> 56);
                buffer_[pointer_ + 1] = static_cast<uint8_t>(value >> 48);
                buffer_[pointer_ + 2] = static_cast<uint8_t>(value >> 40);
                buffer_[pointer_ + 3] = static_cast<uint8_t>(value >> 32);
                buffer_[pointer_ + 4] = static_cast<uint8_t>(value >> 24);
                buffer_[pointer_ + 5] = static_cast<uint8_t>(value >> 16);
                buffer_[pointer_ + 6] = static_cast<uint8_t>(value >> 8);
                buffer_[pointer_ + 7] = static_cast<uint8_t>(value);
            }
            pointer_ += 8;
        }

        void writeInt64(const int64_t value, const bool littleEndian = false) {
            writeBigUint64(static_cast<uint64_t>(value), littleEndian);
        }

        void writeFloat32(const float value, const bool littleEndian = false) {
            uint32_t bits;
            std::memcpy(&bits, &value, sizeof(float));
            writeUint32(bits, littleEndian);
        }

        void writeFloat64(const double value, const bool littleEndian = false) {
            uint64_t bits;
            std::memcpy(&bits, &value, sizeof(double));
            writeBigUint64(bits, littleEndian);
        }

        void writeZigZag32(const int32_t value) {
            uint32_t encoded = (static_cast<uint32_t>(value) << 1) ^ static_cast<uint32_t>(value >> 31);
            for (int i = 0; i < 5; i++) {
                if ((encoded & ~0x7FUL) == 0) {
                    writeUint8(static_cast<uint8_t>(encoded));
                    return;
                }
                writeUint8(static_cast<uint8_t>((encoded & 0x7F) | 0x80));
                encoded >>= 7;
            }
        }

        void writeZigZag64(const int64_t value) {
            uint64_t encoded = (static_cast<uint64_t>(value) << 1) ^ static_cast<uint64_t>(value >> 63);
            for (int i = 0; i < 10; i++) {
                if ((encoded & ~0x7FULL) == 0) {
                    writeUint8(static_cast<uint8_t>(encoded));
                    return;
                }
                writeUint8(static_cast<uint8_t>((encoded & 0x7F) | 0x80));
                encoded >>= 7;
            }
        }

        void writeSliceSpan(const std::span<const uint8_t> value) {
            std::memcpy(buffer_.data() + pointer_, value.data(), value.size());
            pointer_ += value.size();
        }


        std::string readUuid() {
            std::array<uint8_t, 16> bytes{};

            for (int i = 7; i >= 0; i--) {
                bytes[i] = readUint8();
            }
            for (int i = 15; i >= 8; i--) {
                bytes[i] = readUint8();
            }

            char hex[37];
            std::snprintf(hex, sizeof(hex),
                          "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                          bytes[0], bytes[1], bytes[2], bytes[3],
                          bytes[4], bytes[5], bytes[6], bytes[7],
                          bytes[8], bytes[9], bytes[10], bytes[11],
                          bytes[12], bytes[13], bytes[14], bytes[15]);
            return std::string(hex);
        }


        void writeUuid(const std::string &value) {
            std::string uuidNoDashes;
            uuidNoDashes.reserve(value.size());
            for (const char c: value) {
                if (c != '-') uuidNoDashes += c;
            }

            if (uuidNoDashes.size() < 32) {
                uuidNoDashes.insert(0, 32 - uuidNoDashes.size(), '0');
            }

            std::array<uint8_t, 16> bytes{};
            for (size_t i = 0; i < 16; i++) {
                bytes[i] = static_cast<uint8_t>(
                    std::stoul(uuidNoDashes.substr(i * 2, 2), nullptr, 16)
                );
            }

            for (int i = 7; i >= 0; i--) {
                writeUint8(bytes[i]);
            }
            for (int i = 15; i >= 8; i--) {
                writeUint8(bytes[i]);
            }
        }


        std::string readString() {
            const int32_t length = readVarInt();
            const auto span = readSliceSpan(static_cast<size_t>(length));
            return std::string(reinterpret_cast<const char *>(span.data()), length);
        }


        void writeString(const std::string &value) {
            writeVarInt(static_cast<int32_t>(value.size()));
            writeSliceSpan(std::span<const uint8_t>(
                reinterpret_cast<const uint8_t *>(value.data()), value.size()
            ));
        }


        int32_t readVarInt() {
            int32_t num = 0;
            for (int i = 0, shift = 0; i < 5; i++, shift += 7) {
                const uint8_t byte = readUint8();
                num |= (byte & 0x7F) << shift;
                if ((byte & 0x80) == 0) {
                    return num;
                }
            }
            throw std::runtime_error("VarInt32 too long: exceeds 5 bytes");
        }


        void writeVarInt(const int32_t value) {
            uint32_t uvalue = static_cast<uint32_t>(value);
            for (int i = 0; i < 5; i++) {
                if ((uvalue & ~0x7F) == 0) {
                    writeUint8(static_cast<uint8_t>(uvalue));
                    return;
                }
                writeUint8(static_cast<uint8_t>((uvalue & 0x7F) | 0x80));
                uvalue >>= 7;
            }
        }

        bool isEndOfStream() const {
            return pointer_ >= buffer_.size();
        }

        size_t availableSize() const {
            return buffer_.size() - pointer_;
        }

        cursor &reset() {
            pointer_ = 0;
            return *this;
        }

        size_t pointer() const { return pointer_; }
        void setPointer(const size_t pos) { pointer_ = pos; }

        std::span<uint8_t> buffer() const { return buffer_; }

    private:
        std::span<uint8_t> buffer_;
        size_t pointer_;
    };


    class ResizableCursor : public cursor {
    public:
        ResizableCursor(const size_t size, const size_t maxSize)
            : cursor(std::span<uint8_t>())
              , ownedBuffer_(size)
              , maxSize_(maxSize) {
            updateSpan();
        }

        void grow() {
            size_t newSize = ownedBuffer_.size() * 2;
            if (newSize > maxSize_) {
                newSize = maxSize_;
            }
            const size_t currentPointer = pointer();
            ownedBuffer_.resize(newSize);
            updateSpan();
            setPointer(currentPointer);
        }

        void growToFit(const size_t size) {
            while (availableSize() < size) {
                grow();
            }
        }

        std::vector<uint8_t> &ownedBuffer() { return ownedBuffer_; }

    private:
        void updateSpan() {
            static_cast<cursor &>(*this) = cursor(std::span(ownedBuffer_));
        }

        std::vector<uint8_t> ownedBuffer_;
        size_t maxSize_;
    };
}