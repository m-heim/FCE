#include "position.hpp"
#include "bitboard.hpp"
#include "chess.hpp"
#include "magic.hpp"
#include "move.hpp"
#include "zobrist.hpp"
#include <chrono>
#include <cstring>
#include <iostream>

void Position::setSquare(SquareIndex squareVal, Color colorVal, Piece pieceVal) {
    Piece oldPiece = board[squareVal].piece;
    Color oldColor = board[squareVal].color;
    board[squareVal].color = colorVal;
    board[squareVal].piece = pieceVal;

    Bitboard mask = maskedSquare[squareVal];
    Bitboard unmasked = unmaskedSquare[squareVal];
    bitboards[oldColor][oldPiece] &= unmasked; // remove piece from bitboard
    occupation[oldColor] &= unmasked;          // remove piece from occupations

    bitboards[colorVal][pieceVal] |= mask; // add piece to bitboard
    occupation[colorVal] |= mask;          // add piece to occupations
}

Position::Position() {
    SquareInfo empty = SquareInfo(Color::NO_COLOR, Piece::NO_PIECE);
    for (SquareIndex square = Square::SQUARE_A1; square <= Square::SQUARE_H8; square++) {
        board[square] = empty;
    }
    memset(&bitboards, 0, sizeof(bitboards));
    occupation[Color::WHITE] = 0ULL;
    occupation[Color::BLACK] = 0ULL;
    occupation[Color::NO_COLOR] = 0ULL;
    plies = 0;
    plies_since_capture = 0;
    to_move = Color::WHITE;
    opponent = Color::BLACK;
    en_passant = Square::SQUARE_NONE;
}

std::string Position::stringify_board() {
    std::string ret;
    ret.append("  A B C D E F G H\n");
    for (int8_t row = Rank::RANK_TOP; row >= Rank::RANK_1; row--) {
        ret.push_back((char)('1' + row));
        for (int8_t col = File::FILE_A; col <= File::FILE_TOP; col++) {
            ret.push_back(' ');
            SquareIndex square = row * Square::SQUARE_A2 + col;
            char piece = pieceToChar(board[square].piece);
            if (board[square].color == Color::WHITE && piece != ' ') {
                piece += 'A' - 'a';
            }
            ret.push_back(piece);
        }
        ret.push_back('\n');
    }
    return ret;
}

