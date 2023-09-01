#include "bitboard.hpp"
#include <iostream>

std::array<Bitboard, BITBOARD_SUBSETS_N> getBitboardSubsets(Bitboard mask) {
    // https://stackoverflow.com/a/68061886
    // https://www.chessprogramming.org/Traversing_Subsets_of_a_Set
    // use carry bit while masking all non relevant bits to iterate through
    // all subsets: Marcel van Kervinck Chess
    std::array<Bitboard, BITBOARD_SUBSETS_N> result{};
    uint16_t index = 0;
    for (Bitboard current = 0; current != mask; current = ((current | ~mask) + 1) & mask) {
        result[index] = current;
        index++;
    }
    return result;
}

void printBitboard(Bitboard board) {
    std::cout << "  A B C D E F G H" << std::endl;
    for (int8_t row = Rank::RANK_TOP; row >= 0; row--) {
        std::cout << (char)('1' + row);
        for (int8_t col = 0; col <= File::FILE_TOP; col++) {
            std::cout << " ";
            uint8_t index = row * Square::SQUARE_A2 + col;
            std::cout << (board >> index) % 2;
        }
        std::cout << "\n";
    }
}