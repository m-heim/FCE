#include "core.hpp"
#include "bitboard.hpp"
#include <cstdint>
#include <iostream>
#include <strings.h>

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

uint8_t get_ls1b_index(Bitboard bitboard) {
  return ffsll(bitboard);
}