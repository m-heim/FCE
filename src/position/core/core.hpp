#pragma once
#include <cstdint>

typedef std::uint64_t Bitboard;

typedef std::int8_t Offset;
typedef std::uint8_t Depth;

enum File : uint8_t {
    FILE_A = 0,
    FILE_B = 1,
    FILE_C = 2,
    FILE_D = 3,
    FILE_E = 4,
    FILE_F = 5,
    FILE_G = 6,
    FILE_H = 7,
    FILE_TOP = FILE_H,
    FILE_BOTTOM = FILE_A,
    FILE_COUNT = FILE_TOP - FILE_BOTTOM + 1
};

enum Rank : uint8_t {
    RANK_1 = 0,
    RANK_2 = 1,
    RANK_3 = 2,
    RANK_4 = 3,
    RANK_5 = 4,
    RANK_6 = 5,
    RANK_7 = 6,
    RANK_8 = 7,
    RANK_TOP = RANK_8,
    RANK_BOTTOM = RANK_1,
    RANK_COUNT = RANK_TOP - RANK_BOTTOM + 1
};

enum Color : std::uint8_t {
    WHITE = 0,
    BLACK = 1,
    NO_COLOR = 2,
    COLOR_VALID_TOP = BLACK,
    COLOR_TOP = NO_COLOR,
    COLOR_BOTTOM = WHITE,
    COLOR_COUNT_VALID = COLOR_VALID_TOP - COLOR_BOTTOM + 1,
    COLOR_COUNT = COLOR_TOP - COLOR_BOTTOM + 1
};

enum Piece : std::uint8_t {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    NO_PIECE,
    PIECE_TOP = NO_PIECE,
    PIECE_TOP_VALID = KING,
    PIECE_BOTTOM = PAWN,
    PIECE_COUNT_VALID = PIECE_TOP_VALID - PIECE_BOTTOM + 1,
    PIECE_COUNT = PIECE_TOP - PIECE_BOTTOM + 1
};

enum Castle : std::uint8_t {
    KINGSIDE,
    QUEENSIDE,
    NO_CASTLE,
    CASTLE_BOTTOM = KINGSIDE,
    CASTLE_TOP = QUEENSIDE,
    CASTLE_VALID_TOP = QUEENSIDE,
    CASTLE_COUNT_VALID = CASTLE_VALID_TOP - CASTLE_BOTTOM + 1,
    CASTLE_COUNT = CASTLE_TOP - CASTLE_BOTTOM + 1
};

/**
 * @brief All squares on the chess board plus the NULL square
 *
 */
enum Square : uint8_t {
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

enum Direction : int8_t {
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