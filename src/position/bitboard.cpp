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
    Bitboard attacks = ((board & notFileA) << Direction::NORTH_WEST |
                        ((board & notFileH) << Direction::NORTH_EAST));
    pawnAttacks[Color::WHITE][index] = attacks;
    knightAttacks[index] = getKnightAttacks(board);
  }
}