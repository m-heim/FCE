#pragma once
#include <string>

#include "magic.hpp"
#include "position.hpp"

/**
 * @brief Parse a FEN string
 * @exception May fail if FEN invalid!
 * @param fen Reference to FEN string
 * @return Position that equals input FEN string
 */
Position parseFen(const std::string &fen);

/**
 * @brief Read a FEN from a file
 * 
 * @param fenPath The path to the file containing FEN string
 * @return std::string the FEN string read from the file
 */
std::string readFen(const std::string &fenPath);