void Position::print_board() {
    std::cout << stringify_board() << std::endl;
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
                eval += (std::max<Evaluation>(
                            0, bitboardGetHW(bitboards[Color::WHITE][Piece::PAWN] & file) - 1)) *
                        EvaluationLiterals::MULTIPLE_PAWN_VAL;
                eval -= (std::max<Evaluation>(
                            0, bitboardGetHW(bitboards[Color::BLACK][Piece::PAWN] & file) - 1)) *
                        EvaluationLiterals::MULTIPLE_PAWN_VAL;
            }
            eval += bitboardGetHW(bitboards[Color::WHITE][Piece::PAWN] & center) *
                    EvaluationLiterals::CENTER_PAWN_VAL;
            eval -= bitboardGetHW(bitboards[Color::BLACK][Piece::PAWN] & center) *
                    EvaluationLiterals::CENTER_PAWN_VAL;
        }
        if (piece == Piece::BISHOP) {
            eval += std::max<int>(0, bitboardGetHW(bitboards[Color::WHITE][Piece::BISHOP]) - 1) *
                    EvaluationLiterals::MULTIPLE_BISHOP_VAL;
            eval -= std::max<int>(0, bitboardGetHW(bitboards[Color::BLACK][Piece::BISHOP]) - 1) *
                    EvaluationLiterals::MULTIPLE_BISHOP_VAL;
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
    for (uint8_t i = 0; i < moves.count; i++) {
        Position newPos = position;
        newPos.makeMove(moves.get(i));
        Evaluation current = -negaMax(newPos, depth - 1);
        if (current > max) {
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
    position.generateMoves(moves);
    for (uint8_t i = 0; i < moves.count; i++) {
        Position newPos = position;
        Move move = moves.get(i);
        newPos.makeMove(move);
        Evaluation current = -negaMax(newPos, depth - 1);
        std::cout << current << std::endl;
        if (current > max) {
            max = current;
            bestMove = move;
        }
    }
    return bestMove;
}

std::array<std::uint64_t, SEARCH_DEPTH_N> positionsEvaluated;
Evaluation alphaBeta(Position *position, Evaluation alpha, Evaluation beta, uint16_t depthleft) {
    // This search is based on the fact that if we can choose between multiple moves and one of
    // those moves yields in a better position than what our opponent can achieve we can ignore this
    // whole subtree
    // Add algorithm for repetition
    positionsEvaluated.at(depthleft + QUIESCE_DEPTH_N) += 1;
    if (depthleft == 0) {
        return quiesce(position, alpha, beta, QUIESCE_DEPTH_N);
        // return position->evaluate();
    }
    MoveList moves{};
    position->generateMoves(moves);

    Evaluation score = 0;
    uint8_t legalMoves = 0;

    for (uint8_t i = 0; i < moves.count; i++) {
        Position newPos = *position;
        newPos.makeMove(moves.get(i));
        if (newPos.inCheck(position->to_move)) {
            // we left ourselves in check
            continue;
        }
        legalMoves += 1;
        score = -alphaBeta(&newPos, -beta, -alpha, depthleft - 1);
        // opponent has a better move in the search tree already so return their
        // limit as ours
        if (score >= beta) {
            return beta; //  fail hard beta-cutoff
        }
        // if the move found is better than the best score we can achieve update
        if (score > alpha) {
            alpha = score;
        }
    }
    if (legalMoves == 0) {
        return EvaluationLiterals::MATE;
    }
    return alpha;
}

Evaluation quiesce(Position *position, Evaluation alpha, Evaluation beta, uint8_t depth) {
    positionsEvaluated.at(depth) += 1;
    Evaluation eval = position->evaluate();
    bool inCheck = position->inCheck(position->to_move);
    if (depth == 0) {
        return eval;
    }
    if (eval >= beta) {
        return beta;
    }
    if (alpha < eval) {
        alpha = eval;
    }
    MoveList moves;
    Evaluation score = EvaluationLiterals::EVEN;
    position->generateMoves(moves);
    uint8_t legalMoves = 0;
    for (uint8_t index = 0; index < moves.count; index++) {
        Move move = moves.get(index);
        if (inCheck || moveGetFlags(move) == MoveFlags::CAPTURE) {
            Position capturePos = *position;
            capturePos.makeMove(move);
            if (capturePos.inCheck(capturePos.opponent)) {
                continue;
            }
            score = -quiesce(&capturePos, -beta, -alpha, depth - 1);
            legalMoves += 1;
            if (score >= beta) {
                return beta;
            }
            if (score > alpha) {
                alpha = score;
            }
        }
    }
    if (inCheck && legalMoves == 0) {
        return EvaluationLiterals::MATE;
    }
    return alpha;
}

SearchInfo search(Position *position, uint16_t depth) {
    auto start = std::chrono::high_resolution_clock::now();
    MoveList moves;
    position->generateMoves(moves);
    Move bestMove = no_move;
    Evaluation alpha = EvaluationLiterals::NEG_INF;
    Evaluation beta = EvaluationLiterals::POS_INF;
    for (uint8_t index = 0; index < moves.count; index++) {
        Position newPos = *position;
        Move move = moves.get(index);
        newPos.makeMove(move);
        Evaluation current = -alphaBeta(&newPos, -beta, -alpha, depth);
        if (current > alpha) {
            alpha = current;
            bestMove = move;
        }
    }
    uint64_t positions = 0;
    for (int i = 0; i < 40; i++) {
        positions += positionsEvaluated[i];
        std::cout << "Depth" << std::to_string(i) << " " << std::to_string(positionsEvaluated[i])
                  << std::endl;
    }
    double rate = (double)positions /
                  std::chrono::duration_cast<std::chrono::nanoseconds>(
                      std::chrono::high_resolution_clock::now() - start)
                      .count() *
                  1000;
    std::cout << "Total of:\n"
              << std::to_string(positions) << "\nat:\n"
              << std::to_string(rate) << "MP/s" << std::endl;
    std::cout << "Eval" << std::to_string(alpha) << std::endl;
    position->print_board();
    return {bestMove, alpha};
}

Bitboard Position::hash() {
    Bitboard result = emptyBitboard;
    for (SquareIndex square = Square::SQUARE_A1; square < Square::SQUARE_H8; square++) {
        SquareInfo squareInfo = board[square];
        result ^= zobristKeys[squareInfo.color][squareInfo.piece][square];
    }
    result ^= zobristSide;
    if (en_passant != Square::SQUARE_NONE) {
        uint8_t file = en_passant % 8;
        result ^= zobristEnPassant[file];
    }
    result ^= zobristCastle[Color::WHITE][Castle::KINGSIDE]
                           [castle_rights[Color::WHITE][Castle::KINGSIDE]];
    result ^= zobristCastle[Color::WHITE][Castle::QUEENSIDE]
                           [castle_rights[Color::WHITE][Castle::QUEENSIDE]];
    result ^= zobristCastle[Color::BLACK][Castle::KINGSIDE]
                           [castle_rights[Color::BLACK][Castle::KINGSIDE]];
    result ^= zobristCastle[Color::BLACK][Castle::QUEENSIDE]
                           [castle_rights[Color::BLACK][Castle::QUEENSIDE]];
    return result;
}