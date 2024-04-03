#pragma once

#include <helpers/iter_queue.hpp>
#include <helpers/logger.hpp>
#include <helpers/observer.hpp>
#include <memory>
#include "event_interface.hpp"

namespace Helpers {
template <typename EnumT>
class CustomEventManager
    : public std::enable_shared_from_this<CustomEventManager<EnumT> >,
      public Logger,
      public IObserver<EnumT> {
    using Strategy_t = std::shared_ptr<IEvent<EnumT> >;
    using StrategyQueue_t = iter_queue<Strategy_t>;

   protected:
    //* Queue for the strategies
    StrategyQueue_t strategyQueue;

   public:
    CustomEventManager(const std::string& label) {
        this->setLabel(label);
    }
    virtual ~CustomEventManager() {
        this->stop();
    }

    /**
     * @brief Initialize all strategies
     * @note This will call the begin method for all strategies
     */
    virtual void begin() {
        this->log("Initializing Strategies");

        // check if the queue is empty
        if (strategyQueue.empty()) {
            this->log(LogLevel_t::ERROR, "No strategies found");
            return;
        }

        for (auto& strategy : strategyQueue) {
            //* Call the begin method for each strategy
            this->log(LogLevel_t::DEBUG, "Strategy ID: ", strategy->getID());
            strategy->begin();
        }
    }

    /**
     * @brief Stop all strategies
     * @note This will remove all strategies from the queue and set the queue to
     * an empty instance
     */
    virtual void stop() {
        while (!strategyQueue.empty()) {
            strategyQueue.pop();
        }
        strategyQueue = StrategyQueue_t();
        this->log("Strategies Stopped");
    }

    /**
     * @brief Add a subscriber to the queue
     * @param strategy The strategy to add
     * @note This will add the strategy to the queue
     */
    virtual void addSubscriber(const Strategy_t&& strategy) {
        this->log(LogLevel_t::DEBUG, "Adding Strategy: ", strategy->getID());
        strategy->attach(this->getID(), this->shared_from_this());
        this->log(LogLevel_t::DEBUG, "Strategy Attached: ", strategy->getID());
        strategyQueue.emplace(std::move(strategy));
        this->log(LogLevel_t::DEBUG, "Strategy Added: ", strategy->getID());
    }

    /**
     * @brief Remove a subscriber from the queue
     * @param strategy The strategy to remove
     * @note This will remove the strategy from the queue
     */
    virtual void removeSubscriber(const Strategy_t&& strategy) {
        StrategyQueue_t tempQueue;
        while (!strategyQueue.empty()) {
            if (strategyQueue.front().get()->getID() !=
                strategy.get()->getID()) {
                // detach the strategy
                strategyQueue.front().get()->detach(this->shared_from_this());
                tempQueue.emplace(std::move(strategyQueue.front()));
            }
            strategyQueue.pop();
        }
        strategyQueue = std::move(tempQueue);
    }

    /**
     * @brief Call in a loop to handle all strategies sequentially
     * @note Here we call all Strategies for the API
     */
    virtual void handleStrtegies() {
        while (!strategyQueue.empty()) {
            for (auto& event : strategyQueue) {
                event->receiveMessage();
            }
        }
    }

    /**
     * @brief Handle a specific strategy
     * @param strategy The strategy to handle
     * @note This will call the receiveMessage method for the strategy
     */
    virtual void handleStrategy(const Strategy_t&& strategy) {
        if (strategyQueue.empty()) {
            this->log(LogLevel_t::ERROR, "No strategies found");
            return;
        }

        auto _strategy = strategyQueue.find(strategy);
        if (_strategy != strategyQueue.cend()) {
            _strategy->get()->receiveMessage();
            return;
        }

        this->log(LogLevel_t::ERROR, "Strategy not found");
    }

    //* Overrides

    virtual void update(const EnumT& event) override = 0;
};
}  // namespace Helpers