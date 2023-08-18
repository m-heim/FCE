#include "core.hpp"
#include "bitboard.hpp"
#include <cstdint>
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
    std::cerr << "Invalid input when converting piece to character"
              << std::endl;
    exit(1);
  }
}

void fce_error(std::string message, int exit_code) {
  std::cerr << message << std::endl;
  std::exit(exit_code);
}

std::string squareStringify(SquareIndex index) {
  std::string ret;
  uint8_t col = index % 8;
  uint8_t row = index / 8;
  ret.push_back((col + 'a'));
  ret.push_back((row + '1'));
  return ret;
}

void printSquare(SquareIndex index) {
  std::cout << squareStringify(index) << std::endl;
}