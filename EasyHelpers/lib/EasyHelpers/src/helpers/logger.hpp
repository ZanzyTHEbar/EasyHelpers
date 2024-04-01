#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "helpers.hpp"
#include "id_interface.hpp"

namespace Helpers {
class Logger : public IId {
   public:
    enum LogLevel_e : uint8_t {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS
    };

   protected:
    std::string checkLogLevel(LogLevel_e log_level) {
        static const std::array<std::string, NUM_LOG_LEVELS> logLevelStrings = {
            "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

        if (log_level >= 0 && log_level < NUM_LOG_LEVELS)
            return logLevelStrings[log_level];

        return "UNKNOWN";
    }

    template <typename... Args>
    std::string handleInput(Args... args) {
        std::stringstream ss;
        // Fold expression to concatenate arguments
        (ss << ... << args);
        std::string message = ss.str();
        return message;
    }

   public:
    Logger() = default;
    virtual ~Logger() = default;

    virtual std::string getID() const override {
        return "Logger";
    }

    // Templated log function to handle various data types and arguments
    template <typename... Args>
    void log(LogLevel_e log_level, Args... args) {
        std::string message = handleInput(args...);
        std::string logLevel = checkLogLevel(log_level);
        std::string logMessage =
            Helpers::format_string("[%s - %s]: %s", logLevel.c_str(),
                                   this->getID().c_str(), message.c_str());
        std::cout << logMessage << '\n';
    }
    template <typename... Args>
    void log(Args... args) {
        std::string message = handleInput(args...);
        std::string logLevel = checkLogLevel(LogLevel_e::INFO);
        std::string logMessage =
            Helpers::format_string("[%s - %s]: %s", logLevel.c_str(),
                                   this->getID().c_str(), message.c_str());
        std::cout << logMessage << '\n';
    }
};
using LogLevel_t = Logger::LogLevel_e;
}  // namespace Helpers
