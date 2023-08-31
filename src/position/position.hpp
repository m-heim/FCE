#pragma once
#include "bitboard.hpp"
#include "core.hpp"
#include "magic.hpp"
#include <array>
#include <cstdint>
#include <iostream>
#include <vector>
#define QUIESCE_DEPTH_N 10
#define SEARCH_DEPTH_N 1000
class Position {
  public:
    // BOARD REPRESENTATIONS
    std::array<SquareInfo, Square::SQUARE_COUNT> board;
    std::array<std::array<Bitboard, Piece::PIECE_TOP + 1>, Color::COLOR_TOP + 1> bitboards;
    std::array<Bitboard, Color::COLOR_TOP + 1> occupation;
    Color to_move;
    Color opponent;
    SquareIndex en_passant;
    uint16_t plies;
    uint16_t plies_since_capture;
    std::array<std::array<bool, Castle::CASTLE_TOP + 1>, Color::BLACK + 1> castle_rights;

    Position();
    void setSquare(SquareIndex square, Color color, Piece piece);
    std::string stringify_board();
    void print_board();
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
    bool inCheck(Color side);
};

Evaluation negaMax(Position position, uint16_t depth);
Move negaMaxRoot(Position position, uint16_t depth);
Evaluation alphaBeta(Position *position, Evaluation alpha, Evaluation beta, uint16_t depthleft);
SearchInfo search(Position *position, uint16_t depth);
Evaluation quiesce(Position *position, Evaluation alpha, Evaluation beta, uint8_t depth);

inline Evaluation Position::evaluatePosition(void) {
    Evaluation eval = 0;
    return eval;
}

inline bool Position::makeMove(Move m) {
    SquareIndex from = moveGetFrom(m);
    SquareIndex to = moveGetTo(m);
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
    opponent = to_move;
    to_move = (to_move == Color::WHITE ? Color::BLACK : Color::WHITE);

    return true;
}

inline void Position::generatePieceMoves(MoveList &moves) {
    Bitboard ours = occupation[to_move];
    Bitboard theirs = occupation[opponent];
    Bitboard oursOrTheirs = ours | theirs;
    Bitboard neitherOursAndTheirs = ~oursOrTheirs;
    Bitboard knights = bitboards[to_move][Piece::KNIGHT];
    Bitboard bishop = bitboards[to_move][Piece::BISHOP];
    Bitboard rook = bitboards[to_move][Piece::ROOK];
    Bitboard queen = bitboards[to_move][Piece::QUEEN];
    Bitboard king = bitboards[to_move][Piece::KING];
    while (bishop) {
        SquareIndex from = get_ls1b_index(bishop);
        Bitboard result = bishopMagics[from].getAttack(bishopMasks[from] & oursOrTheirs);
        Bitboard attacks = result & theirs;
        Bitboard quiet = result & neitherOursAndTheirs;
        moves.addMoves(from, attacks, MoveFlags::CAPTURE);
        moves.addMoves(from, quiet, MoveFlags::QUIET);
        bishop &= unmaskedSquare[from];
    }
    while (knights) {
        SquareIndex from = get_ls1b_index(knights);
        Bitboard attacks = knightAttacks[from] & theirs;
        Bitboard non_attacks = knightAttacks[from] & neitherOursAndTheirs;
        moves.addMoves(from, attacks, MoveFlags::CAPTURE);
        moves.addMoves(from, non_attacks, MoveFlags::QUIET);
        knights &= unmaskedSquare[from];
    }
    while (rook) {
        SquareIndex from = get_ls1b_index(rook);
        Bitboard result = rookMagics[from].getAttack(rookMasks[from] & oursOrTheirs);
        Bitboard attacks = result & theirs;
        Bitboard quiet = result & neitherOursAndTheirs;
        moves.addMoves(from, attacks, MoveFlags::CAPTURE);
        moves.addMoves(from, quiet, MoveFlags::QUIET);
        rook &= unmaskedSquare[from];
    }
    while (queen) {
        SquareIndex from = get_ls1b_index(queen);
        Bitboard result = rookMagics[from].getAttack(rookMasks[from] & oursOrTheirs) |
                          bishopMagics[from].getAttack(bishopMasks[from] & oursOrTheirs);
        Bitboard attacks = result & theirs;
        Bitboard quiet = result & neitherOursAndTheirs;
        moves.addMoves(from, attacks, MoveFlags::CAPTURE);
        moves.addMoves(from, quiet, MoveFlags::QUIET);
        queen &= unmaskedSquare[from];
    }
    if (king) {
        SquareIndex from = get_ls1b_index(king);
        Bitboard attacks = kingAttacks[from] & theirs;
        Bitboard non_attacks = kingAttacks[from] & neitherOursAndTheirs;
        moves.addMoves(from, attacks, MoveFlags::CAPTURE);
        moves.addMoves(from, non_attacks, MoveFlags::QUIET);
    }
    Bitboard pawns = bitboards[to_move][Piece::PAWN];
    Bitboard pawns_promoting;
    Bitboard pawns_not_promoting;
    Bitboard doublePushRank;
    if (to_move == Color::WHITE) {
        pawns_not_promoting = pawns & notRank7;
        pawns_promoting = pawns & rank7;
        doublePushRank = rank4;
    } else {
        pawns_not_promoting = pawns & notRank2;
        pawns_promoting = pawns & rank2;
        doublePushRank = rank5;
    }
    while (pawns_not_promoting) {
        SquareIndex from = get_ls1b_index(pawns_not_promoting);
        Bitboard attacks = occupation[opponent] & pawnAttacks[to_move][from];
        moves.addMoves(from, attacks, MoveFlags::CAPTURE);
        Bitboard pushes = pawnPushes[to_move][from] & neitherOursAndTheirs;
        if (pushes) {
            SquareIndex to = get_ls1b_index(pushes);
            moves.push_back(serialize_move(from, to, MoveFlags::QUIET));

            Bitboard doublePush = pawnPushes[to_move][to];
            if (doublePush & neitherOursAndTheirs & doublePushRank) {
                moves.push_back(serialize_move(from, get_ls1b_index(doublePush), MoveFlags::QUIET));
            }
        }
        pawns_not_promoting &= unmaskedSquare[from];
    }
    while (pawns_promoting) {
        SquareIndex from = get_ls1b_index(pawns_promoting);
        Bitboard attacks = occupation[opponent] & pawnAttacks[to_move][from];
        Bitboard pushes = neitherOursAndTheirs & pawnPushes[to_move][from];
        moves.addMoves(from, attacks, MoveFlags::CAPTURE);
        moves.addMoves(from, pushes, MoveFlags::QUIET);
        pawns_promoting &= unmaskedSquare[from];
    }
}

