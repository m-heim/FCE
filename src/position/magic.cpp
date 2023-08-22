#include "magic.hpp"
#include "core.hpp"
#include <iostream>

std::array<Bitboard, MAGICS_ARRAY_SIZE> magics;
std::array<Magic, Square::SQUARE_COUNT> rookMagics;
std::array<Magic, Square::SQUARE_COUNT> bishopMagics;
std::array<std::array<Bitboard, Square::SQUARE_COUNT>, 2> pawnAttacks;
std::array<Bitboard, Square::SQUARE_COUNT> knightAttacks;
std::array<Bitboard, Square::SQUARE_COUNT> kingAttacks;
std::array<std::array<Bitboard, Square::SQUARE_COUNT>, 2> pawnPushes;
std::array<std::array<Bitboard, Square::SQUARE_COUNT>, 2> pawnDoublePushes;

void initGlobals() {
  initRayAttacks();
  initMagics();
  initMasks();
}

std::array<Bitboard, Square::SQUARE_COUNT> bishopMasks;
std::array<Bitboard, Square::SQUARE_COUNT> rookMasks;
std::array<Bitboard, Square::SQUARE_COUNT> queenMasks;
std::array<Bitboard, Square::SQUARE_COUNT> maskedSquare;
std::array<Bitboard, Square::SQUARE_COUNT> unmaskedSquare;

void initMasks() {
  for (SquareIndex index = SQUARE_A1; index <= Square::SQUARE_H8; index++) {
    bishopMasks[index] = getBishopMask(index);
    rookMasks[index] = getRookMask(index);
    queenMasks[index] = getQueenMask(index);
    maskedSquare[index] = bitboardSetSquare(index);
    unmaskedSquare[index] = bitboardUnsetSquare(fullBitboard, index);
  }
}

void initMagics() {
  uint64_t slidingIndex = 0;
  for (SquareIndex index = Square::SQUARE_A1; index <= Square::SQUARE_H8;
       index++) {
    Bitboard board = bitboardSetSquare(index);
    Bitboard attacks_white = ((board & notFileA) << Direction::NORTH_WEST |
                              ((board & notFileH) << Direction::NORTH_EAST));
    Bitboard attacks_black = ((board & notFileH) >> Direction::NORTH_WEST |
                              ((board & notFileA) >> Direction::NORTH_EAST));
    pawnAttacks[Color::WHITE][index] = attacks_white;
    pawnAttacks[Color::BLACK][index] = attacks_black;
    pawnPushes[Color::WHITE][index] = board << Direction::NORTH;
    pawnPushes[Color::BLACK][index] = board >> Direction::NORTH;
    pawnDoublePushes[Color::WHITE][index] = board << (Direction::NORTH * 2);
    pawnDoublePushes[Color::BLACK][index] = board >> (Direction::NORTH * 2);
    knightAttacks[index] = getKnightAttacks(board);
    kingAttacks[index] = getKingAttacks(board);
    bishopMagics[index] = initMagicSquare(index, true, &slidingIndex);
    rookMagics[index] = initMagicSquare(index, false, &slidingIndex);
  }
}

Magic initMagicSquare(SquareIndex index, bool bishop, uint64_t *magicIndex) {
  Magic result;
  Bitboard mask = bishop ? getBishopMask(index) : getRookMask(index);
  Bitboard shift = bitboardGetHW(mask);
  Bitboard magicNumber =
      bishop ? bishopMagicNumbers[index] : rookMagicNumbers[index];
  Bitboard *start = &magics[*magicIndex];
  result.mask = mask;
  result.magic = magicNumber;
  result.attacks = start;
  result.shift = shift;
  std::array<Bitboard, 4096> occupancies = getBitboardSubsets(mask);
  std::array<Bitboard, 4096> attacks;
  uint16_t combinations = (1 << shift);
  // calculate all the attacks for the occupancies
  for (int i = 0; i < combinations; i++) {
    attacks[i] = bishop ? getBishopAttacks(index, occupancies[i])
                        : getRookAttacks(index, occupancies[i]);
    // printBitboard(getBishopAttacks(index, occupancies[i]));
    // printBitboard(occupancies[i]);
  }
  for (int i = 0; i < combinations; i++) {
    Bitboard index = getMagicIndex(occupancies[i], magicNumber, shift);
    *(start + index) = attacks[i];
  }
  *magicIndex += combinations;
  std::cout << std::to_string(*magicIndex) << std::endl;
  return result;
}

std::array<Bitboard, Square::SQUARE_COUNT> rankAttacks;
std::array<Bitboard, Square::SQUARE_COUNT> fileAttacks;
std::array<std::array<Bitboard, Square::SQUARE_COUNT>, RayDirection::RAY_COUNT>
    rays;

