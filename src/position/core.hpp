#pragma once
#include <cstdint>

#define MOVE_LIMIT_N 218

typedef uint64_t Bitboard;

enum Color { WHITE, BLACK, COLOR_COUNT, NO_COLOR };

enum Piece { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, PIECE_COUNT, NO_PIECE };

char piece_to_char(Piece piece);

enum Castle { QUEENSIDE, KINGSIDE, CASTLE_COUNT, NO_CASTLE };

enum Promotion {
  NO_PROMOTION = 0,
  QUEEN_PROMOTION = 1,
  ROOK_PROMOTION = 2,
  BISHOP_PROMOTION = 3,
  KNIGHT_PROMOTION = 4
};

enum MoveFlags : std::uint8_t {
  QUIET,
  CAPTURE,
  EN_PASSANT,
  CASTLE_QUEENSIDE,
  CASTLE_KINGSIDE,
  PROMOTE_QUEEN,
  PROMOTE_ROOK,
  PROMOTE_BISHOP,
  PROMOTE_KNIGHT
};

typedef std::int8_t Offset;

enum Direction : Offset {
  SOUTH = -8,
  NORTH = 8,
  WEST = -1,
  EAST = 1,
  SOUTH_WEST = SOUTH + WEST,
  SOUTH_EAST = SOUTH + EAST,
  NORTH_EAST = NORTH + EAST,
  NORTH_WEST = NORTH + WEST,
  SSW = SOUTH + SOUTH + WEST,
  SSE = SOUTH + SOUTH + EAST,
  EES = EAST + EAST + SOUTH,
  EEN = EAST + EAST + NORTH,
  NNE = NORTH + NORTH + EAST,
  NNW = NORTH + NORTH + WEST,
  WWN = WEST + WEST + NORTH,
  WWS = WEST + WEST + SOUTH
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

typedef std::uint16_t move;
constexpr move no_move = 65;

inline move serialize_move(SquareIndex from, SquareIndex to, uint8_t flags) {
  return to | from << 6 | flags << 12;
}

inline SquareIndex moveGetFrom(move m) { return (m >> 6) & 0x3f; }
inline SquareIndex moveGetTo(move m) { return m & 0x3f; }
inline std::uint8_t moveGetFlags(move m) { return (m >> 12) & 0x3f; }

class SquareInfo {
public:
  Color color;
  Piece piece;
  SquareInfo();
};
