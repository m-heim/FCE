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
  std::array<std::array<Bitboard, 7>, 3>
      bitboards;
  std::array<Bitboard, 3> occupation;
  Color to_move;
  SquareIndex en_passant;
  uint16_t plies;
  uint16_t plies_since_capture;
  std::array<std::array<bool, 4>, 2>
      castle_rights;

  Position();
  void setSquare(SquareIndex square, Color color, Piece piece);
  std::string stringify_board();
  double evaluate(void);
  void makeMove(move m);
  move search();
  std::vector<move> generatePawnMoves(MagicBitboards &magics);
  std::vector<move> generatePieceMoves(MagicBitboards &magics);
  std::vector<move> generateMoves(MagicBitboards &magics);
};
std::vector<Position> makeMoves(Position p, std::vector<move> moves);
