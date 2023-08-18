#include <iostream>
#include "bitboard.hpp"
#include "core.hpp"
Bitboard attacks[105000];

class Magic {
    public:
    std::array<Bitboard, 2000> attacks;
    Bitboard magic;
    Bitboard mask;
    uint8_t shift;
    SquareIndex index;
};
// CODE FROM Tord Romstad
Bitboard random_uint64() {
  Bitboard u1, u2, u3, u4;
  u1 = (Bitboard)(random()) & 0xFFFF; u2 = (Bitboard)(random()) & 0xFFFF;
  u3 = (Bitboard)(random()) & 0xFFFF; u4 = (Bitboard)(random()) & 0xFFFF;
  return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}
Bitboard random_uint64_fewbits() {
  return random_uint64() & random_uint64() & random_uint64();
}
//

int main() {
    Magic magic;
    SquareIndex square = Square::SQUARE_C2;
    magic.mask = getBishopMask(square);
    magic.shift = bitboardGetHW(magic.mask);
    printBitboard(magic.mask);
    initRayAttacks();
    for (SquareIndex square = Square::SQUARE_A1; square <= Square::SQUARE_H8; square++) {
        printSquare(square);
        printBitboard(rays[RayDirection::NORTH_EAST_RAY][square]);
    }
}