#pragma once
#include "core.hpp"
#include <array>
#include <bit>
#include <cstdint>
#include <strings.h>
#include <vector>

#define MAGICS_ARRAY_SIZE 200000

inline Bitboard getMagicIndex(Bitboard board, Bitboard magic, uint8_t shift) {
  return (board * magic) >> (64 - shift);
}

class Magic {
public:
  Bitboard *attacks;
  Bitboard magic;
  Bitboard mask;
  uint8_t shift;
  inline Bitboard getAttack(Bitboard occupation) {
    return attacks[getMagicIndex(occupation & mask, magic, shift)];
  }
};

extern std::array<Bitboard, MAGICS_ARRAY_SIZE> magics;
extern std::array<Magic, Square::SQUARE_COUNT> rookMagics;
extern std::array<Magic, Square::SQUARE_COUNT> bishopMagics;
extern std::array<std::array<Bitboard, Square::SQUARE_COUNT>,
                  RayDirection::RAY_COUNT>
    rays;
extern std::array<Bitboard, Square::SQUARE_COUNT> rankAttacks;
extern std::array<Bitboard, Square::SQUARE_COUNT> fileAttacks;
extern std::array<std::array<Bitboard, Square::SQUARE_COUNT>, 2> pawnAttacks;
extern std::array<Bitboard, Square::SQUARE_COUNT> knightAttacks;
extern std::array<Bitboard, Square::SQUARE_COUNT> kingAttacks;
extern std::array<std::array<Bitboard, Square::SQUARE_COUNT>, 2> pawnPushes;
extern std::array<std::array<Bitboard, Square::SQUARE_COUNT>, 2>
    pawnDoublePushes;

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

// Line attacks
Bitboard getDiagonalMask(SquareIndex index);
Bitboard getDiagonal2Mask(SquareIndex index);
Bitboard getRankMask(SquareIndex index);
Bitboard getFileMask(SquareIndex index);

// Piece attacks
Bitboard getQueenMask(SquareIndex index);
Bitboard getRookMask(SquareIndex index);
Bitboard getBishopMask(SquareIndex index);

Bitboard getKnightAttacks(Bitboard board);
Bitboard getBishopAttacks(SquareIndex index, Bitboard occupancy);
Bitboard getRookAttacks(SquareIndex index, Bitboard occupancy);

// Get all possible combinations of bits in a mask
std::array<Bitboard, 4096> getBitboardSubsets(Bitboard mask);

void initGlobals();
void initMagics();
void initRayAttacks();
Magic initMagicSquare(SquareIndex index, bool bishop, uint64_t *magicIndex);
Bitboard getRayAttacks(Bitboard occupied, RayDirection direction,
                       SquareIndex square);

void printBitboard(Bitboard board);

// bitboard manipulation
inline Bitboard bitboardSetSquare(SquareIndex index) {
  return 1ULL << index;
}
inline Bitboard bitboardUnsetSquare(Bitboard board, SquareIndex index) {
  return board & ~bitboardSetSquare(index);
}
inline void bitboardUnsetSquare(Bitboard *board, SquareIndex index) {
  *board &= ~bitboardSetSquare(index);
}

// cpu inline
inline SquareIndex get_ls1b_index(Bitboard bitboard) {
  return ffsll(bitboard) - 1;
}
inline SquareIndex get_ms1b_index(Bitboard bitboard) {
  // NOTE returns 63 if no bit is set
  return 63 - __builtin_clzll(bitboard);
}

inline uint8_t bitboardGetHW(Bitboard bitboard) {
  return std::popcount<Bitboard>(bitboard);
}

constexpr std::array<Bitboard, Square::SQUARE_COUNT> bishopMagicNumbers = {
    297457482031202818ULL,   1423718041586254880ULL,  4612834200762974208ULL,
    1527041337516359808ULL,  299344188145698ULL,      9243675691856363650ULL,
    666815356555232256ULL,   10754667412785726720ULL, 396334498041630787ULL,
    576462959952400898ULL,   54087185661561984ULL,    2350888918288957440ULL,
    36029966323810304ULL,    4611686611680247872ULL,  2314851326252550144ULL,
    27022184162001476ULL,    1212176402970312972ULL,  2537690285244928ULL,
    2340396466438160ULL,     4644508947989768ULL,     3604006710175467586ULL,
    17169982186095136ULL,    281552306045440ULL,      2322789185097764ULL,
    5103934052516352ULL,     18304704023897104ULL,    72215923884556416ULL,
    23089744217379072ULL,    20411335544152080ULL,    13853215394600456192ULL,
    4612816385104759813ULL,  517984601852087360ULL,   9516123644548614144ULL,
    10381361538448491008ULL, 1126209211663888ULL,     4611811364900962432ULL,
    594485050712526912ULL,   4611968635903213632ULL,  9944572508438597760ULL,
    11533456970653663568ULL, 144401072373383296ULL,   1200958822440992ULL,
    435161414118672384ULL,   275150537218ULL,         1175448337558569232ULL,
    9008324605445634ULL,     4613940859145061440ULL,  9248142935969497604ULL,
    9241527825826061328ULL,  5262504629027480704ULL,  283602322432ULL,
    52778773004294ULL,       1161937568708198404ULL,  648629467916404736ULL,
    18067488617074688ULL,    9245892818134663168ULL,  9872453925991490048ULL,
    108093061158404608ULL,   577023706619057668ULL,   9223407272800682497ULL,
    9017096201701504ULL,     145245658143004160ULL,   2454753201996955736ULL,
    306253579411275793ULL};

constexpr std::array<Bitboard, Square::SQUARE_COUNT> rookMagicNumbers = {
    180146184118141000ULL,
    4513082931953666ULL,
    3746995448927224064ULL,
    5333396655343403264ULL,
    20266198323232896ULL,
    9529619010539241472ULL,
    4616541462446735410ULL,
    288371254451273732ULL,
    35184640524288ULL,
    137473819012ULL,
    9010499937141632ULL,
    4785350018860192ULL,
    2199027516578ULL,
    10403315968157156354ULL,
    1302785987448832ULL,
    9799841860198335748ULL,
    36033306734631460ULL,
    90073366936947200ULL,
    306248081786011905ULL,
    2018179981061915184ULL,
    41096480505893376ULL,
    78816304093466624ULL,
    579099599537438976ULL,
    144150406807683088ULL,
    4657285005457031168ULL,
    9017112328718336ULL,
    504473939326747648ULL,
    11583260440754422784ULL,
    108651618987147649ULL,
    1459166313628303392ULL,
    2260596318032516ULL,
    72057613644726272ULL,
    2341871806534648064ULL,
    1152921504612090945ULL,
    4505936358016404ULL,
    37160195582853122ULL,
    2252491706599426ULL,
    4755801498569409024ULL,
    9511602413308477812ULL,
    9223372037392171584ULL,
    9232942186062948614ULL,
    290587763442483200ULL,
    12589056ULL,
    4503599628567104ULL,
    4764809024234455424ULL,
    729583209428287496ULL,
    281479288498192ULL,
    211260851621892ULL,
    14987979834767442216ULL,
    2396943938490432ULL,
    144343886495219968ULL,
    4538835572621320ULL,
    14141304479233937920ULL,
    9007199254807104ULL,
    2306757813726089252ULL,
    19140298433106200ULL,
    9228161784517492739ULL,
    585542718386901056ULL,
    9511602417305649284ULL,
    9223385230994872323ULL,
    8724676609ULL,
    65302211844898818ULL,
    562967133290497ULL,
    10160122958371356680ULL};