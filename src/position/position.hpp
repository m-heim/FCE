#pragma once
#include "bitboard.hpp"
#include "core.hpp"
#include "magic.hpp"
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
    void generatePawnMoves(MoveList &moves);
    void generatePieceMoves(MoveList &moves);
    void generateMoves(MoveList &moves);
    bool makeMove(Position &position, Move m);
};

Evaluation negaMax(Position position, uint16_t depth);
Move negaMaxRoot(Position position, uint16_t depth);
Evaluation alphaBeta(Position *position, Evaluation alpha, Evaluation beta,
                     uint16_t depthleft);
SearchInfo search(Position *position, uint16_t depth);

inline Evaluation Position::evaluatePosition(void) {
    Evaluation eval = 0;
    return eval;
}

inline bool Position::makeMove(Move m) {
    SquareIndex from = moveGetFrom(m);
    SquareIndex to = moveGetTo(m);
    Color opponent = (to_move == Color::WHITE ? Color::BLACK : Color::WHITE);
    // if we take the opponents king
    if (bitboardSetSquare(to) == bitboards[opponent][Piece::KING]) {
        return false;
    }
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
    to_move = (to_move == Color::WHITE ? Color::BLACK : Color::WHITE);

    return true;
}

inline void Position::generatePieceMoves(MoveList &moves) {
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
        knights &= unmaskedSquare[from];
    }
    Bitboard king = bitboards[to_move][Piece::KING];
    if (king) {
        SquareIndex from = get_ls1b_index(king);
        Bitboard attacks = kingAttacks[from] & theirs;
        Bitboard non_attacks = kingAttacks[from] & neitherOursAndTheirs;
        moves.addMoves(from, attacks, MoveFlags::CAPTURE);
        moves.addMoves(from, non_attacks, MoveFlags::QUIET);
    }
    Bitboard bishop = bitboards[to_move][Piece::BISHOP];
    while (bishop) {
        SquareIndex from = get_ls1b_index(bishop);
        Bitboard result =
            bishopMagics[from].getAttack(bishopMasks[from] & oursOrTheirs);
        Bitboard attacks = result & theirs;
        Bitboard quiet = result & neitherOursAndTheirs;
        moves.addMoves(from, attacks, MoveFlags::CAPTURE);
        moves.addMoves(from, quiet, MoveFlags::QUIET);
        bishop &= unmaskedSquare[from];
    }
    Bitboard rook = bitboards[to_move][Piece::ROOK];
    while (rook) {
        SquareIndex from = get_ls1b_index(rook);
        Bitboard result =
            rookMagics[from].getAttack(rookMasks[from] & oursOrTheirs);
        Bitboard attacks = result & theirs;
        Bitboard quiet = result & neitherOursAndTheirs;
        moves.addMoves(from, attacks, MoveFlags::CAPTURE);
        moves.addMoves(from, quiet, MoveFlags::QUIET);
        rook &= unmaskedSquare[from];
    }
    Bitboard queen = bitboards[to_move][Piece::QUEEN];
    while (queen) {
        SquareIndex from = get_ls1b_index(queen);
        Bitboard result =
            rookMagics[from].getAttack(rookMasks[from] & oursOrTheirs) |
            bishopMagics[from].getAttack(bishopMasks[from] & oursOrTheirs);
        Bitboard attacks = result & theirs;
        Bitboard quiet = result & neitherOursAndTheirs;
        moves.addMoves(from, attacks, MoveFlags::CAPTURE);
        moves.addMoves(from, quiet, MoveFlags::QUIET);
        queen &= unmaskedSquare[from];
    }
}

inline void Position::generatePawnMoves(MoveList &moves) {
    Bitboard pawns = bitboards[to_move][Piece::PAWN];
    Bitboard pawns_not_on_7;
    if (to_move == Color::WHITE) {
        pawns_not_on_7 = pawns & notRank7;
    } else {
        pawns_not_on_7 = pawns & notRank2;
    }
    Color opponent = (to_move == Color::BLACK) ? Color::WHITE : Color::BLACK;
    while (pawns_not_on_7) {
        SquareIndex from = get_ls1b_index(pawns_not_on_7);
        Bitboard attacks = occupation[opponent] & pawnAttacks[to_move][from];
        moves.addMoves(from, attacks, MoveFlags::CAPTURE);
        Bitboard empty = ~(occupation[opponent] | occupation[to_move]);
        Bitboard pushes = pawnPushes[to_move][from] & empty;
        while (pushes) {
            SquareIndex to = get_ls1b_index(pushes);
            Bitboard doublePush = pawnPushes[to_move][to];
            if (doublePush & empty & rank4) {
                moves.push_back(serialize_move(from, get_ls1b_index(doublePush),
                                               MoveFlags::QUIET));
            }
            moves.push_back(serialize_move(from, to, MoveFlags::QUIET));
            pushes &= unmaskedSquare[to];
        }
        pawns_not_on_7 &= unmaskedSquare[from];
    }
}

inline void Position::generateMoves(MoveList &moves) {
    generatePawnMoves(moves);
    generatePieceMoves(moves);
}