#pragma once
#include <cstdint>
#include <strings.h>

typedef std::int64_t Evaluation;
typedef std::int8_t Offset;

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
    COLOR_TOP = NO_COLOR
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
    PIECE_BOTTOM = PAWN,
    PIECE_COUNT = PIECE_TOP - PIECE_BOTTOM + 1
};

enum Castle : std::uint8_t {
    KINGSIDE,
    QUEENSIDE,
    CASTLE_BOTTOM = KINGSIDE,
    CASTLE_TOP = QUEENSIDE,
    CASTLE_COUNT = CASTLE_TOP - CASTLE_BOTTOM + 1
};

char pieceToChar(Piece piece);
