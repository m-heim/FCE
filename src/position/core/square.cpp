#include "square.hpp"
#include <iostream>
#include <map>

std::string SquareIndex::stringify() {
    int col = *this % File::FILE_COUNT;
    int row = *this / Rank::RANK_COUNT;
    return std::string({(char)(col + 'a'), (char)(row + '1')});
}

void SquareIndex::print() {
    std::cout << this->stringify() << std::endl;
}