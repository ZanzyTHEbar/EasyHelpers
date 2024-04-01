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
    virtual std::string getID() const override = 0;
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
    using ObserversByNameMap_t = std::unordered_map<std::string, ObserverPtr_t>;
    using ObserverKeysMap_t =
        std::unordered_map<std::string, std::vector<std::string> >;

    ObserverKeysMap_t observerKeys;
    ObserversByNameMap_t observers;

   public:
    virtual ~ISubject() {
        detachAll();
    }

    void attach(const std::string& key, ObserverPtr_t observer) {
        observers.emplace(observer->getID(), observer);
        observerKeys[observer->getID()].push_back(key);
    }

    void detach(const ObserverPtr_t& observer) {
        const std::string& name = observer.getID();
        observerKeys.erase(name);
        observers.erase(name);
    }

    void detach(const std::string& observerName) {
        observerKeys.erase(observerName);
        observers.erase(observerName);
    }

    void detachAll() {
        observerKeys.clear();
        observers.clear();
    }

    void notify(const std::string& key, EnumT event) {
        for (const auto& [observerName, keys] : observerKeys) {
            if (std::find(keys.begin(), keys.end(), key) != keys.end()) {
                // access the observer from the map of vectors and notify it
                observers[observerName]->update(event);
            }
        }
    }

    void notifyAll(EnumT event) {
        for (const auto& [observerName, keys] : observerKeys) {
            //* Notify the observer if it's subscribed to the key
            observers[observerName]->update(event);
        }
    }

    std::vector<std::string> getObserverKeys(
        const std::string& observerName) const {
        if (auto it = observerKeys.find(observerName);
            it != observerKeys.end()) {
            return it->second;
        }
        return {};
    }

    //* Members
    int id = 0;
};
}  // namespace Helpers
