#include "util.hpp"
#include <cstdlib>
#include <iostream>
#include <unistd.h>

void fce_error(std::string message, int exit_code) {
  std::cerr << message << std::endl;
  std::exit(exit_code);
}