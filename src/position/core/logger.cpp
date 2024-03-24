#include "logger.hpp"
#include <iostream>

void fce_log(LOG_LEVEL level, std::string message) {
    std::string prefix;
    if (level == LOG_LEVEL::DEBUG) {
        prefix = "[DEBUG]: ";
    } else if (level == LOG_LEVEL::INFO) {
        prefix = "[INFO]: ";
    } else if (level == LOG_LEVEL::FAIL) {
        prefix = "[FAIL]: ";
    } else if (level == LOG_LEVEL::ERROR) {
        prefix = "[ERROR]: ";
    } else if (level == LOG_LEVEL::REGULAR) {
        prefix = "";
    }
    std::cout << prefix << message << std::endl;
}