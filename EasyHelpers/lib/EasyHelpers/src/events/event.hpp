#pragma once

#include <helpers/iter_queue.hpp>
#include <helpers/logger.hpp>
#include <helpers/observer.hpp>
#include <memory>
#include "event_interface.hpp"

namespace Helpers {
/**
 * @brief Custom Event Manager
 * @tparam EnumT The Enum Type for the Event
 * @note This class is a custom event manager that can be used to manage
 * multiple strategies, this class implementes mutex for thread safety, ensure
 * to properly handle the mutex in the derived class with the overriden method.
 */
template <typename EnumT>
class CustomEventManager
    : public std::enable_shared_from_this<CustomEventManager<EnumT> >,
      public Logger,
      public IObserver<EnumT> {
    using Strategy_t = std::shared_ptr<IEvent<EnumT> >;
    using StrategyQueue_t = iter_queue<Strategy_t>;

   protected:
    //* Queue for the strategies
    SemaphoreHandle_t mutex;
    StrategyQueue_t strategyQueue;

   public:
    CustomEventManager(const std::string& label) {
        mutex = xSemaphoreCreateMutex();
        this->setLabel(label);
    }

    virtual ~CustomEventManager() {
        this->stop();
        vSemaphoreDelete(mutex);
    }

    /**
     * @brief Initialize all strategies
     * @note This will call the begin method for all strategies
     */
    virtual void begin() {
        xSemaphoreTake(mutex, portMAX_DELAY);
        this->log("Initializing Strategies");

        // check if the queue is empty
        if (strategyQueue.empty()) {
            this->log(LogLevel_t::ERROR, "No strategies found");
            return;
        }

        for (auto& strategy : strategyQueue) {
            this->log(LogLevel_t::DEBUG, "Strategy ID: ", strategy->getID());
            strategy->begin();
        }
        xSemaphoreGive(mutex);
    }

    /**
     * @brief Stop all strategies
     * @note This will remove all strategies from the queue and set the queue to
     * an empty instance
     */
    virtual void stop() {
        xSemaphoreTake(mutex, portMAX_DELAY);
        while (!strategyQueue.empty()) {
            strategyQueue.pop();
        }
        strategyQueue = StrategyQueue_t();
        this->log("Strategies Stopped");
        xSemaphoreGive(mutex);
    }

    /**
     * @brief Add a subscriber to the queue
     * @param strategy The strategy to add
     * @note This will add the strategy to the queue
     */
    virtual void addSubscriber(Strategy_t strategy) {
        if (!strategy)
            return;  // Safety check

        // Convert this instance into a shared_ptr before converting to weak_ptr
        auto selfSharedPtr = this->shared_from_this();
        auto selfWeakPtr =
            std::weak_ptr<CustomEventManager<EnumT> >(selfSharedPtr);

        // Use the revised attach method
        strategy->attach(selfWeakPtr);

        xSemaphoreTake(mutex, portMAX_DELAY);
        this->strategyQueue.emplace(strategy);
        xSemaphoreGive(mutex);
    }

    /**
     * @brief Remove a subscriber from the queue
     * @param strategy The strategy to remove
     * @note This will remove the strategy from the queue
     */
    virtual void removeSubscriber(Strategy_t strategy) {
        xSemaphoreTake(mutex, portMAX_DELAY);

        if (!strategy)
            return;  // Safety check

        StrategyQueue_t tempQueue;
        while (!strategyQueue.empty()) {
            auto strategyFront = strategyQueue.front();

            if (strategyFront.get()->getID() != strategy->getID()) {
                tempQueue.emplace(std::move(strategyFront));
            }

            // detach the strategy
            strategyFront.get()->detach(this->getID());
            strategyQueue.pop();
        }
        strategyQueue = std::move(tempQueue);

        xSemaphoreGive(mutex);
    }

    /**
     * @brief Call in a loop to handle all strategies sequentially
     * @note Here we call all Strategies for the API
     */
    virtual void handleStrtegies() {
        xSemaphoreTake(mutex, portMAX_DELAY);

        if (strategyQueue.empty()) {
            this->log(LogLevel_t::ERROR, "No strategies found");
            return;
        }

        for (auto& event : strategyQueue) {
            event->receiveMessage();
        }

        xSemaphoreGive(mutex);
    }

    /**
     * @brief Handle a specific strategy
     * @param strategy The strategy to handle
     * @note This will call the receiveMessage method for the strategy
     */
    virtual void handleStrategy(Strategy_t strategy) {
        xSemaphoreTake(mutex, portMAX_DELAY);

        if (strategyQueue.empty()) {
            this->log(LogLevel_t::ERROR, "No strategies found");
            return;
        }

        auto _strategy = strategyQueue.find(strategy);
        if (_strategy != strategyQueue.cend()) {
            strategy->receiveMessage();
            return;
        }

        this->log(LogLevel_t::ERROR, "Strategy not found");

        xSemaphoreGive(mutex);
    }

    //* Overrides

    virtual void update(const EnumT& event) override = 0;
};
}  // namespace Helpers