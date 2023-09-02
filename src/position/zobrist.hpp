#pragma once
#include "bitboard.hpp"
#include "chess.hpp"
#include <array>

extern std::array<std::array<std::array<Bitboard, Square::SQUARE_COUNT>, Piece::KING + 1>,
                  Color::BLACK + 1>
    zobristKeys;
extern Bitboard zobristSide;
extern std::array<Bitboard, File::FILE_COUNT> zobristEnPassant;
extern std::array<Bitboard, Castle::CASTLE_COUNT> zobristCastle;

void initZobrist(Bitboard seed);