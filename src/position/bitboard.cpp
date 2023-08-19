#include "bitboard.hpp"
#include "core.hpp"
#include <bit>
#include <vector>
#include <iostream>

Bitboard getKnightAttacks(Bitboard board) {
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
  int8_t toShift = ((index & Square::SQUARE_H1) - (index >> 3)) * 8;
  return toShift >= 0 ? diagonal >> toShift : diagonal << -toShift;
}

Bitboard getDiagonal2Mask(SquareIndex index) {
  int8_t toShift = (7 - (index & Square::SQUARE_H1) - (index >> 3)) * 8;
  return toShift >= 0 ? diagonal2 >> toShift : diagonal2 << -toShift;
}

Bitboard getRankMask(SquareIndex index) {
  return rank1 << (index & Square::SQUARE_A8);
}

Bitboard getFileMask(SquareIndex index) {
  return fileA << (index & Square::SQUARE_H1);
}

Bitboard getRookMask(SquareIndex index) {
  return getFileMask(index) ^ getRankMask(index);
}

Bitboard getQueenMask(SquareIndex index) {
  return getRookMask(index) ^ getBishopMask(index);
}

Bitboard getBishopMask(SquareIndex index) {
  return getDiagonalMask(index) ^ getDiagonal2Mask(index);
}

std::array<std::array<Bitboard, Square::SQUARE_COUNT>, 8> rays;

Bitboard getRookAttacks(SquareIndex index, Bitboard occupancy) {
  std::array<Direction, 4> rookDirections = {Direction::SOUTH, Direction::EAST, Direction::NORTH, Direction::WEST};
}

void initRayAttacks() {

  Bitboard north = fileA << 8;
  for (SquareIndex square = Square::SQUARE_A1; square <= Square::SQUARE_H8;
       square++, north <<= 1) {
    rays[RayDirection::NORTH_RAY][square] = north;
  }
}

void printBitboard(Bitboard board) {
  std::cout << "  A B C D E F G H" << std::endl;
  for (int8_t row = 7; row >= 0; row--) {
    std::cout << (char)('1' + row);
    for (int8_t col = 0; col <= 7; col++) {
      std::cout << " ";
      SquareIndex index = row * 8 + col;
      std::cout << (board >> index) % 2;
    }
    std::cout << "\n";
  }
}
std::array<std::array<Bitboard, Square::SQUARE_COUNT>, 2> pawnAttacks;
std::array<Bitboard, Square::SQUARE_COUNT> knightAttacks;
std::array<Bitboard, Square::SQUARE_COUNT> kingAttacks;
std::array<std::array<Bitboard, Square::SQUARE_COUNT>, 2> pawnPushes;
std::array<std::array<Bitboard, Square::SQUARE_COUNT>, 2> pawnDoublePushes;

void initMagics() {
  for (SquareIndex index = SQUARE_A1; index <= SQUARE_H8; index++) {
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
  }
}

std::vector<Bitboard> getBitboardSubsets(Bitboard mask) {
  std::vector<Bitboard> result;
  // https://stackoverflow.com/a/68061886
  // use carry bit while masking all non relevant bits to iterate through all subsets
  for(Bitboard current = 0;current != mask; current = ((current | ~mask) + 1) & mask) {
    result.push_back(current);
  }
  return result;
}