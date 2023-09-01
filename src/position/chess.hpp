#pragma once
#include <array>
#include <cstdint>
#include <limits>
#include <string>
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
    FILE_COUNT = 8
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
    RANK_COUNT = 8,
};

enum EvaluationLiterals : Evaluation {
    NEG_INF = -100000000000,
    // INVALID_MOVE = NEG_INF + 1,
    PAWN_VAL = 100,
    KNIGHT_VAL = 300,
    BISHOP_VAL = 315,
    ROOK_VAL = 450,
    QUEEN_VAL = 900,
    KING_VAL = 2000000,
    POS_INF = 100000000000,
    MATE = NEG_INF + 1000,
    MULTIPLE_PAWN_VAL = -50,
    MULTIPLE_BISHOP_VAL = 150,
    CENTER_PAWN_VAL = 100,
    EVEN = 0
};

enum Color : std::uint8_t {
    WHITE,
    BLACK,
    NO_COLOR,
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
    PIECE_TOP = NO_PIECE
};

enum Castle : std::uint8_t {
    QUEENSIDE,
    KINGSIDE,
    CASTLE_COUNT,
    NO_CASTLE,
    CASTLE_TOP = NO_CASTLE
};

// INLINE FUNCTIONS FAST PROCESSING
// bitboard manipulation

constexpr std::array<Evaluation, Piece::KING + 1> evaluations = {
    EvaluationLiterals::PAWN_VAL, EvaluationLiterals::KNIGHT_VAL, EvaluationLiterals::BISHOP_VAL,
    EvaluationLiterals::ROOK_VAL, EvaluationLiterals::QUEEN_VAL,  EvaluationLiterals::KING_VAL};

// non performance relevant functions
void fce_error(const std::string &message, int exit_code);

char pieceToChar(Piece piece);
