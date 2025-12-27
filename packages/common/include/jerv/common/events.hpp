#pragma once

#include <functional>
#include <vector>
#include <map>
#include <memory>
#include <typeindex>
#include <any>

namespace jerv::common {
    template<typename... Args>
    class Event {
    public:
        using Handler = std::function<void(Args...)>;
        using HandlerId = size_t;

        HandlerId subscribe(Handler handler) {
            HandlerId id = nextId_++;
            handlers_[id] = std::move(handler);
            return id;
        }

        void unsubscribe(HandlerId id) {
            handlers_.erase(id);
        }

        void emit(Args... args) const {
            for (const auto &[id, handler]: handlers_) {
                handler(args...);
            }
        }

        void clear() {
            handlers_.clear();
        }

        size_t listenerCount() const {
            return handlers_.size();
        }

    private:
        std::map<HandlerId, Handler> handlers_;
        HandlerId nextId_ = 0;
    };


    class EventEmitter {
    public:
        template<typename EventType, typename... Args>
        void on(std::function<void(Args...)> handler) {
            auto &event = getOrCreateEvent<EventType, Args...>();
            event.subscribe(std::move(handler));
        }

        template<typename EventType, typename... Args>
        void emit(Args... args) {
            auto it = events_.find(typeid(EventType));
            if (it != events_.end()) {
                auto *event = std::any_cast<Event<Args...> *>(it->second);
                if (event) {
                    event->emit(std::forward<Args>(args)...);
                }
            }
        }

    private:
        template<typename EventType, typename... Args>
        Event<Args...> &getOrCreateEvent() {
            auto &eventAny = events_[typeid(EventType)];
            if (!eventAny.has_value()) {
                eventAny = new Event<Args...>();
            }
            return *std::any_cast<Event<Args...> *>(eventAny);
        }

        std::map<std::type_index, std::any> events_;
    };
}