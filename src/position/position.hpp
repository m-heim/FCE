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
  std::array<std::array<Bitboard, Piece::PIECE_TOP + 1>, Color::COLOR_TOP + 1>
      bitboards;
  std::array<Bitboard, Color::COLOR_TOP + 1> occupation;
  Color to_move;
  SquareIndex en_passant;
  uint16_t plies;
  uint16_t plies_since_capture;
  std::array<std::array<bool, Castle::CASTLE_TOP + 1>, Color::BLACK + 1>
      castle_rights;

  Position();
  void setSquare(SquareIndex square, Color color, Piece piece);
  std::string stringify_board();
  Evaluation evaluate(void);
  Evaluation evaluateMaterial(void);
  Evaluation evaluatePosition(void);
  bool makeMove(Move m);
  inline bool kingExists() {
    return bitboards[to_move][Piece::KING] != 0;
  }
  std::vector<Move> generatePawnMoves();
  std::vector<Move> generatePieceMoves();
  std::vector<Move> generateMoves();
  bool makeMove(Position &position, Move m);
};

Evaluation negaMax(Position position, uint16_t depth);
Move negaMaxRoot(Position position, uint16_t depth);
Evaluation alphaBeta(Position *position, Evaluation alpha, Evaluation beta, uint16_t depthleft);
SearchInfo search(Position *position, uint16_t depth);