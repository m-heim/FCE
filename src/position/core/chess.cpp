#include "chess.hpp"
#include <cstdint>
#include <iostream>

char pieceToChar(Piece piece) {
    if (piece == Piece::PAWN) {
        return 'p';
    }
    if (piece == Piece::KNIGHT) {
        return 'n';
    }
    if (piece == Piece::BISHOP) {
        return 'b';
    }
    if (piece == Piece::ROOK) {
        return 'r';
    }
    if (piece == Piece::QUEEN) {
        return 'q';
    }
    if (piece == Piece::KING) {
        return 'k';
    }
    if (piece == Piece::NO_PIECE) {
        return ' ';
    }
    std::cerr << "Invalid input when converting piece to character" << std::endl;
    exit(1);
}

void fce_error(const std::string &message, int exit_code) {
    std::cerr << message << std::endl;
    std::exit(exit_code);
}