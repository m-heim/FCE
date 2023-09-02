#include "bitboard.hpp"
#include "chess.hpp"
#include "magic.hpp"
#include <cstring>
#include <iostream>
#include <random>

constexpr uint64_t MIN_BITS_MAGIC = 6;
constexpr uint64_t ATTEMPTS_FIND_MAGIC = 10000000000ULL;

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
    // the map that will contain the attacks after hashing the bitboard
    std::array<Bitboard, BITBOARD_SUBSETS_N> attackMap{};
    for (uint64_t attempt = 0; attempt < ATTEMPTS_FIND_MAGIC; attempt++) {
        Bitboard magic = getRandomBitboard() & getRandomBitboard(); // Tord Romstad
        if (bitboardGetHW((mask * magic) & rank8) < MIN_BITS_MAGIC) {
            continue;
        }
        memset(&attackMap, 0, sizeof(attackMap));
        bool success = true;
        for (int i = 0; (i < numOccupancies) && success; i++) {
            Bitboard hash = getMagicIndex(occupancies[i], magic, Square::SQUARE_COUNT - shift);
            if (attackMap[hash] == 0) {
                attackMap[hash] = attacks[i];
            } else if (attackMap[hash] != attacks[i]) {
                success = false;
            }
        }
        if (success) {
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
    reseedBitboardEngine(std::atoi(argv[2]));
    for (SquareIndex square = Square::SQUARE_A1; square <= Square::SQUARE_H8; square++) {
        std::cout << findMagics(square, bishop) << ",";
        std::flush(std::cout);
    }
    std::cout << std::endl;
}