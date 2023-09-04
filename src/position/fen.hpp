#pragma once
#include <string>

#include "magic.hpp"
#include "position.hpp"

Position parseFen(const std::string &fen);

std::string readFen(const std::string &fenPath);