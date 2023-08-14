#pragma once
#include "bitboard.hpp"
#include "core.hpp"
#include <array>
#include <cstdint>
#include <iostream>
#include <vector>


class Position {
public:
  // BOARD REPRESENTATIONS
  std::array<SquareInfo, Square::SQUARE_COUNT> board;
  std::array<std::array<Bitboard, Piece::PIECE_COUNT>, Color::COLOR_COUNT>
      bitboards;
  Color to_move;
  SquareIndex en_passant;
  uint16_t plies;
  uint16_t plies_since_capture;
  std::array<std::array<bool, Castle::CASTLE_COUNT>, Color::COLOR_COUNT>
      castle_rights;

  void set_square(SquareIndex square, Color color, Piece piece);
  std::string stringify_board();
  double evaluate(void);
  std::vector<move> generateMoves(void);
  std::vector<move> generarePawnMoves(Color side);
};
