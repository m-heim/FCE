#pragma once
#include "bitboard.hpp"
#include "chess.hpp"
#include "magic.hpp"
#include "move.hpp"
#include "square.hpp"
#include "zobrist.hpp"
#include <array>
#include <cstdint>
#include <iostream>
#include <vector>

// typedefs
typedef std::pair<Move, Evaluation> SearchInfo;

// constants
constexpr uint64_t SEARCH_DEPTH_N = 1000;
constexpr uint64_t QUIESCE_DEPTH_N = 10;

class Position {
  public:
    // BOARD REPRESENTATIONS
    std::array<SquareInfo, Square::SQUARE_COUNT> board;
    std::array<std::array<Bitboard, Piece::PIECE_TOP + 1>, Color::COLOR_TOP + 1> bitboards;
    std::array<Bitboard, Color::COLOR_TOP + 1> occupation;
    std::array<std::array<bool, Castle::CASTLE_COUNT>, Color::BLACK + 1> castle_rights;
    Color to_move;
    Color opponent;
    SquareIndex en_passant;
    uint16_t plies;
    uint16_t plies_since_capture;
    Bitboard hash;
    Position();
    void setSquare(SquareIndex square, Color color, Piece piece);
    std::string stringify_board();
    void print_board();
    Evaluation evaluate(void);
    Evaluation evaluateMaterial(void);
    Evaluation evaluatePosition(void);
    void makeMove(Move m);
    inline bool kingExists() {
        return bitboards[to_move][Piece::KING] != 0;
    }
    void generatePawnMoves(MoveList &moves);
    void generatePieceMoves(MoveList &moves);
    void generateMoves(MoveList &moves);
    bool inCheck(Color side);
    Bitboard computeHash();
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

inline void Position::makeMove(Move m) {
    SquareIndex from = moveGetFrom(m);
    SquareIndex to = moveGetTo(m);
    uint8_t flags = moveGetFlags(m);
    SquareInfo movingPiece = board[from];
    SquareInfo leavingPiece = board[to];
    if (en_passant != Square::SQUARE_NONE) {
        uint8_t file = en_passant % 8;
        hash ^= zobristEnPassant[file];
        en_passant = Square::SQUARE_NONE;
    }
    // while castling is allowed and the king doesnt move, keep this
    if (movingPiece.piece == Piece::KING) {
        if (castle_rights[to_move][Castle::QUEENSIDE]) {
            castle_rights[to_move][Castle::QUEENSIDE] = false;
            hash ^= zobristCastle[to_move][Castle::QUEENSIDE][true];
        } else if (castle_rights[to_move][Castle::KINGSIDE]) {
            castle_rights[to_move][Castle::KINGSIDE] = false;
            hash ^= zobristCastle[to_move][Castle::KINGSIDE][true];
        }
    } else if (movingPiece.piece == Piece::ROOK) {
        SquareIndex queenside = Square::SQUARE_A1 + ((bool)to_move * (int)Square::SQUARE_A8);
        SquareIndex kingside = Square::SQUARE_H1 + ((bool)to_move * (int)Square::SQUARE_A8);
        if (from == queenside && castle_rights[to_move][Castle::QUEENSIDE]) {
            castle_rights[to_move][Castle::QUEENSIDE] = false;
            hash ^= zobristCastle[to_move][Castle::QUEENSIDE][true];
        } else if (from == kingside && castle_rights[to_move][Castle::KINGSIDE]) {
            castle_rights[to_move][Castle::KINGSIDE] = false;
            hash ^= zobristCastle[to_move][Castle::KINGSIDE][true];
        }
    } else if (leavingPiece.piece == Piece::ROOK) {
        SquareIndex queenside = Square::SQUARE_A1 + ((bool)opponent * (int)Square::SQUARE_A8);
        SquareIndex kingside = Square::SQUARE_H1 + ((bool)opponent * (int)Square::SQUARE_A8);
        if (to == queenside && castle_rights[opponent][Castle::QUEENSIDE]) {
            castle_rights[opponent][Castle::QUEENSIDE] = false;
            hash ^= zobristCastle[opponent][Castle::QUEENSIDE][true];
        } else if (to == kingside && castle_rights[opponent][Castle::KINGSIDE]) {
            castle_rights[opponent][Castle::KINGSIDE] = false;
            hash ^= zobristCastle[opponent][Castle::KINGSIDE][true];
        }
    }
    if (flags == MoveFlags::QUIET) {
        setSquare(to, movingPiece.color, movingPiece.piece);
        setSquare(from, Color::NO_COLOR, Piece::NO_PIECE);
        hash ^= zobristKeys[to_move][movingPiece.piece][from];
        hash ^= zobristKeys[to_move][movingPiece.piece][to];
        plies_since_capture += 1;
    } else if (flags == MoveFlags::DOUBLE_PUSH) {
        setSquare(to, movingPiece.color, movingPiece.piece);
        setSquare(from, Color::NO_COLOR, Piece::NO_PIECE);
        hash ^= zobristKeys[to_move][movingPiece.piece][from];
        hash ^= zobristKeys[to_move][movingPiece.piece][to];
        en_passant = (from + to) / 2;
        hash ^= zobristEnPassant[en_passant % 8];
        plies_since_capture += 1;
    } else if (flags == MoveFlags::CAPTURE) {

        setSquare(to, movingPiece.color, movingPiece.piece);
        setSquare(from, Color::NO_COLOR, Piece::NO_PIECE);
        hash ^= zobristKeys[to_move][movingPiece.piece][from];
        hash ^= zobristKeys[to_move][movingPiece.piece][to];
        hash ^= zobristKeys[opponent][leavingPiece.piece][to];
    } else if (flags == MoveFlags::CASTLE_KINGSIDE) {
        setSquare(to, movingPiece.color, Piece::KING);
        setSquare(from + 1, movingPiece.color, Piece::ROOK);
        setSquare(from, Color::NO_COLOR, Piece::NO_PIECE);
        setSquare(to + 1, Color::NO_COLOR, Piece::NO_PIECE);
        hash ^= zobristKeys[to_move][movingPiece.piece][from];
        hash ^= zobristKeys[to_move][Piece::ROOK][from + 3];
        hash ^= zobristKeys[to_move][Piece::ROOK][to - 1];
        hash ^= zobristKeys[to_move][movingPiece.piece][to];
    } else if (flags == MoveFlags::EN_PASSANT) {
        Offset epOpponent = from;
        if (to > from) {
            // WHITE
            if (to == (from + Direction::NORTH_WEST)) {
                epOpponent -= 1;
            } else {
                epOpponent += 1;
            }
        } else {
            if (to == (from + Direction::SOUTH_WEST)) {
                epOpponent -= 1;
            } else {
                epOpponent += 1;
            }
        }
        setSquare(to, movingPiece.color, movingPiece.piece);
        setSquare(from, Color::NO_COLOR, Piece::NO_PIECE);
        setSquare(epOpponent, Color::NO_COLOR, Piece::NO_PIECE);
        hash ^= zobristKeys[to_move][movingPiece.piece][from];
        hash ^= zobristKeys[to_move][movingPiece.piece][to];
        hash ^= zobristKeys[opponent][leavingPiece.piece][epOpponent];
    }
    hash ^= zobristSide;
    plies += 1;
    opponent = to_move;
    to_move = (to_move == Color::WHITE ? Color::BLACK : Color::WHITE);
    return;
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
        Bitboard theoreticalAttackingMoves = pawnAttacks[to_move][from];
        Bitboard realAttackingMoves = theirs & theoreticalAttackingMoves;
        if (en_passant != Square::SQUARE_NONE) {
            Bitboard epBitboard = maskedSquare[en_passant];
            if (theoreticalAttackingMoves & epBitboard) {
                moves.push_back(encodeMove(from, en_passant, MoveFlags::EN_PASSANT));
            }
        }
        moves.addMoves(from, realAttackingMoves, MoveFlags::CAPTURE);
        Bitboard pushes = pawnPushes[to_move][from] & neitherOursAndTheirs;
        if (pushes) {
            SquareIndex to = get_ls1b_index(pushes);
            moves.push_back(encodeMove(from, to, MoveFlags::QUIET));

            Bitboard doublePush = pawnPushes[to_move][to];
            if (doublePush & neitherOursAndTheirs & doublePushRank) {
                moves.push_back(
                    encodeMove(from, get_ls1b_index(doublePush), MoveFlags::DOUBLE_PUSH));
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
    while (bishop) {
        SquareIndex from = get_ls1b_index(bishop);
        Bitboard result = bishopMagics[from].getAttack(oursOrTheirs);
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
        Bitboard result = rookMagics[from].getAttack(oursOrTheirs);
        Bitboard attacks = result & theirs;
        Bitboard quiet = result & neitherOursAndTheirs;
        moves.addMoves(from, attacks, MoveFlags::CAPTURE);
        moves.addMoves(from, quiet, MoveFlags::QUIET);
        rook &= unmaskedSquare[from];
    }
    while (queen) {
        SquareIndex from = get_ls1b_index(queen);
        Bitboard result =
            rookMagics[from].getAttack(oursOrTheirs) | bishopMagics[from].getAttack(oursOrTheirs);
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
        if (castle_rights[to_move][Castle::KINGSIDE]) {
            Offset moveDir = Direction::EAST;
            Offset moveDirTarget = moveDir + Direction::EAST;
            if (((king << moveDir) & neitherOursAndTheirs) &&
                ((king << moveDirTarget) & neitherOursAndTheirs) && !inCheck(to_move)) {
                Position intermediate = *this;
                intermediate.setSquare(from + moveDir, to_move, Piece::KING);
                intermediate.setSquare(from, Color::NO_COLOR, Piece::NO_PIECE);
                Position intermediate2 = *this;
                intermediate.setSquare(from + moveDirTarget, to_move, Piece::KING);
                intermediate.setSquare(from, Color::NO_COLOR, Piece::NO_PIECE);
                if (!intermediate.inCheck(intermediate.to_move) &&
                    !intermediate2.inCheck(intermediate2.to_move)) {
                    moves.push_back(
                        encodeMove(from, from + moveDirTarget, MoveFlags::CASTLE_KINGSIDE));
                }
            }
        }
    }
}

inline bool Position::inCheck(Color side) {
    Color opponentSide = (side == Color::BLACK ? Color::WHITE : Color::BLACK);
    SquareIndex square = get_ls1b_index(bitboards[side][Piece::KING]);
    Bitboard oursAndTheirs = occupation[Color::WHITE] | occupation[Color::BLACK];
    auto opponentPieces = bitboards[opponentSide];
    Bitboard knight = knightAttacks[square] & opponentPieces[Piece::KNIGHT];
    Bitboard rook = rookMagics.at(square).getAttack(oursAndTheirs) &
                    (opponentPieces[Piece::QUEEN] | opponentPieces[Piece::ROOK]);
    Bitboard bishop = bishopMagics.at(square).getAttack(oursAndTheirs) &
                      (opponentPieces[Piece::QUEEN] | opponentPieces[Piece::BISHOP]);
    Bitboard pawn = pawnAttacks[side][square] & opponentPieces[Piece::PAWN];
    Bitboard king = kingAttacks[square] & opponentPieces[Piece::KING];
    return (knight | rook | bishop | pawn | king) > emptyBitboard;
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
        if (en_passant != Square::SQUARE_NONE) {
            Bitboard ep = maskedSquare[en_passant];
            if (attacks & ep) {
                moves.push_back(encodeMove(from, en_passant, MoveFlags::EN_PASSANT));
            }
        }
        Bitboard empty = ~(occupation[opponent] | occupation[to_move]);
        Bitboard pushes = pawnPushes[to_move][from] & empty;
        while (pushes) {
            SquareIndex to = get_ls1b_index(pushes);
            Bitboard doublePush = pawnPushes[to_move][to];
            if (doublePush & empty & doublePushRank) {
                moves.push_back(encodeMove(from, get_ls1b_index(doublePush), MoveFlags::QUIET));
            }
            moves.push_back(encodeMove(from, to, MoveFlags::QUIET));
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