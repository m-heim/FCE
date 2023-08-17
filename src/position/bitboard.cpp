#include "bitboard.hpp"
#include "core.hpp"
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
    Bitboard cols = (board << Direction::NORTH) | (board >> Direction::NORTH) | board;
    return ((cols >> -Direction::WEST) & notFileH) | ((cols << Direction::EAST) & notFileA) | cols;
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

MagicBitboards::MagicBitboards() {
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