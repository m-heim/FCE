#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include "util.hpp"

void fce_error(std::string message, int exit_code) {
    std::cerr << message << std::endl;
    std::exit(exit_code);
}