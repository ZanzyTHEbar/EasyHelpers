#pragma once
#include <ArduinoJson.h>
#include <helpers/strategy.hpp>

namespace Helpers {
template <typename EnumT>
class IEvent : public MessageBuffer<EnumT> {
   public:
    virtual void begin() {}
    virtual void sendMessage(const JsonDocument& message) {}
    virtual void receiveMessage() {}
    bool repeat = false;
};
}  // namespace Helpers