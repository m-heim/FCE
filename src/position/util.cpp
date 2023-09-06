#include "util.hpp"
#include <iostream>
#include <string>

void fce_error(const std::string &message, int exit_code) {
    std::cerr << message << std::endl;
    std::exit(exit_code);
}