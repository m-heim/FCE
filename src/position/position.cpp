#include "position.hpp"
#include "bitboard.hpp"
#include "core.hpp"
#include "magic.hpp"
#include <chrono>
#include <iostream>
#include <string.h>

void Position::setSquare(SquareIndex squareVal, Color colorVal,
                         Piece pieceVal) {
    Piece piece = board[squareVal].piece;
    Color color = board[squareVal].color;
    board[squareVal].color = colorVal;
    board[squareVal].piece = pieceVal;

    Bitboard mask = maskedSquare[squareVal];
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

inline Evaluation Position::evaluate() {
    Evaluation eval = 0;
    eval += evaluateMaterial();
    eval += evaluatePosition();
    return eval;
}

inline Evaluation Position::evaluateMaterial() {
    // NOTE Assuming king is top
    Evaluation eval = EvaluationLiterals::EVEN;
    for (uint8_t piece = 0; piece <= Piece::KING; piece++) {
        eval += (bitboardGetHW(bitboards[Color::WHITE][piece]) -
                 bitboardGetHW(bitboards[Color::BLACK][piece])) *
                evaluations[piece];
        if (piece == Piece::PAWN) {
            // doubled pawns
            for (uint8_t col = 0; col < Square::SQUARE_A2; col++) {
                Bitboard file = fileAttacks[col];
                eval -=
                    (std::min<int>(
                        0, bitboardGetHW(bitboards[Color::WHITE][Piece::PAWN] &
                                         file) -
                               1)) *
                    50;
                eval +=
                    (std::min<int>(
                        0, bitboardGetHW(bitboards[Color::BLACK][Piece::PAWN] &
                                         file) -
                               1)) *
                    50;
            }
        }
    }
    return (to_move == Color::WHITE) ? eval : -eval;
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
    Move m = no_move;
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
    positionsEvaluated.at(depthleft) += 1;
    if (depthleft == 0) {
        return position->evaluate();
    }
    MoveList moves;
    position->generateMoves(moves);
    Evaluation score = 0;
    bool gotChecked = false;
    uint8_t legalMoves = 0;
    for (uint8_t i = 0; i < moves.count; i++) {
        Position positionWithMyMove = *position;
        // if the Move made by us leads to the capture of the king
        if (!positionWithMyMove.makeMove(moves.get(i))) {
            // notify the callee
            return EvaluationLiterals::INVALID_MOVE;
        }
        score = -alphaBeta(&positionWithMyMove, -beta, -alpha, depthleft - 1);
        if (score == -EvaluationLiterals::INVALID_MOVE) {
            // if we are in check
            gotChecked = true;
            continue;
        }
        legalMoves++;
        // std::cout << p.stringify_board() << std::endl;
        if (score >= beta) {
            return beta; //  fail hard beta-cutoff
        }
        if (score > alpha) {
            // std::cout << p.stringify_board() << std::endl;
            alpha = score; // alpha acts like max in MiniMax
            // std::cout << "Found a better Move" << p.stringify_board() <<
            // std::endl;
        }
    }
    if (legalMoves == 0) {
      return EvaluationLiterals::NEG_INF;
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
