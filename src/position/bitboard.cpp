#include "bitboard.hpp"
#include "core.hpp"

Bitboard bitboardSetSquare(SquareIndex index) {
    return 1ULL << index;
}

MagicBitboards::MagicBitboards() {
    for (SquareIndex index = SQUARE_A1; index < SQUARE_H8; index++) {
        Bitboard board = bitboardSetSquare(index);
        Bitboard attacks = ((board & notFileA) << Direction::NORTH_WEST || ((board & notFileH) << Direction::NORTH_EAST));
        pawnAttacks[Color::WHITE][index] = attacks;
        knightAttacks[index] = board;
    }
}