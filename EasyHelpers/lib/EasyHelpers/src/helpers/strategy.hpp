#pragma once
#include <ArduinoJson.h>
#include <string>
#include "iter_queue.hpp"
#include "logger.hpp"
#include "observer.hpp"

namespace Helpers {
class MessageBuffer : public ISubject<CommunicationEvents_e>, public Logger {
    iter_queue<JsonDocument> buffer;

   public:
    MessageBuffer() : buffer() {}
    virtual ~MessageBuffer() {
        while (!buffer.empty()) {
            buffer.pop();
        }
    }

    MessageBuffer& operator=(const MessageBuffer& other) {
        if (this != &other) {
            buffer = other.buffer;
        }
        return *this;
    }

    virtual void begin() {}
    virtual void sendMessage(const std::string& message) {}
    virtual void receiveMessage() {}

    void addMessage(const JsonDocument& message) {
        buffer.push(message);
    }

    JsonDocument& getMessage() {
        if (buffer.empty())
            // return an empty JsonDocument
            return buffer.front();
        auto message = buffer.front();
        buffer.pop();
        return message;
    }

    JsonDocument& peekMessage() {
        return buffer.front();
    }

    JsonDocument& getLatestMessage() {
        if (buffer.empty())
            // return an empty JsonDocument
            return buffer.front();
        return buffer.back();
    }

    MessageBuffer& getInstance() {
        return *this;
    }
    bool isEmpty() const {
        return buffer.empty();
    }

    bool repeat = false;
};
}  // namespace Helpers
