#pragma once
#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "freertos/semphr.h"
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
    SemaphoreHandle_t mutex;
    using ObserverPtr_t = std::weak_ptr<IObserver<EnumT> >;
    using ObserversByNameMap_t = std::unordered_map<uint64_t, ObserverPtr_t>;

    ObserversByNameMap_t observers;

   public:
    ISubject() {
        mutex = xSemaphoreCreateMutex();
    }

    virtual ~ISubject() {
        detachAll();
        vSemaphoreDelete(mutex);
    }

    void attach(ObserverPtr_t observerWeak) {
        xSemaphoreTake(mutex, portMAX_DELAY);
        if (auto observer = observerWeak.lock()) {
            observers[observer->getID()] = observerWeak;
        }
        xSemaphoreGive(mutex);
    }

    void detach(const ObserverPtr_t& observerWeak) {
        xSemaphoreTake(mutex, portMAX_DELAY);
        observers.erase(
            std::remove_if(
                observers.begin(), observers.end(),
                [&observerWeak](const std::weak_ptr<IObserver<EnumT> >& o) {
                    return o.lock() == observerWeak.lock();
                }),
            observers.end());
        xSemaphoreGive(mutex);
    }

    void detach(uint64_t observerKey) {
        xSemaphoreTake(mutex, portMAX_DELAY);
        observers.erase(observerKey);
        xSemaphoreGive(mutex);
    }

    void detachAll() {
        xSemaphoreTake(mutex, portMAX_DELAY);
        observers.clear();
        xSemaphoreGive(mutex);
    }

    void notify(uint64_t key, EnumT event) {
        xSemaphoreTake(mutex, portMAX_DELAY);
        for (const auto& [observerKey, observerWeak] : observers) {
            if (observerKey == key) {
                if (auto observer = observerWeak.lock()) {
                    observer->update(event);
                }
            }
        }
        xSemaphoreGive(mutex);
    }

    void notifyAll(EnumT event) {
        xSemaphoreTake(mutex, portMAX_DELAY);
        for (const auto& [observerKey, observerWeak] : observers) {
            if (auto observer = observerWeak.lock()) {
                observer->update(event);
            }
        }
        xSemaphoreGive(mutex);
    }
};
}  // namespace Helpers
