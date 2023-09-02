#pragma once
#include "bitboard.hpp"
#include "chess.hpp"
#include <array>

extern std::array<std::array<std::array<Bitboard, Square::SQUARE_COUNT>, Piece::KING + 1>,
                  Color::BLACK + 1>
    zobristKeys;
extern Bitboard zobristSide;
extern std::array<Bitboard, File::FILE_COUNT> zobristEnPassant;
extern std::array<std::array<std::array<Bitboard, 2>, Castle::CASTLE_COUNT>, Color::BLACK + 1>
    zobristCastle;

void initZobrist(uint16_t seed);