#include "square.hpp"
#include <iostream>

std::string squareIndexStringify(SquareIndex index) {
    std::string ret;
    int col = index % File::FILE_COUNT;
    int row = index / Rank::RANK_COUNT;
    ret.push_back((char)(col + 'a'));
    ret.push_back((char)(row + '1'));
    return ret;
}

void squareIndexPrint(SquareIndex index) {
    std::cout << squareIndexStringify(index) << std::endl;
}

SquareIndex stringToSquareIndex(const std::string &square) {
    uint8_t col = square.at(0) - 'a';
    uint8_t row = square.at(1) - '1';
    return row * Square::SQUARE_A2 + col;
}

bool squareIndexValidate(const std::string &square) {
    return square.length() == 2 &&
           (square[0] >= 'a' && square[0] <= 'h' && square[1] >= '1' && square[1] <= '8');
}

SquareInfo::SquareInfo(Color colorVal, Piece pieceVal) {
    color = colorVal;
    piece = pieceVal;
}
SquareInfo::SquareInfo(char pieceAsChar) {
    color = (pieceAsChar >= 'a' && pieceAsChar <= 'z' ? Color::BLACK : Color::WHITE);
    if (pieceAsChar == 'p' || pieceAsChar == 'P') {
        piece = Piece::PAWN;
    } else if (pieceAsChar == 'n' || pieceAsChar == 'N') {
        piece = Piece::KNIGHT;
    } else if (pieceAsChar == 'b' || pieceAsChar == 'B') {
        piece = Piece::BISHOP;
    } else if (pieceAsChar == 'r' || pieceAsChar == 'R') {
        piece = Piece::ROOK;
    } else if (pieceAsChar == 'q' || pieceAsChar == 'Q') {
        piece = Piece::QUEEN;
    } else if (pieceAsChar == 'k' || pieceAsChar == 'K') {
        piece = Piece::KING;
    } else {
        std::cerr << "Invalid input when converting character to SquareInfo" << std::endl;
        exit(1);
    }
}