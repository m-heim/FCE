#include "zobrist.hpp"

std::array<std::array<std::array<Bitboard, Square::SQUARE_COUNT>, Piece::KING + 1>,
           Color::BLACK + 1>
    zobristKeys;
Bitboard zobristSide;
std::array<Bitboard, File::FILE_COUNT> zobristEnPassant;
std::array<std::array<std::array<Bitboard, 2>, Castle::CASTLE_COUNT>, Color::BLACK + 1>
    zobristCastle;

void initZobrist(uint16_t seed) {
    reseedBitboardEngine(seed);
    for (uint8_t color = Color::WHITE; color <= Color::BLACK; color++) {
        for (uint8_t piece = 0; piece <= Piece::KING; piece++) {
            for (SquareIndex square = Square::SQUARE_A1; square <= Square::SQUARE_H8; square++) {
                zobristKeys[color][piece][square] = getRandomBitboard();
            }
        }
    }
    zobristSide = getRandomBitboard();
    for (uint8_t file = File::FILE_A; file <= File::FILE_H; file++) {
        zobristEnPassant[file] = getRandomBitboard();
    }
    for (uint8_t color = Color::WHITE; color <= Color::BLACK; color++) {
        for (uint8_t castle = Castle::CASTLE_BOTTOM; castle <= Castle::CASTLE_TOP; castle++) {
            zobristCastle[color][castle][false] = 0;
            zobristCastle[color][castle][true] = getRandomBitboard();
        }
    }
}