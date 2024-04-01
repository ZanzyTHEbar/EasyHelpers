#pragma once

#include <string>

class IId {
   public:
    virtual ~IId() = default;
    virtual std::string getID() const = 0;
};
