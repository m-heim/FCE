#include "position.hpp"
#include "core.hpp"
#include <iostream>

std::uint8_t Position::square_to_index(std::uint8_t rank, std::uint8_t file) {
    return rank * 8 + file;
}

void Position::set_square(uint8_t squareVal, Color colorVal, Piece pieceVal) {
    board[squareVal].color = colorVal;
    board[squareVal].piece = pieceVal;
    bitboards[pieceVal][colorVal] |= 1ULL << squareVal;
}

std::string Position::stringify_board() {
    std::string ret;
    int index = 0;
    for (int i = 0; i < RANKS; i++) {
        for(int j = 0; j < FILES; j++) {
            index = square_to_index(i, j);
            ret.push_back(piece_to_char(board[index].piece) + ('A' - 'a') * (board[index].color == Color::BLACK));
        }
        ret.push_back('\n');
    }
    return ret;
}

Square::Square() {}

double Position::evaluate(void) {
    return 0.0;
}

std::vector<move> Position::generateMoves() {
    std::vector<move> result;
    return result;
}

std::vector<move> Position::generarePawnMoves() {
    std::vector<move> moves;
    Bitboard pawns = bitboards[to_move][Piece::PAWN];
    Bitboard pawns_on_7 = pawns & rank7;
    Bitboard pawns_not_on_7 = pawns & ~rank7;
    Bitboard pawn_attacks = (pawns & notFileA) << 9 & (pawns & notFileH) << 7;
    Bitboard pawn_moves = (pawns );
    return moves;
}