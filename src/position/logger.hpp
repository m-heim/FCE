#pragma once

#include <string>

enum LOG_LEVEL {
    DEBUG,
    FAIL,
    INFO,
    ERROR,
};

void fce_log(std::string &message, LOG_LEVEL level);