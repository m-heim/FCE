#include "position.hpp"
#include "bitboard.hpp"
#include "core.hpp"
#include <iostream>

void Position::setSquare(SquareIndex squareVal, Color colorVal,
                          Piece pieceVal) {
  board[squareVal].color = colorVal;
  board[squareVal].piece = pieceVal;
  if (pieceVal == Piece::NO_PIECE) {
    occupation[Color::BLACK] ^= 1ULL << squareVal;
    occupation[Color::WHITE] ^= 1ULL << squareVal;
  } else {
    bitboards[colorVal][pieceVal] |= 1ULL << squareVal;
    occupation[colorVal] |= 1ULL << squareVal;
  }
}

std::string Position::stringify_board() {
  std::string ret;
  for (int8_t row = 7; row >= 0; row--) {
    for (int8_t col = 0; col <= 7; col++) {
      SquareIndex square = row * 8 + col;
      char piece = piece_to_char(board[square].piece);
      if (board[square].color == Color::WHITE && piece != ' ') {
        piece += 'A' - 'a';
      }
      ret.push_back(piece);
      if ((square + 1) % 8 == 0) {
        ret.push_back('\n');
      }
    }
  }
  return ret;
}

SquareInfo::SquareInfo() {}

double Position::evaluate(void) { return 0.0; }

std::vector<move> Position::generatePawnMoves(MagicBitboards &magics) {
    std::vector<move> moves;
    Bitboard pawns = bitboards[to_move][Piece::PAWN];
    Bitboard pawns_on_7 = pawns & rank7;
    Bitboard pawns_not_on_7 = pawns & notRank7;
    while (pawns_not_on_7) {
        SquareIndex from = get_ls1b_index(pawns_not_on_7);
        Bitboard attacks = occupation[to_move == Color::BLACK ? Color::WHITE : Color::BLACK] & magics.pawnAttacks[to_move][from];
        while(attacks) {
            SquareIndex to = get_ls1b_index(attacks);
            moves.push_back(serialize_move(from, to, MoveFlags::QUIET));
            Bitboard mask = ~bitboardSetSquare(to);
            attacks &= mask;
        }
        Bitboard mask = ~bitboardSetSquare(from);
        pawns_not_on_7 &= mask;
    }
    //while (pawns_on_7) {
    //  SquareIndex from = get_ls1b_index(pawns_on_7);
    //  
    //}
    return moves;
}

std::vector<move> Position::generateMoves(MagicBitboards &magics) {
  std::vector<move> result;
  for(move m : generatePawnMoves(magics)) {
    result.push_back(m);
  }
  return result;
}

void Position::makeMove(move m) {
  SquareIndex from = moveGetFrom(m);
  SquareIndex to = moveGetTo(m);
  uint8_t flags = moveGetFlags(m);
  SquareInfo movingPiece = board[from];
  if (flags == MoveFlags::QUIET) {
    setSquare(to, movingPiece.color, movingPiece.piece);
    setSquare(from, Color::NO_COLOR, Piece::NO_PIECE);
    plies_since_capture += 1;
  }
  plies += 1;

  return;
}

move Position::search() {
  move bestMove;
  bestMove = no_move;
  return bestMove;
}