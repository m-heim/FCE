#pragma once
#include "square.hpp"
#include <array>
#include <bit>
#include <cstdint>
#include <random>
#include <strings.h>

// typedefs
typedef std::uint64_t Bitboard;

// constants
constexpr uint64_t BITBOARD_SUBSETS_N = 4096;

constexpr Bitboard emptyBitboard = 0x0000000000000000;
constexpr Bitboard fullBitboard = ~emptyBitboard;

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
constexpr Bitboard rank2 = rank1 << 8;
constexpr Bitboard rank3 = rank2 << 8;
constexpr Bitboard rank4 = rank3 << 8;
constexpr Bitboard rank5 = rank4 << 8;
constexpr Bitboard rank6 = rank5 << 8;
constexpr Bitboard rank7 = rank6 << 8;
constexpr Bitboard rank8 = rank7 << 8;

constexpr Bitboard notRank1 = ~rank1;
constexpr Bitboard notRank2 = ~rank2;
constexpr Bitboard notRank3 = ~rank3;
constexpr Bitboard notRank4 = ~rank4;
constexpr Bitboard notRank5 = ~rank5;
constexpr Bitboard notRank6 = ~rank6;
constexpr Bitboard notRank7 = ~rank7;
constexpr Bitboard notRank8 = ~rank8;

constexpr Bitboard diagonal = 0x8040201008040201;
constexpr Bitboard diagonal2 = 0x0102040810204080;

constexpr Bitboard borders = rank1 | rank8 | fileA | fileH;
constexpr Bitboard notBorders = ~borders;

constexpr Bitboard center = (fileD | fileE) & (rank4 | rank5);
constexpr Bitboard notCenter = ~center;
constexpr Bitboard extendedCenter =
    (fileC | fileD | fileE | fileF) & (rank3 | rank4 | rank5 | rank6);

// non performance relevant functions
std::array<Bitboard, BITBOARD_SUBSETS_N> getBitboardSubsets(Bitboard mask);
std::string stringifyBitboard(Bitboard board);
void printBitboard(Bitboard board);

// inline functions
inline Bitboard bitboardSetSquare(SquareIndex index) {
    return 1ULL << index;
}
inline Bitboard bitboardUnsetSquare(Bitboard board, SquareIndex index) {
    return board & ~bitboardSetSquare(index);
}
inline void bitboardUnsetSquare(Bitboard *board, SquareIndex index) {
    *board &= ~bitboardSetSquare(index);
}
inline SquareIndex get_ls1b_index(Bitboard bitboard) {
    // NOTE integer wrap if empty
    return ffsll(bitboard) - 1;
}
inline SquareIndex get_ms1b_index(Bitboard bitboard) {
    // NOTE if bb empty = 63
    return 63 - __builtin_clzll(bitboard);
}
inline uint8_t bitboardGetHW(Bitboard bitboard) {
    return std::popcount<Bitboard>(bitboard);
}

constexpr Bitboard BITBOARD_RANDOM_NUMBER_SEED = fileA | fileC | rank3 | rank7;
extern std::mt19937_64 randomNumberEngine;
extern std::uniform_int_distribution<Bitboard> randomBitboardDistribution;
void reseedBitboardEngine(Bitboard seed);
Bitboard getRandomBitboard();