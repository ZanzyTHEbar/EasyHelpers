#pragma once
#include <ArduinoJson.h>
#include <string>
#include "iter_queue.hpp"
#include "logger.hpp"
#include "observer.hpp"

namespace Helpers {

template <typename EnumT>
class MessageBuffer : public ISubject<EnumT>, public Logger {
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

    /**
     * @brief Get the message object at the front of the queue
     * @note This function will remove the message from the bufferusing `pop()`
     */
    JsonDocument& getMessage() {
        if (buffer.empty())
            // return an empty JsonDocument
            return buffer.front();
        auto message = buffer.front();
        buffer.pop();
        return message;
    }

    /**
     * @brief Get the message object at the front of the queue
     * @note This function will not remove the message from the buffer
     */
    JsonDocument& peekMessage() {
        return buffer.front();
    }

    /**
     * @brief Get the latest message object
     * @note This function will not remove the message from the buffer
     */
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

    size_t size() const {
        return buffer.size();
    }

    void clear() {
        while (!buffer.empty()) {
            buffer.pop();
        }
    }

    template <typename T>
    T serialize(bool iterate = false, bool clearBuffer = false) {
        T result;

        if (buffer.empty()) {
            this->log(LogLevel_t::ERROR, "Buffer is empty");
            return result;
        }

        if (iterate) {
            for (auto& message : buffer) {
                serializeJson(result, message);
            }
        } else {
            serializeJson(result, buffer.front());
        }
        return result;
    }

    template <typename T>
    bool deserialize(const T& data) {
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, data);
        if (err) {
            this->log(LogLevel_t::ERROR, "deserializeJson() failed: ", err);
            return false;
        }

        buffer.push(doc);
        return true;
    }

    virtual std::string getID() const override {
        return "MessageBuffer";
    }
};
}  // namespace Helpers
