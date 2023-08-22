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

SquareInfo charToSquareInfo(char piece) {
  SquareInfo info{};
  info.color = piece >= 'a' && piece <= 'z' ? Color::BLACK : Color::WHITE;
  if (piece == 'p' || piece == 'P') {
    info.piece = Piece::PAWN;
  } else if (piece == 'n' || piece == 'N') {
    info.piece = Piece::KNIGHT;
  } else if (piece == 'b' || piece == 'B') {
    info.piece = Piece::BISHOP;
  } else if (piece == 'r' || piece == 'R') {
    info.piece = Piece::ROOK;
  } else if (piece == 'q' || piece == 'Q') {
    info.piece = Piece::QUEEN;
    //} else if (piece == ' ') {
    //  info.piece = Piece::NO_PIECE;
    //  info.color = Color::NO_COLOR;
  } else if (piece == 'k' || piece == 'K') {
    info.piece = Piece::KING;
  } else {
    std::cerr << piece << std::endl;
    std::cerr << "Invalid input when converting character to SquareInfo"
              << std::endl;
    exit(1);
  }
  return info;
}

void fce_error(std::string message, int exit_code) {
  std::cerr << message << std::endl;
  std::exit(exit_code);
}

std::string squareStringify(SquareIndex index) {
  std::string ret = "";
  uint8_t col = index % 8;
  uint8_t row = index / 8;
  ret.push_back((col + 'a'));
  ret.push_back((row + '1'));
  return ret;
}

void printSquare(SquareIndex index) {
  std::cout << squareStringify(index) << std::endl;
}

SquareInfo::SquareInfo(Color colorVal, Piece pieceVal) {
  color = colorVal;
  piece = pieceVal;
}

// NOTE inline this somehow

void printBitboard(Bitboard board) {
  std::cout << "  A B C D E F G H" << std::endl;
  for (int8_t row = 7; row >= 0; row--) {
    std::cout << (char)('1' + row);
    for (int8_t col = 0; col <= 7; col++) {
      std::cout << " ";
      uint8_t index = row * 8 + col;
      std::cout << (board >> index) % 2;
    }
    std::cout << "\n";
  }
}

std::array<Bitboard, 4096> getBitboardSubsets(Bitboard mask) {
  // https://stackoverflow.com/a/68061886
  // https://www.chessprogramming.org/Traversing_Subsets_of_a_Set
  // use carry bit while masking all non relevant bits to iterate through all
  // subsets: Marcel van Kervinck Chess
  std::array<Bitboard, 4096> result;
  uint16_t i = 0;
  for (Bitboard current = 0; current != mask;
       current = ((current | ~mask) + 1) & mask) {
    result[i] = current;
    i++;
  }
  return result;
}

// void MoveList::addMoves(const SquareIndex &from, Bitboard &board,
//                         MoveFlags flags) {
//   while (board) {
//     SquareIndex to = get_ls1b_index(board);
//     push_back(serialize_move(from, to, flags));
//     board &= bitboardUnsetSquare(board, to);
//   }
// }