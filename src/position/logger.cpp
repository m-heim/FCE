#include "logger.hpp"
#include <iostream>

void fce_log(std::string &message, LOG_LEVEL level) {
    std::string prefix;
    if (level == LOG_LEVEL::DEBUG) {
        prefix = "[DEBUG]: ";
    } else if (level == LOG_LEVEL::INFO) {
        prefix = "[INFO]: ";
    } else if (level == LOG_LEVEL::FAIL) {
        prefix = "[FAIL]: ";
    } else if (level == LOG_LEVEL::ERROR) {
        prefix = "[ERROR]: ";
    }
    std::cout << prefix << message;
}