#include "position.hpp"
#include "core.hpp"
#include <iostream>

void Position::set_square(SquareIndex squareVal, Color colorVal, Piece pieceVal) {
  board[squareVal].color = colorVal;
  board[squareVal].piece = pieceVal;
  bitboards[colorVal][pieceVal] |= 1ULL << squareVal;
}

std::string Position::stringify_board() {
  std::string ret;
  for (SquareIndex square = Square::SQUARE_A1; square < Square::SQUARE_H8; square++) {
    char piece = piece_to_char(board[square].piece);
    if (Color::BLACK && piece != ' ') {
        piece += 'A' - 'a';
    }
    ret.push_back(piece);
    if (square % 8 == 0) {
        ret.push_back('\n');
    }
  }
  return ret;
}

SquareInfo::SquareInfo() {}

double Position::evaluate(void) { return 0.0; }

std::vector<move> Position::generateMoves() {
  std::vector<move> result;
  return result;
}

std::vector<move> Position::generarePawnMoves(Color side) {
  std::vector<move> moves;
  Bitboard pawns = bitboards[side][Piece::PAWN];
  Bitboard pawns_on_7 = pawns & rank7;
  Bitboard pawns_not_on_7 = pawns & ~rank7;

  Bitboard pawn_attacks = (pawns & notFileA) << 9 & (pawns & notFileH) << 7;
  Bitboard pawn_moves = (pawns);
  return moves;
}