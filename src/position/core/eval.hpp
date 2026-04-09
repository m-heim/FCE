#ifndef EVAL_HPP
#define EVAL_HPP
#include <cstdint>

// evaluation in centipawns, positive means advantage for white, negative means advantage for black
typedef std::int64_t Evaluation;
enum EvaluationLiterals : Evaluation {
    NEG_INF = -100000000000,
    MATE = NEG_INF + 1000,
    EVEN = 0,
    POS_INF = 100000000000,

    PAWN_VAL = 100,
    KNIGHT_VAL = 300,
    BISHOP_VAL = 315,
    ROOK_VAL = 450,
    QUEEN_VAL = 900,
    KING_VAL = 2000000,

    MULTIPLE_PAWN_VAL = -10,
    MULTIPLE_BISHOP_VAL = 15,
    CENTER_PAWN_VAL = 8,
    SAFE_KING_VAL = 18,
    EXTENDED_CENTER_KING_VAL = -50,
    BORDER_KNIGHT_VAL = -8,
    CORNER_BISHOP_VAL = -8,
    CENTER_PIECE_VAL = 4,
    MOBILITY_MULTIPLIER = 4,

    MARGIN = 70,
};
#endif // EVAL_HPP