#include "core.hpp"
#include <iostream>

char piece_to_char(Piece piece) {
    if (piece == Piece::PAWN) {
        return 'p';
    } else if (piece == Piece::KNIGHT) {
        return 'n';
    } else if (piece == Piece::BISHOP) {
        return 'b';
    } else if (piece == Piece::ROOK) {
        return 'r';
    } else if (piece == Piece::QUEEN) {
        return 'q';
    } else if (piece == Piece::KING) {
        return 'k';
    } else if (piece == Piece::NO_PIECE) {
        return ' ';
    } else {
        std::cerr << "Invalid input when converting piece to character" << std::endl;
        exit(1);
    }
}