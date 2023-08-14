#pragma once
#include <cstdint>

enum Color { WHITE, BLACK, COLOR_COUNT, NO_COLOR };

enum Piece { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, PIECE_COUNT, NO_PIECE };

char piece_to_char(Piece piece);

enum Castle { QUEENSIDE, KINGSIDE, CASTLE_COUNT, NO_CASTLE };

typedef std::int8_t Offset;
enum Direction : Offset {
  SOUTH = -8,
  NORTH = 8,
  WEST = -1,
  EAST = 1,
  SOUTH_WEST = SOUTH + WEST,
  SOUTH_EAST = SOUTH + EAST,
  NORTH_EAST = NORTH + EAST,
  NORTH_WEST = NORTH + WEST
};

enum File { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H };

enum Rank {
  RANK_1 = 0,
  RANK_2 = 8 * 1,
  RANK_3 = 8 * 2,
  RANK_4 = 8 * 3,
  RANK_5 = 8 * 4,
  RANK_6 = 8 * 5,
  RANK_7 = 8 * 6,
  RANK_8 = 8 * 7
};

typedef std::uint8_t SquareIndex;
enum Square : SquareIndex {
  SQUARE_A1,
  SQUARE_B1,
  SQUARE_C1,
  SQUARE_D1,
  SQUARE_E1,
  SQUARE_F1,
  SQUARE_G1,
  SQUARE_H1,
  SQUARE_A2,
  SQUARE_B2,
  SQUARE_C2,
  SQUARE_D2,
  SQUARE_E2,
  SQUARE_F2,
  SQUARE_G2,
  SQUARE_H2,
  SQUARE_A3,
  SQUARE_B3,
  SQUARE_C3,
  SQUARE_D3,
  SQUARE_E3,
  SQUARE_F3,
  SQUARE_G3,
  SQUARE_H3,
  SQUARE_A4,
  SQUARE_B4,
  SQUARE_C4,
  SQUARE_D4,
  SQUARE_E4,
  SQUARE_F4,
  SQUARE_G4,
  SQUARE_H4,
  SQUARE_A5,
  SQUARE_B5,
  SQUARE_C5,
  SQUARE_D5,
  SQUARE_E5,
  SQUARE_F5,
  SQUARE_G5,
  SQUARE_H5,
  SQUARE_A6,
  SQUARE_B6,
  SQUARE_C6,
  SQUARE_D6,
  SQUARE_E6,
  SQUARE_F6,
  SQUARE_G6,
  SQUARE_H6,
  SQUARE_A7,
  SQUARE_B7,
  SQUARE_C7,
  SQUARE_D7,
  SQUARE_E7,
  SQUARE_F7,
  SQUARE_G7,
  SQUARE_H7,
  SQUARE_A8,
  SQUARE_B8,
  SQUARE_C8,
  SQUARE_D8,
  SQUARE_E8,
  SQUARE_F8,
  SQUARE_G8,
  SQUARE_H8,
  SQUARE_COUNT,
  SQUARE_NONE
};

#define FILES 8
#define RANKS 8
#define BOARD_SIZE 64

typedef std::uint16_t move;
constexpr move no_move = 65;
typedef uint8_t relative_move;

class SquareInfo {
public:
  Color color;
  Piece piece;
  SquareInfo();
};