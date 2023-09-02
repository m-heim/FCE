#include "zobrist.hpp"

std::array<std::array<std::array<Bitboard, Square::SQUARE_COUNT>, Piece::KING + 1>,
           Color::BLACK + 1>
    zobristKeys;
Bitboard zobristSide;
std::array<Bitboard, File::FILE_COUNT> zobristEnPassant;
std::array<Bitboard, Castle::CASTLE_COUNT> zobristCastle;

void initZobrist(Bitboard seed) {
}