#pragma once
#include "bitboard.hpp"
#include "board.hpp"
#include "core.hpp"

#include <array>

class Game {
public:
  std::array<std::array<Bitboard, Piece::PIECE_COUNT>, Color::COLOR_COUNT>
      bitboards;
  Board board;
  Color to_move;
  uint16_t move;
  uint16_t moves_since_capture;
};