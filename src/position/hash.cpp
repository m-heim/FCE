#include "hash.hpp"

void Zobrist::init() {
    // reseed to make sure we get the same zobrist coefficients during every execution
    reseedBitboardEngine(ZOBRIST_SEED);
    for (uint8_t color = Color::WHITE; color <= Color::BLACK; color++) {
        for (uint8_t piece = 0; piece <= Piece::KING; piece++) {
            for (SquareIndex square = Square::SQUARE_A1; square <= Square::SQUARE_H8; square++) {
                Zobrist::zobristKeys[color][piece][square] = getRandomBitboard();
            }
        }
    }
    zobristSide = getRandomBitboard();
    for (uint8_t file = File::FILE_A; file <= File::FILE_H; file++) {
        zobristEnPassant[file] = getRandomBitboard();
    }
    for (uint8_t color = Color::WHITE; color <= Color::BLACK; color++) {
        for (uint8_t castle = Castle::CASTLE_BOTTOM; castle <= Castle::CASTLE_TOP; castle++) {
            Zobrist::zobristCastle[color][castle][false] = 0;
            Zobrist::zobristCastle[color][castle][true] = getRandomBitboard();
        }
    }
}