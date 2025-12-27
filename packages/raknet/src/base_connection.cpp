#include <jerv/raknet/base_connection.hpp>
#include <jerv/raknet/proto.hpp>
#include <cstring>
#include <algorithm>
#include <iostream>

namespace jerv::raknet {
    BaseConnection::BaseConnection(SocketSource &source, const AddressInfo &endpoint, uint64_t guid)
        : source_(source)
          , endpoint_(endpoint)
          , guid_(guid)
          , id_(getIdentifierFor(endpoint))
          , outgoingBuffer_(1024 + 512)
          , outgoingToSendStack_(1024)
          , outgoingMTU_(outgoingBuffer_.size()) {
        outgoingBuffer_[0] = VALID_DATAGRAM_BIT;
        incomingLastActivity = std::chrono::steady_clock::now();
    }

    void BaseConnection::handleIncoming(std::span<uint8_t> msg) {
        uint8_t mask = msg[0] & ONLINE_DATAGRAM_BIT_MASK;
        incomingLastActivity = std::chrono::steady_clock::now();

        if (mask == VALID_DATAGRAM_BIT) {
            handleFrameSet(msg);
            return;
        }

        if ((mask & ACK_DATAGRAM_BIT) == ACK_DATAGRAM_BIT) {
            handleAck(msg);
            return;
        }

        if ((mask & NACK_DATAGRAM_BIT) == NACK_DATAGRAM_BIT) {
            handleNack(msg);
            return;
        }
    }

    void BaseConnection::handleAck(std::span<uint8_t> msg) {
        auto ranges = proto::readACKLikePacket(msg);
        for (const auto &range: ranges) {
            for (uint32_t i = range.min; i <= range.max; i++) {
                auto it = outgoingUnacknowledgedCache_.find(i);
                if (it != outgoingUnacknowledgedCache_.end()) {
                    outgoingUnacknowledgedReliableCapsules_ -= it->second.size();
                    outgoingUnacknowledgedCache_.erase(it);
                }
                flushQueuePending_ = true;
            }
        }
    }

    void BaseConnection::handleNack(std::span<uint8_t> msg) {
        auto ranges = proto::readACKLikePacket(msg);
        for (auto it = ranges.rbegin(); it != ranges.rend(); ++it) {
            for (int32_t i = it->max; i >= static_cast<int32_t>(it->min); i--) {
                auto cacheIt = outgoingUnacknowledgedCache_.find(i);
                if (cacheIt != outgoingUnacknowledgedCache_.end()) {
                    for (auto capsuleIt = cacheIt->second.rbegin();
                         capsuleIt != cacheIt->second.rend(); ++capsuleIt) {
                        outgoingToSendStack_.reverseEnqueue(*capsuleIt);
                    }
                    outgoingUnacknowledgedCache_.erase(cacheIt);
                }
            }
            flushQueuePending_ = true;
        }
    }

    void BaseConnection::handleFrameSet(std::span<uint8_t> msg) {
        const uint32_t id = proto::readUint24(msg.data(), 1);

        if (id > 0xFFFFE0) {
            // playing for too long, please don't do that, its unhealthy
            disconnect();
        }

        const uint32_t frameIndex = id & 0xFF;
        const uint32_t correctionIndex = (id + unacknowledgedWindowSize_) & 0xFF;

        incomingReceivedDatagram_.erase(correctionIndex);

        if (incomingReceivedDatagram_[frameIndex]) {
            if (onErrorHandle) {
                onErrorHandle("duplicate frame");
            }
        }

        incomingReceivedDatagram_[frameIndex] = true;
        incomingMissingDatagram_.erase(id);

        if (incomingLastDatagramId_ != static_cast<uint32_t>(-1) && id - incomingLastDatagramId_ > 1) {
            for (uint32_t i = incomingLastDatagramId_ + 1; i < id; i++) {
                incomingMissingDatagram_.insert(i);
            }
        }

        incomingReceivedDatagramAcknowledgeStack_.push_back(id);
        incomingLastDatagramId_ = id;

        size_t offset = 4;
        while (offset < msg.size()) {
            offset = handleCapsule(msg, offset);
        }

        acknowledgePending_ = true;
        processCurrentAcknowledge();
    }

