#pragma once
#include <cstdint>
#include <limits>
#include <string>

#define MOVE_LIMIT_N 218

typedef std::int64_t Evaluation;
typedef std::uint64_t Bitboard;
typedef std::int8_t Offset;
typedef std::uint8_t SquareIndex;
typedef std::uint16_t Move;
typedef std::array<Move, MOVE_LIMIT_N> MoveList;
typedef std::pair<Move, Evaluation> SearchInfo;

enum EvaluationLiterals : Evaluation {
  NEG_INF = -100000000000,
  INVALID_MOVE = NEG_INF + 1,
  PAWN_VAL = 100,
  KNIGHT_VAL = 300,
  BISHOP_VAL = 315,
  ROOK_VAL = 450,
  QUEEN_VAL = 900,
  KING_VAL = 2000000,
  POS_INF = 100000000000,
  EVEN = 0
};

enum Color : uint8_t { WHITE, BLACK, NO_COLOR, COLOR_TOP = NO_COLOR };

enum Piece : uint8_t {
  PAWN,
  KNIGHT,
  BISHOP,
  ROOK,
  QUEEN,
  KING,
  NO_PIECE,
  PIECE_TOP = NO_PIECE
};

enum Castle {
  QUEENSIDE,
  KINGSIDE,
  CASTLE_COUNT,
  NO_CASTLE,
  CASTLE_TOP = NO_CASTLE
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
  PROMOTE_KNIGHT,
  MASK = 0x0F
};

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

enum RayDirection : Offset {
  SOUTH_EAST_RAY,
  SOUTH_RAY,
  SOUTH_WEST_RAY,
  WEST_RAY,
  NORTH_WEST_RAY,
  NORTH_RAY,
  NORTH_EAST_RAY,
  EAST_RAY,
  POSITIVE = NORTH_WEST_RAY,
  RAY_COUNT = EAST_RAY + 1
};

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
  SQUARE_COUNT = SQUARE_H8 - SQUARE_A1 + 1,
  SQUARE_NONE
};

constexpr Move no_move = 65;

// INLINE FUNCTIONS FAST PROCESSING

inline Move serialize_move(SquareIndex from, SquareIndex to, uint8_t flags) {
  return to | from << 6 | flags << 12;
}

inline SquareIndex moveGetFrom(Move m) { return (m >> 6) & 0x3f; }
inline SquareIndex moveGetTo(Move m) { return m & 0x3f; }
// FIXME
inline MoveFlags moveGetFlags(Move m) {
  return static_cast<MoveFlags>((m >> 12) & MoveFlags::MASK);
}

// non performance relevant functions
void fce_error(std::string message, int exit_code);

std::string squareStringify(SquareIndex index);

char piece_to_char(Piece piece);

class SquareInfo {
public:
  Color color;
  Piece piece;
  SquareInfo();
  SquareInfo(Color colorVal, Piece pieceVal);
};

SquareInfo charToSquareInfo(char piece);

void printSquare(SquareIndex index);