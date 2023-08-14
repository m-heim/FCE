#pragma once
#include "core.hpp"
#include <cstdint>

typedef uint64_t Bitboard;

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

extern Bitboard pawnAttacks[Color::COLOR_COUNT][Square::SQUARE_COUNT];
extern Bitboard knightAttacks[Square::SQUARE_COUNT];
