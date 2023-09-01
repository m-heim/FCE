#pragma once
#include "bitboard.hpp"
#include "magic.hpp"
#include "square.hpp"
#include <array>
#include <cstdint>

typedef std::uint16_t Move;

#define MOVE_LIMIT_N 256
constexpr Move no_move = 65;

enum MoveFlags : std::uint8_t {
    QUIET,
    CAPTURE,
    EN_PASSANT,
    CASTLE_QUEENSIDE,
    CASTLE_KINGSIDE,
    PROMOTE_QUEEN,
    PROMOTE_ROOK,
    PROMOTE_BISHOP,
    PROMOTE_KNIGHT,
    MASK = 0x0F
};

inline SquareIndex moveGetFrom(Move m) {
    return (m >> 6) & 0x3f;
}
inline SquareIndex moveGetTo(Move m) {
    return m & 0x3f;
}

// FIXME
inline MoveFlags moveGetFlags(Move m) {
    return static_cast<MoveFlags>((m >> 12) & MoveFlags::MASK);
}

inline Move encodeMove(SquareIndex from, SquareIndex to, uint8_t flags) {
    return to | from << 6 | flags << 12;
}

class MoveList {
  public:
    uint8_t count = 0;
    std::array<Move, MOVE_LIMIT_N> moves;
    inline Move get(uint8_t index) {
        return moves[index];
    }
    inline void push_back(Move move) {
        moves[count] = move;
        count++;
    }
    inline void addMoves(const SquareIndex &from, Bitboard &board, MoveFlags flags) {
        while (board) {
            SquareIndex to = get_ls1b_index(board);
            push_back(encodeMove(from, to, flags));
            board &= unmaskedSquare[to];
        }
    }
};