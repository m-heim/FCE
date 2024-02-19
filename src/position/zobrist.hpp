#pragma once
#include <array>
#include <bitboard.hpp>
#include <chess.hpp>

/*
 @link https://www.chessprogramming.org/Zobrist_Hashing
 @brief Allows to hash a position into a 64 Bit number for transposition purposes
 */

constexpr Bitboard ZOBRIST_SEED = fileA | fileH | rank4;

extern std::array<std::array<std::array<Bitboard, Square::SQUARE_COUNT>, Piece::KING + 1>,
                  Color::BLACK + 1>
    zobristKeys;

extern Bitboard zobristSide;
extern std::array<Bitboard, File::FILE_COUNT> zobristEnPassant;
extern std::array<std::array<std::array<Bitboard, 2>, Castle::CASTLE_COUNT>, Color::BLACK + 1>
    zobristCastle;

void initZobrist(Bitboard seed);