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
  board_index en_passant;
  uint16_t move;
  uint16_t moves_since_capture;
  std::array<std::array<bool, Castle::CASTLE_COUNT>, Color::COLOR_COUNT> castle_rights;
};