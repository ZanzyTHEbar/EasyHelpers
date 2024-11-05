#pragma once
#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "freertos/semphr.h"
#include "id_interface.hpp"

namespace Helpers {

template <typename EnumT, typename PayloadT = void>
class IObserver : public IId {
   public:
    virtual void update(const EnumT& event, const PayloadT& payload) = 0;
};

template <typename EnumT>
class IObserver<EnumT, void> : public IId {
   public:
    virtual void update(const EnumT& event) = 0;
};

template <typename EnumT, typename PayloadT = void>
class ISubject {
   private:
    SemaphoreHandle_t mutex;
    using ObserverPtr_t = std::weak_ptr<IObserver<EnumT, PayloadT> >;
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
                [&observerWeak](
                    const std::weak_ptr<IObserver<EnumT, PayloadT> >& o) {
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

    // Notify method with payload (only enabled when PayloadT is not void)
    template <typename T = PayloadT>
    typename std::enable_if<!std::is_void<T>::value>::type notify(
        uint64_t key, EnumT event, const T& payload) {
        xSemaphoreTake(mutex, portMAX_DELAY);
        for (const auto& [observerKey, observerWeak] : observers) {
            if (observerKey != key) {
                continue;
            }

            if (auto observer = observerWeak.lock()) {
                observer->update(event, payload);
            }
        }
        xSemaphoreGive(mutex);
    }

    // Notify all observers with payload (only enabled when PayloadT is not
    // void)
    template <typename T = PayloadT>
    typename std::enable_if<!std::is_void<T>::value>::type notifyAll(
        EnumT event, const T& payload) {
        xSemaphoreTake(mutex, portMAX_DELAY);
        for (const auto& [observerKey, observerWeak] : observers) {
            if (auto observer = observerWeak.lock()) {
                observer->update(event, payload);
            }
        }
        xSemaphoreGive(mutex);
    }

    // Notify method without payload (only enabled when PayloadT is void)
    template <typename T = PayloadT>
    typename std::enable_if<std::is_void<T>::value>::type notify(uint64_t key,
                                                                 EnumT event) {
        xSemaphoreTake(mutex, portMAX_DELAY);
        for (const auto& [observerKey, observerWeak] : observers) {
            if (observerKey != key) {
                continue;
            }

            if (auto observer = observerWeak.lock()) {
                observer->update(event);
            }
        }
        xSemaphoreGive(mutex);
    }

    // Notify all observers without payload (only enabled when PayloadT is void)
    template <typename T = PayloadT>
    typename std::enable_if<std::is_void<T>::value>::type notifyAll(
        EnumT event) {
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
