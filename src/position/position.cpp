#include "position.hpp"
#include "core.hpp"
#include <iostream>

void Position::set_square(SquareIndex squareVal, Color colorVal,
                          Piece pieceVal) {
  board[squareVal].color = colorVal;
  board[squareVal].piece = pieceVal;
  bitboards[colorVal][pieceVal] |= 1ULL << squareVal;
}

std::string Position::stringify_board() {
  std::string ret;
  for (SquareIndex square = Square::SQUARE_A1; square <= Square::SQUARE_H8;
       square++) {
    char piece = piece_to_char(board[square].piece);
    if (board[square].color == Color::BLACK && piece != ' ') {
      piece += 'A' - 'a';
    }
    ret.push_back(piece);
    if ((square + 1) % 8 == 0) {
      ret.push_back('\n');
    }
  }
  return ret;
}

SquareInfo::SquareInfo() {}

double Position::evaluate(void) { return 0.0; }

template<Color side> std::vector<move> Position::generatePawnMoves() {
    std::vector<move> moves;
    Bitboard pawns = bitboards[side][Piece::PAWN];
    Bitboard pawns_on_7 = pawns & rank7;
    Bitboard pawns_not_on_7 = pawns & ~rank7;

    while (pawns_not_on_7) {
        SquareIndex from = get_ls1b_index(pawns);
        Bitboard attacks = 0; //occupation[side] & pawnAttacks[side][from];
        while(attacks) {
            SquareIndex to = get_ls1b_index(attacks);
            moves.push_back(serialize_move(from, to, NO_PROMOTION));
        }
    }
    return moves;
}

template<Color side> std::vector<move> Position::generateMoves() {
  std::vector<move> result;
  for(move m : generatePawnMoves<side>()) {
    result.push_back(m);
  }
  return result;
}

void Position::makeMoves() {
  if (to_move == Color::WHITE) {
    std::vector<move> moves = generateMoves<Color::WHITE>();
  }
  return;
}

move Position::search() {

}