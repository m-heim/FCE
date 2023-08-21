#include "position.hpp"
#include "bitboard.hpp"
#include "core.hpp"
#include <chrono>
#include <iostream>
#include <string.h>

void Position::setSquare(SquareIndex squareVal, Color colorVal,
                         Piece pieceVal) {
  Piece piece = board[squareVal].piece;
  Color color = board[squareVal].color;
  board[squareVal].color = colorVal;
  board[squareVal].piece = pieceVal;

  Bitboard mask = bitboardSetSquare(squareVal);
  bitboards[color][piece] &= ~mask;      // remove piece from bitboard
  occupation[color] &= ~mask;            // remove piece from occupations
  bitboards[colorVal][pieceVal] |= mask; // add piece to bitboard
  occupation[colorVal] |= mask;          // add piece to occupations
}

Position::Position() {
  for (SquareIndex square = Square::SQUARE_A1; square <= Square::SQUARE_H8;
       square++) {
    board[square].color = Color::NO_COLOR;
    board[square].piece = Piece::NO_PIECE;
  }
  occupation[Color::WHITE] = 0ULL;
  occupation[Color::BLACK] = 0ULL;
  occupation[Color::NO_COLOR] = 0ULL;
  memset(&bitboards, 0, sizeof(bitboards));
}

std::string Position::stringify_board() {
  std::string ret;
  ret.append("  A B C D E F G H\n");
  for (int8_t row = 7; row >= 0; row--) {
    ret.push_back((char)('1' + row));
    for (int8_t col = 0; col <= 7; col++) {
      ret.push_back(' ');
      SquareIndex square = row * 8 + col;
      char piece = piece_to_char(board[square].piece);
      if (board[square].color == Color::WHITE && piece != ' ') {
        piece += 'A' - 'a';
      }
      ret.push_back(piece);
    }
    ret.push_back('\n');
  }
  return ret;
}

SquareInfo::SquareInfo() {
}

Evaluation Position::evaluate(void) {
  Evaluation eval = 0;
  eval += evaluateMaterial();
  eval += evaluatePosition();
  return eval;
}

Evaluation Position::evaluateMaterial(void) {
  std::array<Evaluation, Piece::KING + 1> values;
  values[Piece::PAWN] = 100;
  values[Piece::KNIGHT] = 300;
  values[Piece::BISHOP] = 320;
  values[Piece::ROOK] = 450;
  values[Piece::QUEEN] = 900;
  values[Piece::KING] = 2000000;
  Evaluation eval = 0;

  // NOTE Assuming king is top
  for (uint8_t piece = 0; piece < Piece::KING + 1; piece++) {
    eval += (bitboardGetHW(bitboards[Color::WHITE][piece]) -
             bitboardGetHW(bitboards[Color::BLACK][piece])) *
            values[piece];
    if (piece == Piece::PAWN) {
      for (uint8_t col = 0; col < 8; col++) {
        eval += (bitboardGetHW(bitboards[Color::WHITE][piece]) -
                 bitboardGetHW(bitboards[Color::BLACK][piece])) *
                -50;
      }
    }
  }
  return to_move == Color::WHITE ? eval : -eval;
}

Evaluation Position::evaluatePosition(void) {
  Evaluation eval = 0;
  return eval;
}

bool Position::makeMove(Move m) {
  SquareIndex from = moveGetFrom(m);
  SquareIndex to = moveGetTo(m);
  uint8_t flags = moveGetFlags(m);
  SquareInfo movingPiece = board[from];
  if (flags == MoveFlags::QUIET) {
    setSquare(to, movingPiece.color, movingPiece.piece);
    setSquare(from, Color::NO_COLOR, Piece::NO_PIECE);
    plies_since_capture += 1;
  } else if (flags == MoveFlags::CAPTURE) {
    setSquare(to, movingPiece.color, movingPiece.piece);
    setSquare(from, Color::NO_COLOR, Piece::NO_PIECE);
  }
  plies += 1;
  to_move = to_move == Color::WHITE ? Color::BLACK : Color::WHITE;

  return kingExists();
}

