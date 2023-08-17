#pragma once
#include "core.hpp"
#include <bit>
#include <cstdint>
#include <string.h>

constexpr Bitboard fileA = 0x0101010101010101;
constexpr Bitboard fileB = fileA << 1;
constexpr Bitboard fileC = fileB << 1;
constexpr Bitboard fileD = fileC << 1;
constexpr Bitboard fileE = fileD << 1;
constexpr Bitboard fileF = fileE << 1;
constexpr Bitboard fileG = fileF << 1;
constexpr Bitboard fileH = fileG << 1;

constexpr Bitboard notFileA = ~fileA;
constexpr Bitboard notFileB = ~fileB;
constexpr Bitboard notFileC = ~fileC;
constexpr Bitboard notFileD = ~fileD;
constexpr Bitboard notFileE = ~fileE;
constexpr Bitboard notFileF = ~fileF;
constexpr Bitboard notFileG = ~fileG;
constexpr Bitboard notFileH = ~fileH;

constexpr Bitboard rank1 = 0xFF;
constexpr Bitboard rank2 = rank1 * 8;
constexpr Bitboard rank3 = rank2 * 8;
constexpr Bitboard rank4 = rank3 * 8;
constexpr Bitboard rank5 = rank4 * 8;
constexpr Bitboard rank6 = rank5 * 8;
constexpr Bitboard rank7 = rank6 * 8;
constexpr Bitboard rank8 = rank7 * 8;

constexpr Bitboard notRank1 = ~rank1;
constexpr Bitboard notRank2 = ~rank2;
constexpr Bitboard notRank3 = ~rank3;
constexpr Bitboard notRank4 = ~rank4;
constexpr Bitboard notRank5 = ~rank5;
constexpr Bitboard notRank6 = ~rank6;
constexpr Bitboard notRank7 = ~rank7;
constexpr Bitboard notRank8 = ~rank8;

Bitboard bitboardSetSquare(SquareIndex index);
Bitboard getKnightAttacks(Bitboard board);
void printBitboard(Bitboard board);

inline Bitboard bitboardSetSquare(SquareIndex index) { return 1ULL << index; }

inline Bitboard bitboardUnsetSquare(Bitboard board, SquareIndex index) {
  return board & ~bitboardSetSquare(index);
}

inline void bitboardUnsetSquare(Bitboard *board, SquareIndex index) {
  *board &= ~bitboardSetSquare(index);
}

inline SquareIndex get_ls1b_index(Bitboard bitboard) {
  return ffsll(bitboard) - 1;
}

inline uint8_t bitboardGetHW(Bitboard bitboard) {
  return std::popcount(bitboard);
}

class MagicBitboards {
public:
  Bitboard pawnAttacks[Color::NO_COLOR + 1][Square::SQUARE_COUNT];
  Bitboard knightAttacks[Square::SQUARE_COUNT];
  MagicBitboards();
};