    size_t BaseConnection::handleCapsule(std::span<uint8_t> data, size_t offset) {
        auto capsuleData = proto::readCapsuleFrameData(data, offset);

        FrameDescriptor desc;
        desc.body = capsuleData.body;
        desc.fragment = capsuleData.fragment;
        desc.orderChannel = capsuleData.orderChannel;
        desc.orderIndex = capsuleData.orderIndex;
        desc.reliableIndex = capsuleData.reliableIndex;
        desc.sequenceIndex = capsuleData.sequenceIndex;

        if (desc.fragment) {
            handleFragment(desc);
        } else {
            if (onFrame) {
                onFrame(desc);
            }
        }

        return capsuleData.offset;
    }

    void BaseConnection::handleFragment(FrameDescriptor &data) {
        if (!data.fragment) {
            if (onErrorHandle) {
                onErrorHandle("not a fragment");
            }
            return;
        }

        auto &meta = incomingFragmentRebuildTable_[data.fragment->id];
        meta.set(data.fragment->index, data.body);

        if (meta.length() >= data.fragment->length) {
            auto rebuilt = meta.build();
            incomingFragmentRebuildTable_.erase(data.fragment->id);


            FrameDescriptor rebuiltDesc;
            rebuiltDesc.body = std::span<uint8_t>(rebuilt);
            rebuiltDesc.orderChannel = data.orderChannel;
            rebuiltDesc.orderIndex = data.orderIndex;
            rebuiltDesc.reliableIndex = data.reliableIndex;
            rebuiltDesc.sequenceIndex = data.sequenceIndex;

            if (onFrame) {
                onFrame(rebuiltDesc);
            }
        }
    }

    void BaseConnection::handleFrame(FrameDescriptor &desc) {
        if (onFrame) {
            onFrame(desc);
        }
    }

    void BaseConnection::processCurrentAcknowledge() {
        if (!incomingReceivedDatagramAcknowledgeStack_.empty()) {
            auto ranges = getRangesFromSequence(incomingReceivedDatagramAcknowledgeStack_);
            auto buffer = proto::rentAcknowledgePacketWith(
                static_cast<uint8_t>(UnconnectedPacketId::AckDatagram), ranges
            );
            sendToSocket(buffer);
            incomingReceivedDatagramAcknowledgeStack_.clear();
        }

        if (!incomingMissingDatagram_.empty()) {
            std::vector<uint32_t> missing(incomingMissingDatagram_.begin(), incomingMissingDatagram_.end());
            auto ranges = getRangesFromSequence(missing);
            auto buffer = proto::rentAcknowledgePacketWith(
                static_cast<uint8_t>(UnconnectedPacketId::NackDatagram), ranges
            );
            sendToSocket(buffer);
            incomingMissingDatagram_.clear();
        }
    }

    void BaseConnection::enqueueFrame(std::span<const uint8_t> data, uint8_t reliability) {
        flushQueuePending_ = true;

        if (outgoingOrderChannels_.find(outgoingChannelIndex_) == outgoingOrderChannels_.end()) {
            outgoingOrderChannels_[outgoingChannelIndex_] = 0;
            outgoingSequenceChannels_[outgoingChannelIndex_] = 0;
        }

        FrameDescriptor meta;
        meta.orderChannel = outgoingChannelIndex_;

        if (IS_SEQUENCED_LOOKUP[reliability]) {
            meta.orderIndex = outgoingOrderChannels_[outgoingChannelIndex_];
            meta.sequenceIndex = outgoingSequenceChannels_[outgoingChannelIndex_]++;
        }

        if (IS_ORDERED_EXCLUSIVE_LOOKUP[reliability]) {
            meta.orderIndex = outgoingOrderChannels_[outgoingChannelIndex_]++;
            outgoingSequenceChannels_[outgoingChannelIndex_] = 0;
        }


        if (data.size() >= outgoingMTU_ - MAX_FRAME_SET_HEADER_SIZE) {
            size_t chunkSize = outgoingMTU_ - MAX_FRAME_SET_HEADER_SIZE;
            size_t fragmentCount = (data.size() + chunkSize - 1) / chunkSize;
            uint16_t id = outgoingNextFragmentId_++;

            auto chunks = proto::getChunks(data, chunkSize);
            uint32_t index = 0;

            for (const auto &chunk: chunks) {
                FrameDescriptor fragMeta = meta;
                auto *fragInfo = new FrameDescriptor::FragmentInfo{id, index, static_cast<uint32_t>(fragmentCount)};
                fragMeta.fragment = fragInfo;
                fragMeta.body = std::span<uint8_t>(const_cast<uint8_t *>(chunk.data()), chunk.size());
                fragMeta.reliableIndex = outgoingReliableIndex_++;

                enqueueCapsule(fragMeta, reliability);
                index++;
            }
            return;
        }

        meta.reliableIndex = outgoingReliableIndex_++;
        meta.body = std::span<uint8_t>(const_cast<uint8_t *>(data.data()), data.size());
        enqueueCapsule(meta, reliability);
    }

