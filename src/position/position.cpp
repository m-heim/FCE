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

SquareInfo::SquareInfo() {}

Evaluation Position::evaluate(void) {
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
    eval += (bitboardGetHW(bitboards[Color::WHITE][piece]) - bitboardGetHW(bitboards[Color::BLACK][piece])) * values[piece];
  }
  return to_move == Color::WHITE ? eval : - eval;
}

std::vector<move> Position::generatePieceMoves(MagicBitboards &magics) {
  std::vector<move> moves;
  Color opponent = (to_move == Color::BLACK) ? Color::WHITE : Color::BLACK;
  Bitboard knights = bitboards[to_move][Piece::KNIGHT];
  while (knights) {
    SquareIndex from = get_ls1b_index(knights);
    Bitboard attacks = magics.knightAttacks[from] & occupation[opponent];
    Bitboard non_attacks = magics.knightAttacks[from] & ~(occupation[opponent] | occupation[to_move]);
    while (attacks) {
      SquareIndex to = get_ls1b_index(attacks);
      moves.push_back(serialize_move(from, to, MoveFlags::QUIET));
      attacks = bitboardUnsetSquare(attacks, to);
    }
    while (non_attacks) {
      SquareIndex to = get_ls1b_index(non_attacks);
      moves.push_back(serialize_move(from, to, MoveFlags::QUIET));
      non_attacks = bitboardUnsetSquare(non_attacks, to);
    }
    knights = bitboardUnsetSquare(knights, from);
  }
  return moves;
}

std::vector<move> Position::generatePawnMoves(MagicBitboards &magics) {
  std::vector<move> moves;
  Bitboard pawns = bitboards[to_move][Piece::PAWN];
  Bitboard pawns_on_7 = pawns & rank7;
  Bitboard pawns_not_on_7 = pawns & notRank7;
  Color opponent = (to_move == Color::BLACK) ? Color::WHITE : Color::BLACK;
  while (pawns_not_on_7) {
    SquareIndex from = get_ls1b_index(pawns_not_on_7);
    Bitboard attacks = occupation[opponent] & magics.pawnAttacks[to_move][from];
    while (attacks) {
      SquareIndex to = get_ls1b_index(attacks);
      moves.push_back(serialize_move(from, to, MoveFlags::CAPTURE));
      attacks = bitboardUnsetSquare(attacks, to);
    }
    Bitboard pushes;
    if (to_move == Color::WHITE) {
      pushes = ~(occupation[opponent] | occupation[to_move]) &
                      (bitboardSetSquare(from) << Direction::NORTH);
    } else {
      pushes = ~(occupation[opponent] | occupation[to_move]) &
                      (bitboardSetSquare(from) >> Direction::NORTH);
    }
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
  for (move m : generatePieceMoves(magics)) {
    result.push_back(m);
  }
  return result;
}

void makeMove(Position &position, move m) {
  SquareIndex from = moveGetFrom(m);
  SquareIndex to = moveGetTo(m);
  uint8_t flags = moveGetFlags(m);
  Color opponent = position.to_move == Color::BLACK ? Color::WHITE : Color::BLACK;
  SquareInfo movingPiece = position.board[from];
  if (flags == MoveFlags::QUIET) {
    position.setSquare(to, movingPiece.color, movingPiece.piece);
    position.setSquare(from, Color::NO_COLOR, Piece::NO_PIECE);
    position.plies_since_capture += 1;
  } else if (flags == MoveFlags::CAPTURE) {
    position.setSquare(to, movingPiece.color, movingPiece.piece);
    position.setSquare(from, Color::NO_COLOR, Piece::NO_PIECE);
  }
  position.plies += 1;
  position.to_move = opponent;

  return;
}

Evaluation negaMax(Position position, uint16_t depth, MagicBitboards &magics) {
  Color side = position.to_move;
  if (depth == 0 || !position.kingExists()) {
    return position.evaluate();
  }
  Evaluation max = EvaluationLiterals::NEG_INF;
  std::vector<move> moves = position.generateMoves(magics);
  //std::cout << "Found" << moves.size() << "moves" << std::endl;
  for (move m : moves) {
    Position p = position;
    makeMove(p, m);
    //std::cout << p.stringify_board() << std::endl;
    Evaluation current = - negaMax(p, depth - 1, magics);
    //std::cout << current << std::endl;
    if (current > max) {
      //std::cout << "BETTER MOVE" << std::endl;
      max = current;
    }
  }
  return max;
}

move negaMaxRoot(Position position, uint16_t depth, MagicBitboards &magics) {
  if (depth == 0 || !position.kingExists()) {
    return position.evaluate();
  }
  move bestMove = no_move;
  Evaluation max = EvaluationLiterals::NEG_INF;
  std::vector<move> moves = position.generateMoves(magics);
  for (move m : moves) {
    Position p = position;
    makeMove(p, m);
    Evaluation current = - negaMax(p, depth - 1, magics);
    std::cout << current << std::endl;
    if (current > max) {
      max = current;
      bestMove = m;
    }
  }
  return bestMove;
}

Evaluation alphaBeta(Position *position, Evaluation alpha, Evaluation beta, uint16_t depthleft, MagicBitboards &magics, bool top) {
    //std::cout << position.stringify_board() << alpha << beta << std::endl;
   if( depthleft == 0 or !position->kingExists()) {
    return position->evaluate();
   }
   std::vector<move> moves = position->generateMoves(magics);
   Evaluation score;
   move bestMove = no_move;
   for (move m : moves)  {
      Position p = *position;
      makeMove(p, m);
      score = -alphaBeta(&p, -beta, -alpha, depthleft - 1, magics, false);
      if( score >= beta ) {
         return beta;   //  fail hard beta-cutoff
      }
      if( score > alpha ) {
         alpha = score; // alpha acts like max in MiniMax
         if (top) {
          bestMove = m;
         }
      }
   }
   if (top) {
    std::cout << "Best move" << std::to_string(moveGetFrom(bestMove)) << std::to_string(moveGetTo(bestMove)) << std::endl;
        Position pNew = *position;
        makeMove(pNew, bestMove);
        std::cout << pNew.stringify_board() << std::endl;
   }
   return alpha;
}

Evaluation alphaBetaMax(Position p, Evaluation alpha, Evaluation beta, int depthleft, MagicBitboards &magics) {
   std::cout << p.stringify_board() << std::endl;
   if ( depthleft == 0 || !p.kingExists()) return p.evaluate();
   std::vector<move> moves = p.generateMoves(magics);
   Evaluation score;
   for (move m : moves) {
      Position new_pos = p;
      makeMove(new_pos, m);
      score = alphaBetaMin(new_pos, alpha, beta, depthleft - 1, magics);
      if( score >= beta )
         return beta;   // fail hard beta-cutoff
      if( score > alpha )
         alpha = score; // alpha acts like max in MiniMax
   }
   return alpha;
}

Evaluation alphaBetaMin(Position p, Evaluation alpha, Evaluation beta, int depthleft, MagicBitboards &magics) {
   if ( depthleft == 0 || !p.kingExists()) return -p.evaluate();
   std::vector<move> moves = p.generateMoves(magics);
   Evaluation score;
   for (move m : moves) {
      Position new_pos = p;
      makeMove(new_pos, m);
      score = alphaBetaMax(new_pos, alpha, beta, depthleft - 1, magics);
      if( score <= alpha )
         return alpha; // fail hard alpha-cutoff
      if( score < beta )
         beta = score; // beta acts like min in MiniMax
   }
   return beta;
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