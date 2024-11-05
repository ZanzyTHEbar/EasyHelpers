#pragma once
#include <ArduinoJson.h>
#include <helpers/id_interface.hpp>
#include <helpers/message_buffer.hpp>

namespace Helpers {
template <typename EnumT>
class IEvent : public IId, public MessageBuffer<EnumT> {
   public:
    virtual void begin() {}
    virtual void sendMessage(const JsonDocument& message) {}
    virtual void receiveMessage() {}
    bool repeat = false;
};
}  // namespace Helpers