#include "util.hpp"
#include "logger.hpp"
#include <iostream>
#include <string>

void fce_error(const std::string &message, int exit_code) {
    fce_log(LOG_LEVEL::ERROR, message);
    std::exit(exit_code);
}