inline bool Position::inCheck(Color side) {
    Color opponentSide = (side == Color::WHITE) ? Color::BLACK : Color::WHITE;
    SquareIndex square = get_ls1b_index(bitboards[side][Piece::KING]);
    Bitboard oursAndTheirs = occupation[Color::WHITE] | occupation[Color::BLACK];
    auto opponentPieces = bitboards[opponentSide];
    Bitboard knight = knightAttacks[square] & opponentPieces[Piece::KNIGHT];
    Bitboard rook = rookMagics.at(square).getAttack(getRookMask(square) & oursAndTheirs) &
                    (opponentPieces[Piece::QUEEN] | opponentPieces[Piece::ROOK]);
    Bitboard bishop = bishopMagics.at(square).getAttack(getBishopMask(square) & oursAndTheirs) &
                      (opponentPieces[Piece::QUEEN] | opponentPieces[Piece::BISHOP]);
    Bitboard pawn = pawnAttacks[side][square] & opponentPieces[Piece::PAWN];
    return knight | rook | bishop | pawn;
}

inline void Position::generatePawnMoves(MoveList &moves) {
    Bitboard pawns = bitboards[to_move][Piece::PAWN];
    Bitboard pawns_not_promoting;
    Bitboard pawns_promoting;
    Bitboard doublePushRank;
    if (to_move == Color::WHITE) {
        pawns_not_promoting = pawns & notRank7;
        pawns_promoting = pawns & rank7;
        doublePushRank = rank4;
    } else {
        pawns_not_promoting = pawns & notRank2;
        pawns_promoting = pawns & rank2;
        doublePushRank = rank5;
    }
    while (pawns_not_promoting) {
        SquareIndex from = get_ls1b_index(pawns_not_promoting);
        Bitboard attacks = occupation[opponent] & pawnAttacks[to_move][from];
        moves.addMoves(from, attacks, MoveFlags::CAPTURE);
        Bitboard empty = ~(occupation[opponent] | occupation[to_move]);
        Bitboard pushes = pawnPushes[to_move][from] & empty;
        while (pushes) {
            SquareIndex to = get_ls1b_index(pushes);
            Bitboard doublePush = pawnPushes[to_move][to];
            if (doublePush & empty & doublePushRank) {
                moves.push_back(serialize_move(from, get_ls1b_index(doublePush), MoveFlags::QUIET));
            }
            moves.push_back(serialize_move(from, to, MoveFlags::QUIET));
            pushes &= unmaskedSquare[to];
        }
        pawns_not_promoting &= unmaskedSquare[from];
    }
    while (pawns_promoting) {
        SquareIndex from = get_ls1b_index(pawns_promoting);
        Bitboard attacks = occupation[opponent] & pawnAttacks[to_move][from];
        moves.addMoves(from, attacks, MoveFlags::CAPTURE);
        pawns_promoting &= unmaskedSquare[from];
    }
}

inline void Position::generateMoves(MoveList &moves) {
    generatePieceMoves(moves);
    // generatePawnMoves(moves);
}