#pragma once
#include "bitboard.hpp"
#include "eval.hpp"
#include <array>
#include <strings.h>

typedef std::array<bool, Castle::CASTLE_COUNT> CastleRights;

typedef std::array<Bitboard, Piece::PIECE_COUNT> PiecesBitboards;

char pieceToChar(Piece piece);

constexpr std::array<Evaluation, Piece::PIECE_COUNT_VALID> evaluations = {
    EvaluationLiterals::PAWN_VAL, EvaluationLiterals::KNIGHT_VAL, EvaluationLiterals::BISHOP_VAL,
    EvaluationLiterals::ROOK_VAL, EvaluationLiterals::QUEEN_VAL,  EvaluationLiterals::KING_VAL};
