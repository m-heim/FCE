#include <iostream>
#include <random>
#include "bitboard.hpp"
#include "string.h"
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

Bitboard makeMagicIndex(Bitboard board, Bitboard magic, uint8_t shift) {
  return (board * magic) >> (64 - shift);
}

Bitboard findBishopMagics(SquareIndex square) {
  Bitboard mask = getBishopMask(square);
  uint8_t shift = bitboardGetHW(mask);
  uint16_t numOccupancies = 1 << shift;
  std::cout << std::to_string(numOccupancies) << std::endl;
  // size is 1 << shift
  std::array<Bitboard, 4096> occupancies = getBitboardSubsets(mask);
  std::array<Bitboard, 4096> attacks;
  // calculate all the attacks for the occupancies
  for(int i = 0; i < numOccupancies; i++) {
    attacks[i] = getBishopAttacks(square, occupancies[i]);
  }
  Bitboard hashtop;
  // the map that will contain the attacks after hashing the bitboard
  std::array<Bitboard, 4096> attackMap;
  for(int attempt = 0; attempt < 100000000; attempt++) {
    hashtop = 0;
    memset(&attackMap, '\0', sizeof(attackMap));
    bool success = true;
    Bitboard magic = random_uint64() & random_uint64() & random_uint64();
    if(bitboardGetHW((mask * magic) & rank8) < 6) {
      continue;
    }
    // go through all possible occupancies
    for(int i = 0; (i < numOccupancies) && success; i++) {
      Bitboard hash = makeMagicIndex(occupancies[i], magic, shift);
      hashtop = std::max(hash, hashtop);
      // if no 
      if(attackMap[hash] == 0) {
        attackMap[hash] = attacks[i];
      } else if(attackMap[hash] != attacks[i]) {
        success = false;
      }
    }
    if(success) {
      //uint8_t width = std::log2(hashtop);
      //printSquare(square);
      //std::cout << "top hash" << std::to_string(hashtop) << "at" << std::to_string(attempt) //<< "with width" << std::to_string(width) << std::endl;
      //std::cout << std::hex << magic << std::endl;
      return magic;
    }
  }
  std::cerr << "Unable to find magics" << std::endl;
  exit(1);
  return 0ULL;
}

int main() {
    Magic magic;
    SquareIndex square = Square::SQUARE_C2;
    magic.mask = getBishopMask(square);
    magic.shift = bitboardGetHW(magic.mask);
    initRayAttacks();
    std::array<Bitboard, 64> magics;
    for(SquareIndex square = Square::SQUARE_A1; square <= Square::SQUARE_H8; square++) {
      magics[square] = findBishopMagics(square);
    }
}