#pragma once

#include <string>

/**
 * @brief Raise a critical error and exit
 *
 * @param message The message to display to stdout
 * @param exit_code Exit code, usually 1 or 2
 */
void fce_error(const std::string &message, int exit_code);