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
    using Strategy_t = iter_queue<std::shared_ptr<IEvent<EnumT> > >;

   protected:
    std::string _id;
    //* Queue for the strategies
    Strategy_t strategyQueue;

   public:
    CustomEventManager(const std::string& id) : _id(id) {}
    virtual ~CustomEventManager() {
        this->stop();
        while (!strategyQueue.empty()) {
            strategyQueue.pop();
        }
    }

    virtual void begin() {
        this->log("Initializing Strategies");

        // check if the queue is empty
        if (strategyQueue.empty()) {
            this->log(LogLevel_t::ERROR, "No strategies found");
            return;
        }

        for (auto& strategy : strategyQueue) {
            //* Call the begin method for each strategy
            this->log(LogLevel_t::DEBUG, "Strategy ID: ", strategy->id);
            strategy->begin();
        }
    }

    virtual void stop() {
        while (!strategyQueue.empty()) {
            strategyQueue.pop();
        }
        strategyQueue = Strategy_t();
        this->log("Strategies Stopped");
    }

    virtual void addSubscriber(
        const std::shared_ptr<MessageBuffer<EnumT> >&& strategy) {
        strategyQueue.emplace(std::move(strategy));
    }

    virtual void removeSubscriber(
        const std::shared_ptr<MessageBuffer<EnumT> >&& strategy) {
        Strategy_t tempQueue;
        while (!strategyQueue.empty()) {
            if (strategyQueue.front().get()->id != strategy.get()->id) {
                tempQueue.emplace(std::move(strategyQueue.front()));
            }
            strategyQueue.pop();
        }
        strategyQueue = std::move(tempQueue);
    }

    /**
     * @brief Call in the Arduino loop to handle events
     * @note Here we call all Strategies for the API
     */
    virtual void handleEvents() {
        while (!strategyQueue.empty()) {
            for (auto& event : strategyQueue) {
                event->receiveMessage();
            }
        }
    }

    //* Overrides

    virtual void update(const EnumT& event) override = 0;

    std::string getID() const override {
        return this->_id;
    }
};
}  // namespace Helpers