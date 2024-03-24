#pragma once

#include <string>

enum LOG_LEVEL {
    DEBUG,
    VERBOSE,
    REGULAR,
    FAIL,
    INFO,
    ERROR,
    LEVEL_COUNT
};

void fce_log(LOG_LEVEL level, std::string message);