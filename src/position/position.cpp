#include "position.hpp"
#include "bitboard.hpp"
#include "core.hpp"
#include <iostream>

void Position::setSquare(SquareIndex squareVal, Color colorVal,
                         Piece pieceVal) {
  Piece piece = board[squareVal].piece;
  Color color = board[squareVal].color;
  board[squareVal].color = colorVal;
  board[squareVal].piece = pieceVal;
  Bitboard mask = bitboardSetSquare(squareVal);
  bitboards[color][piece] &= ~mask;
  occupation[color] &= ~mask;
  bitboards[colorVal][pieceVal] |= mask;
  occupation[colorVal] |= mask;
}

Position::Position() {
  for(SquareIndex square = Square::SQUARE_A1; square <= Square::SQUARE_H8; square++) {
    board[square].color = Color::NO_COLOR;
    board[square].piece = Piece::NO_PIECE;
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
    Color opponent = (to_move == Color::BLACK) ? Color::WHITE : Color::BLACK;
    Bitboard attacks = occupation[opponent] & magics.pawnAttacks[to_move][from];
    while (attacks) {
      SquareIndex to = get_ls1b_index(attacks);
      moves.push_back(serialize_move(from, to, MoveFlags::CAPTURE));
      attacks = bitboardUnsetSquare(attacks, to);
    }
    Bitboard pushes = ~(occupation[opponent] | occupation[to_move]) &
                      (bitboardSetSquare(from) << Direction::NORTH);
    while (pushes) {
      SquareIndex to = get_ls1b_index(pushes);
      moves.push_back(serialize_move(from, to, MoveFlags::QUIET));
      pushes = bitboardUnsetSquare(pushes, to);
    }
    Bitboard mask = ~bitboardSetSquare(from);
    pawns_not_on_7 &= mask;
  }
  // while (pawns_on_7) {
  //   SquareIndex from = get_ls1b_index(pawns_on_7);
  //
  // }
  return moves;
}

std::vector<move> Position::generateMoves(MagicBitboards &magics) {
  std::vector<move> result;
  for (move m : generatePawnMoves(magics)) {
    result.push_back(m);
  }
  return result;
}

void makeMove(Position &position, move m) {
  SquareIndex from = moveGetFrom(m);
  SquareIndex to = moveGetTo(m);
  uint8_t flags = moveGetFlags(m);
  std::cout << "Making move" << from << to << std::endl;
  SquareInfo movingPiece = position.board[from];
  if (flags == MoveFlags::QUIET) {
    std::cout << "Moving" << std::to_string(from) << std::to_string(to) << std::endl;
    position.setSquare(to, movingPiece.color, movingPiece.piece);
    position.setSquare(from, Color::NO_COLOR, Piece::NO_PIECE);
    position.plies_since_capture += 1;
  } else if (flags == MoveFlags::CAPTURE) {
    position.setSquare(to, movingPiece.color, movingPiece.piece);
    position.setSquare(from, Color::NO_COLOR, Piece::NO_PIECE);
  }
  position.plies += 1;

  return;
}

std::vector<Position> makeMoves(Position position, std::vector<move> moves) {
  std::vector<Position> positions;
  for (auto m : moves) {
    Position position_new = position;
    makeMove(position_new, m);
    positions.push_back(position_new);
  }
  return positions;
}


move Position::search() {
  move bestMove;
  bestMove = no_move;
  return bestMove;
}