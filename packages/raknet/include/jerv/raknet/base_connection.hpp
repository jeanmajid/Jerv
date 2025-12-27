#pragma once

#include <jerv/raknet/interfaces.hpp>
#include <jerv/raknet/constants.hpp>
#include <jerv/raknet/enums.hpp>
#include <jerv/raknet/fragment_meta.hpp>
#include <jerv/common/deferred_runner.hpp>

#include <map>
#include <set>
#include <vector>
#include <functional>
#include <memory>
#include <chrono>

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

    struct CapsuleCache {
        FrameDescriptor frame;
        uint8_t reliability;
    };

    class BaseConnection : public Connection {
    public:
        static std::string getIdentifierFor(const AddressInfo &address) {
            return address.address + "#" + std::to_string(address.port);
        }

        BaseConnection(SocketSource &source, const AddressInfo &endpoint, uint64_t guid);
        virtual ~BaseConnection() = default;

        const std::string &id() const override { return id_; }
        uint64_t guid() const { return guid_; }
        const AddressInfo &endpoint() const { return endpoint_; }

        void send(std::span<const uint8_t> data, uint8_t reliability) override;

        void disconnect() override;

        void handleIncoming(std::span<uint8_t> msg);

        std::function<void(const std::string &)> onErrorHandle;
        std::function<void()> onConnectionEstablishedHandle;

        std::chrono::steady_clock::time_point incomingLastActivity;

    protected:
        void handleFrame(FrameDescriptor &desc);
        void enqueueFrame(std::span<const uint8_t> data, uint8_t reliability);
        void sendToSocket(std::span<const uint8_t> data);
        void close();
        void processQueue();

    public:
        std::function<void(FrameDescriptor &)> onFrame;

    private:
        void handleAck(std::span<uint8_t> msg);
        void handleNack(std::span<uint8_t> msg);
        void handleFrameSet(std::span<uint8_t> msg);
        size_t handleCapsule(std::span<uint8_t> data, size_t offset);
        void handleFragment(FrameDescriptor &data);
        void processCurrentAcknowledge();
        void enqueueCapsule(const FrameDescriptor &descriptor, uint8_t reliability);
        void processCapsule(CapsuleCache &data);
        void batchCurrentBuffer();
        static std::vector<std::pair<uint32_t, uint32_t> > getRangesFromSequence(const std::vector<uint32_t> &sequence);

    protected:
        SocketSource &source_;
        AddressInfo endpoint_;
        uint64_t guid_;
        std::string id_;

        std::map<uint16_t, FragmentMeta> incomingFragmentRebuildTable_;
        std::map<uint32_t, bool> incomingReceivedDatagram_;
        std::vector<uint32_t> incomingReceivedDatagramAcknowledgeStack_;
        std::set<uint32_t> incomingMissingDatagram_;
        int32_t incomingLastDatagramId_ = -1;

        std::vector<uint8_t> outgoingBuffer_;
        std::map<uint32_t, std::vector<CapsuleCache> > outgoingUnacknowledgedCache_;
        CircularBufferQueue<CapsuleCache> outgoingToSendStack_;
        size_t outgoingUnacknowledgedReliableCapsules_ = 0;
        std::vector<CapsuleCache> outgoingUnacknowledgedStack_;
        size_t outgoingBufferCursor_ = 4;
        uint32_t outgoingFrameSetId_ = 0;
        uint8_t outgoingChannelIndex_ = 0;
        uint16_t outgoingNextFragmentId_ = 0;
        std::map<uint8_t, uint32_t> outgoingOrderChannels_;
        std::map<uint8_t, uint32_t> outgoingSequenceChannels_;
        uint32_t outgoingReliableIndex_ = 0;
        size_t outgoingMTU_;

        size_t unacknowledgedWindowSize_ = 512;

        bool flushStackPending_ = false;
        bool flushQueuePending_ = false;
        bool acknowledgePending_ = false;
    };
}