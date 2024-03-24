#pragma once

#include <string>

enum LOG_LEVEL {
    DEBUG,
    FAIL,
    INFO,
    ERROR,
};

void fce_log(LOG_LEVEL level, std::string message);