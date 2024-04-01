#pragma once
#include <ArduinoJson.h>
#include <string>
#include "iter_queue.hpp"
#include "observer.hpp"

namespace Helpers {

template <typename EnumT>
class MessageBuffer : public ISubject<EnumT> {
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

    template <typename T>
    T serialize(const JsonDocument& doc) {
        T result;
        serializeJson(doc, result);
        return result;
    }

    template <typename T>
    JsonDocument deserialize(const T& data) {
        JsonDocument doc;
        deserializeJson(doc, data);
        return doc;
    }

    bool isEmpty() const {
        return buffer.empty();
    }
};
}  // namespace Helpers
