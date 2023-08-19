#include "bitboard.hpp"
#include "core.hpp"
#include "string.h"
#include <iostream>
#include <random>
Bitboard attacks[105000];

// CODE FROM Tord Romstad
Bitboard random_uint64() {
  Bitboard u1, u2, u3, u4;
  u1 = (Bitboard)(random()) & 0xFFFF;
  u2 = (Bitboard)(random()) & 0xFFFF;
  u3 = (Bitboard)(random()) & 0xFFFF;
  u4 = (Bitboard)(random()) & 0xFFFF;
  return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}
Bitboard random_uint64_fewbits() {
  return random_uint64() & random_uint64() & random_uint64();
}
//
Bitboard findMagics(SquareIndex square, bool bishop) {
  Bitboard mask = bishop ? getBishopMask(square) : getRookMask(square);
  uint8_t shift = bitboardGetHW(mask);
  uint16_t numOccupancies = 1 << shift;
  // std::cout << std::to_string(numOccupancies) << std::endl;
  //  size is 1 << shift
  std::array<Bitboard, 4096> occupancies = getBitboardSubsets(mask);
  std::array<Bitboard, 4096> attacks;
  // calculate all the attacks for the occupancies
  for (int i = 0; i < numOccupancies; i++) {
    attacks[i] = bishop ? getBishopAttacks(square, occupancies[i])
                        : getRookAttacks(square, occupancies[i]);
  }
  Bitboard hashtop;
  // the map that will contain the attacks after hashing the bitboard
  std::array<Bitboard, 4096> attackMap;
  for (uint64_t attempt = 0; attempt < 10000000000; attempt++) {
    hashtop = 0;
    memset(&attackMap, '\0', sizeof(attackMap));
    bool success = true;
    Bitboard magic = random_uint64() & random_uint64() & random_uint64();
    if (bitboardGetHW((mask * magic) & rank8) < 6) {
      continue;
    }
    // go through all possible occupancies
    for (int i = 0; (i < numOccupancies) && success; i++) {
      Bitboard hash = getMagicIndex(occupancies[i], magic, shift);
      hashtop = std::max(hash, hashtop);
      // if no
      if (attackMap[hash] == 0) {
        attackMap[hash] = attacks[i];
      } else if (attackMap[hash] != attacks[i]) {
        success = false;
      }
    }
    if (success) {
      uint8_t width = std::log2(hashtop) + 1;
      // printSquare(square);
      // std::cout << "top hash" << std::to_string(hashtop) << "at" <<
      // std::to_string(attempt) //<< "with width" << std::to_string(width) <<
      // std::endl; std::cout << std::to_string(width) << std::endl; std::cout
      // << std::hex << magic << std::endl;
      return magic;
    }
  }
  std::cerr << "Unable to find magics" << std::endl;
  exit(1);
  return 0ULL;
}

int main(int argc, char **argv) {
  int bishop = 0;
  if (argc < 2) {
    std::cerr << "Need an option" << std::endl;
    exit(2);
  }
  if (strcmp(argv[1], "r") == 0) {
    bishop = 0;
  } else if (strcmp(argv[1], "b") == 0) {
    bishop = 1;
  } else {
    std::cerr << "Invalid input" << std::endl;
    std::exit(2);
  }
  initGlobals();
  for (SquareIndex square = Square::SQUARE_A1; square <= Square::SQUARE_H8;
       square++) {
    std::cout << findMagics(square, bishop) << ",";
    std::flush(std::cout);
  }
  std::cout << std::endl;
}