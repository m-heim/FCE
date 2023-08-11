#include "board.hpp"
#include "core.hpp"
#include <iostream>

std::uint8_t Board::square_to_index(std::uint8_t rank, std::uint8_t file) {
    return rank * 8 + file;
}
void Board::set_square(uint8_t squareVal, Color colorVal, Piece pieceVal) {
    board[squareVal].color = colorVal;
    board[squareVal].piece = pieceVal;
}

std::string Board::stringify_board() {
    std::string ret;
    for (int i = 0; i < RANKS; i++) {
        for(int j = 0; j < FILES; j++) {
            ret.push_back(piece_to_char(board[square_to_index(i, j)].piece));
        }
        ret.push_back('\n');
    }
    return ret;
}

Square::Square() {}