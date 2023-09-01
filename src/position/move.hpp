#pragma once
#include "bitboard.hpp"
#include "magic.hpp"
#include "square.hpp"
#include <array>
#include <cstdint>

// typedefs
typedef std::uint16_t Move;

// constants
constexpr uint64_t MOVE_LIMIT_N = 256;
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
    return (m >> 6) & Square::SQUARE_H8;
}
inline SquareIndex moveGetTo(Move m) {
    return m & Square::SQUARE_H8;
}

// FIXME
inline MoveFlags moveGetFlags(Move m) {
    return static_cast<MoveFlags>(MoveFlags::MASK & (m >> 12));
}

inline Move encodeMove(SquareIndex from, SquareIndex to, uint8_t flags) {
    return to | from << 6 | flags << 12;
}

class MoveList {
  public:
    uint8_t count = 0;
    std::array<Move, MOVE_LIMIT_N> moves;
    MoveList() = default;
    inline Move get(uint8_t index) {
        return moves[index];
    }
    inline void push_back(Move move) {
        moves[count] = move;
        count++;
    }
    inline void addMoves(const SquareIndex &from, Bitboard board, MoveFlags flags) {
        while (board) {
            SquareIndex to = get_ls1b_index(board);
            push_back(encodeMove(from, to, flags));
            board &= unmaskedSquare[to];
        }
    }
    inline std::string stringify() {
        std::string result;
        for (uint8_t i = 0; i < count; i++) {
            std::string fromSquare = squareIndexStringify(moveGetFrom(moves[i]));
            std::string toSquare = squareIndexStringify(moveGetTo(moves[i]));
            result.append(std::to_string(i) + " " + fromSquare + " " + toSquare + "\n");
        }
        return result;
    }
};