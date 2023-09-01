#include "bitboard.hpp"
#include "chess.hpp"
#include "magic.hpp"
#include <cstring>
#include <iostream>
#include <random>

constexpr uint64_t MIN_BITS_MAGIC = 6;
constexpr uint64_t ATTEMPTS_FIND_MAGIC = 10000000000ULL;

std::random_device randDev;
std::default_random_engine randEngine(randDev());
std::uniform_int_distribution<Bitboard> numbers(emptyBitboard, fullBitboard);

// CODE FROM Tord Romstad
Bitboard randomBitboard() {
    return numbers(randEngine);
}
Bitboard randomBitboardFewbits() {
    return randomBitboard() & randomBitboard() & randomBitboard();
}
//
Bitboard findMagics(SquareIndex square, bool bishop) {
    Bitboard mask = bishop ? getBishopMask(square) : getRookMask(square);
    uint8_t shift = bitboardGetHW(mask);
    uint16_t numOccupancies = 1 << shift;
    std::array<Bitboard, BITBOARD_SUBSETS_N> occupancies = getBitboardSubsets(mask);
    std::array<Bitboard, BITBOARD_SUBSETS_N> attacks{};
    for (int i = 0; i < numOccupancies; i++) {
        attacks[i] = bishop ? getBishopAttacks(square, occupancies[i])
                            : getRookAttacks(square, occupancies[i]);
    }
    Bitboard hashtop = 0;
    // the map that will contain the attacks after hashing the bitboard
    std::array<Bitboard, BITBOARD_SUBSETS_N> attackMap{};
    for (uint64_t attempt = 0; attempt < ATTEMPTS_FIND_MAGIC; attempt++) {
        Bitboard magic = randomBitboard() & randomBitboard();
        if (bitboardGetHW((mask * magic) & rank8) < MIN_BITS_MAGIC) {
            continue;
        }
        hashtop = 0;
        memset(&attackMap, '\0', sizeof(attackMap));
        bool success = true;
        // go through all possible occupancies
        for (int i = 0; (i < numOccupancies) && success; i++) {
            Bitboard hash = (occupancies[i] * magic) >> (Square::SQUARE_COUNT - shift);
            hashtop = std::max(hash, hashtop);
            // if no
            if (attackMap[hash] == 0) {
                attackMap[hash] = attacks[i];
            } else if (attackMap[hash] != attacks[i]) {
                success = false;
            }
        }
        if (success) {
            // uint8_t width = std::log2(hashtop) + 1;
            //  printSquare(square);
            //  std::cout << "top hash" << std::to_string(hashtop) << "at" <<
            //  std::to_string(attempt) //<< "with width" <<
            //  std::to_string(width) << std::endl; std::cout <<
            //  std::to_string(width) << std::endl; std::cout
            //  << std::hex << magic << std::endl;
            return magic;
        }
    }
    std::cerr << "Unable to find magics" << std::endl;
    exit(1);
    return 0ULL;
}

int main(int argc, char **argv) {
    bool bishop = false;
    if (argc < 2) {
        std::cerr << "Need an option" << std::endl;
        exit(2);
    }
    if (strcmp(argv[1], "r") == 0) {
        bishop = false;
    } else if (strcmp(argv[1], "b") == 0) {
        bishop = true;
    } else {
        std::cerr << "Invalid input" << std::endl;
        std::exit(2);
    }
    initGlobals();
    for (SquareIndex square = Square::SQUARE_A1; square <= Square::SQUARE_H8; square++) {
        std::cout << findMagics(square, bishop) << ",";
        std::flush(std::cout);
    }
    std::cout << std::endl;
}