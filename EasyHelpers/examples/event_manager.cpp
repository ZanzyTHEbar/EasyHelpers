#include <Arduino.h>
#include <EasyHelpers.h>

enum class EventID { EVENT_1, EVENT_2 };

//! Very basic example of the CustomEventManager.

class Strategy1 : public Helpers::Logger, public Helpers::IEvent<EventID> {
   public:
    Strategy1() : Helpers::IEvent<EventID>() {
        this->setLabel("Strategy 1");
        this->repeat = true;
    }

    void begin() override {
        this->log("Strategy 1 Begin");
    }

    void sendMessage(const JsonDocument& message) override {
        this->log("Strategy 1 Send Message");
    }

    void receiveMessage() override {
        this->log("Strategy 1 Receive Message");
    }
};

class Strategy2 : public Helpers::IEvent<EventID>, private Helpers::Logger {
   public:
    Strategy2() : Helpers::IEvent<EventID>() {
        this->setLabel("Strategy 2");
    }

    void begin() override {
        this->log("Strategy 2 Begin");
        this->notify(100, EventID::EVENT_1);
    }

    void sendMessage(const JsonDocument& message) override {
        this->log("Strategy 2 Send Message");
    }

    void receiveMessage() override {
        this->log("Strategy 2 Receive Message");
    }
};

class EventManager : public Helpers::CustomEventManager<EventID> {
    //* setup as shared pointers
    std::shared_ptr<Strategy1> strategy1Ptr;
    std::shared_ptr<Strategy2> strategy2Ptr;

    enum class StrategyID : uint64_t { STRATEGY_1, STRATEGY_2 };

   public:
    EventManager()
        : Helpers::CustomEventManager<EventID>("EventManager"),
          strategy1Ptr(std::make_shared<Strategy1>()),
          strategy2Ptr(std::make_shared<Strategy2>()) {
        this->setID(100);
    }

    void begin() {
        this->log("Initializing Strategies");

        //* Set the ID for each strategy
        this->log("Setting Strategy IDs");
        this->strategy1Ptr->setID(
            static_cast<uint64_t>(StrategyID::STRATEGY_1));
        this->strategy2Ptr->setID(
            static_cast<uint64_t>(StrategyID::STRATEGY_2));

        this->log("Attaching Strategies with ID: ", this->getID());

        //* Add the strategies to the event manager Message Queue
        this->log("Adding Strategies to Queue");
        this->addSubscriber(this->strategy1Ptr);
        this->addSubscriber(this->strategy2Ptr);

        this->log(Helpers::LogLevel_t::DEBUG,
                  "Strategies Initialized: ", strategyQueue.size());

        Helpers::CustomEventManager<EventID>::begin();
    }

    void update(const EventID& event) override {
        switch (event) {
            case EventID::EVENT_1:
                this->log("Event 1 Received");
                break;
            case EventID::EVENT_2:
                this->log("Event 2 Received");
                break;
            default:
                break;
        }
    }
};

/**
 * @brief Event Manager
 * @note This is a shared pointer to the EventManager
 * @note This is required to be a shared pointer
 */
std::shared_ptr<EventManager> eventManager = std::make_shared<EventManager>();

void setup() {
    Serial.begin(115200);
    pinMode(4, OUTPUT);

    digitalWrite(4, HIGH);

    eventManager->begin();

    delay(1000);
}

void loop() {
    //* Loop through the strategies
    eventManager->handleStrtegies();
    delay(1000);
}