    void BaseConnection::enqueueCapsule(const FrameDescriptor &descriptor, const uint8_t reliability) {
        CapsuleCache cache;
        cache.frame = descriptor;
        cache.reliability = reliability;
        outgoingToSendStack_.enqueue(cache);
    }

    void BaseConnection::processQueue() {
        while (!outgoingToSendStack_.isEmpty() &&
               outgoingUnacknowledgedReliableCapsules_ < unacknowledgedWindowSize_) {
            auto capsule = outgoingToSendStack_.dequeue();

            size_t availableSize = outgoingMTU_ - outgoingBufferCursor_;
            if (availableSize <= MAX_CAPSULE_HEADER_SIZE + capsule.frame.body.size()) {
                batchCurrentBuffer();
            }

            processCapsule(capsule);
        }

        if (outgoingBufferCursor_ > 4) {
            flushStackPending_ = true;
            batchCurrentBuffer();
        }
    }

    void BaseConnection::processCapsule(CapsuleCache &data) {
        if (data.reliability != static_cast<uint8_t>(Reliability::Unreliable) &&
            data.reliability != static_cast<uint8_t>(Reliability::UnreliableSequenced)) {
            outgoingUnacknowledgedStack_.push_back(data);
            outgoingUnacknowledgedReliableCapsules_++;
        }

        outgoingBufferCursor_ = proto::writeCapsuleFrameHeader(
            outgoingBufferCursor_,
            outgoingBuffer_.data(),
            data.frame,
            data.frame.body.size(),
            data.reliability
        );

        std::memcpy(outgoingBuffer_.data() + outgoingBufferCursor_,
                    data.frame.body.data(), data.frame.body.size());
        outgoingBufferCursor_ += data.frame.body.size();
    }

    void BaseConnection::batchCurrentBuffer() {
        if (outgoingBufferCursor_ <= 4) return;

        outgoingBuffer_[0] = VALID_DATAGRAM_BIT;
        outgoingUnacknowledgedCache_[outgoingFrameSetId_] = std::move(outgoingUnacknowledgedStack_);
        outgoingUnacknowledgedStack_.clear();

        proto::writeUint24(outgoingBuffer_.data(), 1, outgoingFrameSetId_++);
        sendToSocket(std::span<const uint8_t>(outgoingBuffer_.data(), outgoingBufferCursor_));
        outgoingBufferCursor_ = 4;
    }

    void BaseConnection::sendToSocket(std::span<const uint8_t> data) {
        source_.send(data, endpoint_);
    }

    void BaseConnection::close() {
        processQueue();
        batchCurrentBuffer();
        processCurrentAcknowledge();
    }

    void BaseConnection::disconnect() {
        std::vector disconnectPacket = {static_cast<uint8_t>(ConnectedPacketId::Disconnect)};
        enqueueFrame(disconnectPacket, static_cast<uint8_t>(Reliability::Unreliable));
        close();
    }

    void BaseConnection::send(std::span<const uint8_t> data, uint8_t reliability) {
        enqueueFrame(data, reliability);
        processQueue();
    }

    std::vector<std::pair<uint32_t, uint32_t> > BaseConnection::getRangesFromSequence(
        const std::vector<uint32_t> &sequence) {
        std::vector<std::pair<uint32_t, uint32_t> > ranges;
        if (sequence.empty()) return ranges;

        uint32_t min = sequence[0];
        uint32_t max = min;

        for (size_t i = 1; i < sequence.size(); i++) {
            uint32_t current = sequence[i];
            if (current == max + 1) {
                max = current;
            } else {
                ranges.push_back({min, max});
                min = max = current;
            }
        }
        ranges.push_back({min, max});

        return ranges;
    }
}