void Position::generatePieceMoves(MoveList &moves) {
  Color opponent = (to_move == Color::BLACK) ? Color::WHITE : Color::BLACK;
  Bitboard ours = occupation[to_move];
  Bitboard theirs = occupation[opponent];
  Bitboard oursOrTheirs = ours | theirs;
  Bitboard neitherOursAndTheirs = ~oursOrTheirs;
  Bitboard knights = bitboards[to_move][Piece::KNIGHT];
  while (knights) {
    SquareIndex from = get_ls1b_index(knights);
    Bitboard attacks = knightAttacks[from] & theirs;
    Bitboard non_attacks = knightAttacks[from] & neitherOursAndTheirs;
    moves.addMoves(from, attacks, MoveFlags::CAPTURE);
    moves.addMoves(from, non_attacks, MoveFlags::QUIET);
    knights = bitboardUnsetSquare(knights, from);
  }
  Bitboard king = bitboards[to_move][Piece::KING];
  while (king) {
    SquareIndex from = get_ls1b_index(king);
    Bitboard attacks = kingAttacks[from] & theirs;
    Bitboard non_attacks = kingAttacks[from] & neitherOursAndTheirs;
    moves.addMoves(from, attacks, MoveFlags::CAPTURE);
    moves.addMoves(from, non_attacks, MoveFlags::QUIET);
    king = bitboardUnsetSquare(king, from);
  }
  Bitboard bishop = bitboards[to_move][Piece::BISHOP];
  while (bishop) {
    SquareIndex from = get_ls1b_index(bishop);
    Bitboard mask = getBishopMask(from);
    Bitboard result = bishopMagics[from].getAttack(mask & oursOrTheirs);
    Bitboard attacks = result & theirs;
    Bitboard quiet = result & neitherOursAndTheirs;
    moves.addMoves(from, attacks, MoveFlags::CAPTURE);
    moves.addMoves(from, quiet, MoveFlags::QUIET);
    bishop = bitboardUnsetSquare(bishop, from);
  }
  Bitboard rook = bitboards[to_move][Piece::ROOK];
  while (rook) {
    SquareIndex from = get_ls1b_index(rook);
    Bitboard mask = getRookMask(from);
    Bitboard result = rookMagics[from].getAttack(mask & oursOrTheirs);
    Bitboard attacks = result & theirs;
    Bitboard quiet = result & neitherOursAndTheirs;
    moves.addMoves(from, attacks, MoveFlags::CAPTURE);
    moves.addMoves(from, quiet, MoveFlags::QUIET);
    rook = bitboardUnsetSquare(rook, from);
  }
}

void Position::generatePawnMoves(MoveList &moves) {
  Bitboard pawns = bitboards[to_move][Piece::PAWN];
  Bitboard pawns_not_on_7 = pawns & notRank7;
  Color opponent = (to_move == Color::BLACK) ? Color::WHITE : Color::BLACK;
  while (pawns_not_on_7) {
    SquareIndex from = get_ls1b_index(pawns_not_on_7);
    Bitboard attacks = occupation[opponent] & pawnAttacks[to_move][from];
    while (attacks) {
      SquareIndex to = get_ls1b_index(attacks);
      moves.push_back(serialize_move(from, to, MoveFlags::CAPTURE));
      attacks = bitboardUnsetSquare(attacks, to);
    }
    Bitboard empty = ~(occupation[opponent] | occupation[to_move]);
    Bitboard pushes = pawnPushes[to_move][from] & empty;
    while (pushes) {
      SquareIndex to = get_ls1b_index(pushes);
      Bitboard doublePush = pawnPushes[to_move][to];
      if (doublePush & empty & rank4) {
        moves.push_back(
            serialize_move(from, get_ls1b_index(doublePush), MoveFlags::QUIET));
      }
      moves.push_back(serialize_move(from, to, MoveFlags::QUIET));
      pushes = bitboardUnsetSquare(pushes, to);
    }
    Bitboard mask = ~bitboardSetSquare(from);
    pawns_not_on_7 &= mask;
  }
}

void Position::generateMoves(MoveList &moves) {
  generatePawnMoves(moves);
  generatePieceMoves(moves);
}

