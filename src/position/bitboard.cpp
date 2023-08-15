#include "bitboard.hpp"
#include "core.hpp"

Bitboard bitboardSetSquare(SquareIndex index) {
    return 1ULL << index;
}

Bitboard getKnightAttacks(Bitboard board) {
        return ((board << Direction::NNE | board >> -Direction::SSE) & notFileA) |
        ((board << Direction::NNW | board >> -Direction::SSW) & notFileH) |
        ((board << Direction::WWN | board >> -Direction::WWS) & notFileH & notFileG) |
        ((board << Direction::EEN | board >> -Direction::EES) & notFileA & notFileB);
}

MagicBitboards::MagicBitboards() {
    for (SquareIndex index = SQUARE_A1; index < SQUARE_H8; index++) {
        Bitboard board = bitboardSetSquare(index);
        Bitboard attacks = ((board & notFileA) << Direction::NORTH_WEST || ((board & notFileH) << Direction::NORTH_EAST));
        pawnAttacks[Color::WHITE][index] = attacks;
        knightAttacks[index] = getKnightAttacks(board);
    }
}