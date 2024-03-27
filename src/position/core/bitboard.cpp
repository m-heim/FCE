#include "bitboard.hpp"
#include <iostream>
#include <sstream>

/**
 * @brief Return an array of all possible subsets of bitboard positions based on a mask
 * @note https://stackoverflow.com/a/68061886,
 * https://www.chessprogramming.org/Traversing_Subsets_of_a_Set, Marcel van Kervinck Chess
 */
std::array<Bitboard, BITBOARD_SUBSETS_N> getBitboardSubsets(Bitboard mask) {
    std::array<Bitboard, BITBOARD_SUBSETS_N> result{};
    uint16_t index = 0;
    for (Bitboard current = 0;;) {
        current = ((current | ~mask) + 1) & mask;
        result[index] = current;
        index++;
        if (current == mask) {
            break;
        }
    }
    return result;
}

std::string stringifyBitboard(Bitboard board) {
    std::stringstream result;
    result << "  A B C D E F G H";
    result << std::endl;
    for (int8_t row = Rank::RANK_TOP; row >= Rank::RANK_BOTTOM; row--) {
        result << (char)('1' + row);
        for (int8_t col = File::FILE_BOTTOM; col <= File::FILE_TOP; col++) {
            result << " ";
            uint8_t index = row * Square::SQUARE_A2 + col;
            result << (board >> index) % 2;
        }
        result << std::endl;
    }
    return result.str();
}

void printBitboard(Bitboard board) {
    std::cout << stringifyBitboard(board);
}

// RAND
std::mt19937_64 randomNumberEngine(BITBOARD_RANDOM_NUMBER_SEED);
std::uniform_int_distribution<Bitboard> randomBitboardDistribution(emptyBitboard, fullBitboard);

void reseedBitboardEngine(Bitboard seed) {
    randomNumberEngine.seed(seed);
}
Bitboard getRandomBitboard() {
    return randomBitboardDistribution(randomNumberEngine);
}