Evaluation negaMax(Position position, uint16_t depth) {
  if (depth == 0 || !position.kingExists()) {
    return position.evaluate();
  }
  Evaluation max = EvaluationLiterals::NEG_INF;
  MoveList moves;
  position.generateMoves(moves);
  // std::cout << "Found" << moves.size() << "moves" << std::endl;
  for (uint8_t i = 0; i < moves.count; i++) {
    Position p = position;
    p.makeMove(moves.get(i));
    // std::cout << p.stringify_board() << std::endl;
    Evaluation current = -negaMax(p, depth - 1);
    // std::cout << current << std::endl;
    if (current > max) {
      // std::cout << "BETTER MOVE" << std::endl;
      max = current;
    }
  }
  return max;
}

Move negaMaxRoot(Position position, uint16_t depth) {
  if (depth == 0 || !position.kingExists()) {
    return position.evaluate();
  }
  Move bestMove = no_move;
  MoveList moves;
  Evaluation max = EvaluationLiterals::NEG_INF;
  Move m;
  position.generateMoves(moves);
  // std::cout << "Found" << moves.size() << "moves" << std::endl;
  for (uint8_t i = 0; i < moves.count; i++) {
    Position p = position;
    m = p.makeMove(moves.get(i));
    Evaluation current = -negaMax(p, depth - 1);
    std::cout << current << std::endl;
    if (current > max) {
      max = current;
      bestMove = m;
    }
  }
  return bestMove;
}

std::array<std::uint64_t, 40> positionsEvaluated;
Evaluation alphaBeta(Position *position, Evaluation alpha, Evaluation beta,
                     uint16_t depthleft) {
  // std::cout << "At depth" << std::to_string(depthleft) << std::endl;
  // std::cout << position->stringify_board() << alpha << beta << std::endl;
  positionsEvaluated[depthleft] += 1;
  if (depthleft == 0) {
    return position->evaluate();
  }
  MoveList moves;
  position->generateMoves(moves);
  Evaluation score;
  for (uint8_t i = 0; i < moves.count; i++) {
    Position p = *position;
    // if the Move made by us leads to the capture of the king
    if (!p.makeMove(moves.get(i))) {
      return p.to_move == Color::BLACK ? -EvaluationLiterals::INVALID_MOVE
                                       : EvaluationLiterals::INVALID_MOVE;
    }
    score = -alphaBeta(&p, -beta, -alpha, depthleft - 1);
    if (score == -EvaluationLiterals::INVALID_MOVE) {
      continue;
    }
    if (score >= beta) {
      return beta; //  fail hard beta-cutoff
    }
    if (score > alpha) {
      alpha = score; // alpha acts like max in MiniMax
      // std::cout << "Found a better Move" << p.stringify_board() << std::endl;
    }
  }
  return alpha;
}

SearchInfo search(Position *position, uint16_t depth) {
  auto start = std::chrono::high_resolution_clock::now();
  MoveList moves;
  position->generateMoves(moves);
  std::cout << std::to_string(moves.count) << std::endl;
  Evaluation best = EvaluationLiterals::NEG_INF;
  Move bestMove = no_move;
  for (uint8_t i = 0; i < moves.count; i++) {
    Position p = *position;
    Move m = moves.get(i);
    p.makeMove(m);
    Evaluation current = -alphaBeta(&p, EvaluationLiterals::NEG_INF,
                                    EvaluationLiterals::POS_INF, depth);
    if (current > best) {
      best = current;
      bestMove = m;
    }
  }
  uint64_t positions = 0;
  for (int i = 0; i < 40; i++) {
    positions += positionsEvaluated[i];
    std::cout << "Depth" << std::to_string(i) << " "
              << std::to_string(positionsEvaluated[i]) << std::endl;
  }
  double rate = (double)positions /
                std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::high_resolution_clock::now() - start)
                    .count() *
                1000;
  std::cout << "Total of:\n"
            << std::to_string(positions) << "\nat:\n"
            << std::to_string(rate) << "MP/s" << std::endl;
  return std::pair<Move, Evaluation>(bestMove, best);
}
