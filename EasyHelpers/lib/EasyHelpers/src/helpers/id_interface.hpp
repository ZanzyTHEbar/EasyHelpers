#pragma once

#include <atomic>
#include <string>

class IId {
    inline static std::atomic<uint64_t> idCounter{0};
    uint64_t mID;

   public:
    IId() {
        mID = idCounter++;
    }
    virtual ~IId() = default;

    void setID(uint64_t id) {
        mID = id;
    }

    uint64_t getID() const {
        return mID;
    }
};
