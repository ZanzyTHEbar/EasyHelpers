#pragma once
#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "id_interface.hpp"

namespace Helpers {
template <typename EnumT>
class IObserver : public IId {
   public:
    virtual void update(const EnumT& event) = 0;
};

/**
 * @brief
 * @note The `key` parameter is used to determine which Events to notify on.
 * @tparam EnumT
 */
template <typename EnumT>
class ISubject {
   private:
    using ObserverPtr_t = std::shared_ptr<IObserver<EnumT> >;
    using ObserversByNameMap_t = std::unordered_map<uint64_t, ObserverPtr_t>;
    using ObserverKeysMap_t =
        std::unordered_map<uint64_t, std::vector<uint64_t> >;

    ObserverKeysMap_t observerKeys;
    ObserversByNameMap_t observers;

   public:
    virtual ~ISubject() {
        detachAll();
    }

    void attach(uint64_t key, ObserverPtr_t observer) {
        observers.emplace(observer->getID(), observer);
        observerKeys[observer->getID()].push_back(key);
    }

    void detach(const ObserverPtr_t& observer) {
        uint64_t key = observer->getID();
        observerKeys.erase(key);
        observers.erase(key);
    }

    void detach(uint64_t observerKey) {
        observerKeys.erase(observerKey);
        observers.erase(observerKey);
    }

    void detachAll() {
        observerKeys.clear();
        observers.clear();
    }

    void notify(uint64_t key, EnumT event) {
        for (const auto& [observerKey, keys] : observerKeys) {
            if (std::find(keys.begin(), keys.end(), key) != keys.end()) {
                // access the observer from the map of vectors and notify it
                observers[observerKey]->update(event);
            }
        }
    }

    void notifyAll(EnumT event) {
        for (const auto& [observerKey, keys] : observerKeys) {
            //* Notify the observer if it's subscribed to the key
            observers[observerKey]->update(event);
        }
    }

    std::vector<uint64_t> getObserverKeys(uint64_t observerID) const {
        if (auto it = observerKeys.find(observerID); it != observerKeys.end()) {
            return it->second;
        }
        return {};
    }
};
}  // namespace Helpers
