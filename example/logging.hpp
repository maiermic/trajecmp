#ifndef TRAJECTORY_DETECTION_LOGGING_H
#define TRAJECTORY_DETECTION_LOGGING_H

#include <string>
#include <iostream>

namespace logging {
    inline namespace {
        bool is_logging = false;
        std::string text = "";
    }
} // namespace logging

#define START_LOGGING(_text) \
    logging::is_logging = true; \
    logging::text = _text; \
    std::cout << "==================================================\n"; \
    std::cout << "BEGIN: " << _text << std::endl; \
    std::cout << "==================================================\n";

#define STOP_LOGGING() \
    std::cout << "==================================================\n"; \
    std::cout << "END: " << logging::text << std::endl; \
    std::cout << "==================================================\n"; \
    logging::is_logging = false;

#define LOG(expr) \
    if (logging::is_logging) { \
        std::cout << #expr << " := " << (expr) << std::endl; \
    }

#define LOG_TEXT(text) \
    if (logging::is_logging) { \
        std::cout << text << std::endl; \
    }

#define LOG_SEP() \
    LOG_TEXT("--------------------------------------------------")

#endif //TRAJECTORY_DETECTION_LOGGING_H
