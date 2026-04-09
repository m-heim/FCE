#pragma once
#include "core.hpp"
#include "magic.hpp"
#include <array>
#include <bitboard.hpp>
#include <cstdint>
#include <square.hpp>

constexpr uint64_t MOVE_LIMIT_N = 256; // how many moves are stored in the MoveList
constexpr uint16_t no_move = 65;

enum MoveFlags : std::uint8_t {
    QUIET,
    CAPTURE,
    DOUBLE_PUSH,
    EN_PASSANT,
    CASTLE_QUEENSIDE,
    CASTLE_KINGSIDE,
    PROMOTE_QUEEN = 8,
    PROMOTE_ROOK = 9,
    PROMOTE_BISHOP = 10,
    PROMOTE_KNIGHT = 11,
    MOVEFLAGS_BOTTOM = QUIET,
    MOVEFLAGS_TOP = PROMOTE_KNIGHT,
    MOVEFLAGS_COUNT_VALID = MOVEFLAGS_TOP - MOVEFLAGS_BOTTOM + 1,
    NULL_MOVE = 12,
    MASK = 0x0F
};

// typedefs
class Move {
  private:
    std::uint16_t move;

  public:
    Move() {
    }
    Move(uint16_t moveVal) : move(moveVal) {
    }
    Move(SquareIndex from, SquareIndex to, MoveFlags flags) {
        move = (from << 6) | (flags << 12) | to;
    }
    operator int() const {
        return move;
    }
    SquareIndex getFrom() {
        return (move >> 6) & Square::SQUARE_H8;
    }
    SquareIndex getTo() {
        return move & Square::SQUARE_H8;
    }

    MoveFlags getFlags() {
        return static_cast<MoveFlags>(MoveFlags::MASK & (move >> 12));
    }
    std::string stringify() {
        std::string fromSquare = getFrom().stringify();
        std::string toSquare = getTo().stringify();
        MoveFlags flag = getFlags();
        std::string flagString;
        switch (flag) {
        case MoveFlags::QUIET:
            flagString = "QUIET";
            break;
        case MoveFlags::CAPTURE:
            flagString = "CAPTURE";
            break;
        case MoveFlags::DOUBLE_PUSH:
            flagString = "DOUBLE_PUSH";
            break;
        case MoveFlags::EN_PASSANT:
            flagString = "EN_PASSANT";
            break;
        case MoveFlags::CASTLE_QUEENSIDE:
            flagString = "CASTLE_QUEENSIDE";
            break;
        case MoveFlags::CASTLE_KINGSIDE:
            flagString = "CASTLE_KINGSIDE";
            break;
        case MoveFlags::PROMOTE_QUEEN:
            flagString = "PROMOTE_QUEEN";
            break;
        case MoveFlags::PROMOTE_ROOK:
            flagString = "PROMOTE_ROOK";
            break;
        case MoveFlags::PROMOTE_BISHOP:
            flagString = "PROMOTE_BISHOP";
            break;
        case MoveFlags::PROMOTE_KNIGHT:
            flagString = "PROMOTE_KNIGHT";
            break;
        default:
            flagString = "?";
            break;
        }
        return fromSquare + " " + toSquare + " " + flagString;
    }
};

class MoveList {
  public:
    uint8_t count;
    std::array<Move, MOVE_LIMIT_N> moves{};
    MoveList() : count(0){};
    const Move &get(uint8_t index) {
        return moves[index];
    }
    const Move &operator[](uint8_t index) {
        return moves[index];
    }
    void swap(uint8_t index1, uint8_t index2) {
        std::swap(moves[index1], moves[index2]);
    }
    void push_back(Move move) {
        moves[count] = move;
        count++;
    }
    void addMoves(SquareIndex from, Bitboard board, MoveFlags flags) {
        while (board) {
            SquareIndex to = get_ls1b_index(board);
            push_back(Move(from, to, flags));
            board &= unmaskedSquare[to];
        }
    }
    std::string stringify();
};

class UndoInfo {
  public:
    Bitboard hash;
    Move move;
    SquareIndex en_passant;
    bool castle_rights[2][2];
    uint8_t plies_since_capture;
    Piece previousPiece; // if the move was a capture, this is the piece that was captured,
                         // otherwise it is NO_PIECE
};