void initRayAttacks() {
  for (SquareIndex square = Square::SQUARE_A1; square <= Square::SQUARE_H8;
       square++) {
    Bitboard origin = bitboardSetSquare(square);
    Bitboard fileMask = fileAttacks[square];
    Bitboard rankMask = rankAttacks[square];
    Bitboard diagonalMask = getDiagonalMask(square);
    Bitboard diagonal2Mask = getDiagonal2Mask(square);
    Bitboard positive = ((~emptyBitboard) << square);
    Bitboard north = (fileMask & positive) ^ origin;
    Bitboard east = (rankMask & positive) ^ origin;
    Bitboard south = north ^ fileMask ^ origin;
    Bitboard west = east ^ rankMask ^ origin;
    Bitboard northWest = (diagonal2Mask & positive) ^ origin;
    Bitboard northEast = (diagonalMask & positive) ^ origin;
    Bitboard southEast = northWest ^ diagonal2Mask ^ origin;
    Bitboard southWest = northEast ^ diagonalMask ^ origin;
    rays[RayDirection::NORTH_RAY][square] = north;
    rays[RayDirection::EAST_RAY][square] = east;
    rays[RayDirection::SOUTH_RAY][square] = south;
    rays[RayDirection::WEST_RAY][square] = west;
    rays[RayDirection::NORTH_WEST_RAY][square] = northWest;
    rays[RayDirection::NORTH_EAST_RAY][square] = northEast;
    rays[RayDirection::SOUTH_WEST_RAY][square] = southWest;
    rays[RayDirection::SOUTH_EAST_RAY][square] = southEast;
    rankAttacks[square] = rank1 << (square & Square::SQUARE_A8);
    fileAttacks[square] = fileA << (square >> 3);
  }
}

Bitboard getRayAttacks(Bitboard occupied, RayDirection direction,
                       SquareIndex square) {
  // get all possible attacks
  // TODO does this go onto the target square?
  Bitboard attacks = rays[direction][square];
  Bitboard blocker = attacks & occupied;
  square = direction >= RayDirection::POSITIVE
               ? get_ls1b_index(blocker | (fileH & rank8))
               : get_ms1b_index(blocker | (fileA & rank1));
  return attacks ^ rays[direction][square];
}

Bitboard getRookAttacks(SquareIndex index, Bitboard occupancy) {
  Bitboard result = 0ULL;
  std::array<RayDirection, 4> rookDirections = {
      RayDirection::SOUTH_RAY, RayDirection::EAST_RAY, RayDirection::NORTH_RAY,
      RayDirection::WEST_RAY};
  for (RayDirection direction : rookDirections) {
    result |= getRayAttacks(occupancy, direction, index);
  }
  return result;
}

Bitboard getBishopAttacks(SquareIndex index, Bitboard occupancy) {
  Bitboard result = 0ULL;
  std::array<RayDirection, 4> rookDirections = {
      RayDirection::SOUTH_EAST_RAY, RayDirection::NORTH_EAST_RAY,
      RayDirection::NORTH_WEST_RAY, RayDirection::SOUTH_WEST_RAY};
  for (Offset i = 0; i < 4; i++) {
    result |= getRayAttacks(occupancy, rookDirections[i], index);
  }
  return result;
}

Bitboard getKnightAttacks(Bitboard board) {
  // Generate all possible knight moves from knights
  return ((board << Direction::NNE | board >> -Direction::SSE) & notFileA) |
         ((board << Direction::NNW | board >> -Direction::SSW) & notFileH) |
         ((board << Direction::WWN | board >> -Direction::WWS) & notFileH &
          notFileG) |
         ((board << Direction::EEN | board >> -Direction::EES) & notFileA &
          notFileB);
}

Bitboard getKingAttacks(Bitboard board) {
  Bitboard cols =
      (board << Direction::NORTH) | (board >> Direction::NORTH) | board;
  return ((cols >> -Direction::WEST) & notFileH) |
         ((cols << Direction::EAST) & notFileA) | cols;
}

Bitboard getDiagonalMask(SquareIndex index) {
  int8_t toShift =
      ((index & Square::SQUARE_H1) - (index >> 3)) * Square::SQUARE_A2;
  return toShift >= 0 ? diagonal >> toShift : diagonal << -toShift;
}

Bitboard getDiagonal2Mask(SquareIndex index) {
  int8_t toShift =
      (Square::SQUARE_H1 - (index & Square::SQUARE_H1) - (index >> 3)) *
      Square::SQUARE_A2;
  return toShift >= 0 ? diagonal2 >> toShift : diagonal2 << -toShift;
}

Bitboard getRankMask(SquareIndex index) {
  return rank1 << (index & Square::SQUARE_A8);
}

Bitboard getFileMask(SquareIndex index) {
  return fileA << (index & Square::SQUARE_H1);
}

Bitboard getRookMask(SquareIndex index) {
  // mask for magic, no borders, careful
  return ((getFileMask(index) & notRank1 & notRank8) ^
          (getRankMask(index) & notFileA & notFileH));
}

Bitboard getQueenMask(SquareIndex index) {
  // mask for magic, no borders, careful
  return (getRookMask(index) ^ getBishopMask(index));
}

Bitboard getBishopMask(SquareIndex index) {
  // mask for magic, no borders, careful
  return (getDiagonalMask(index) ^ getDiagonal2Mask(index)) & notBorders;
}