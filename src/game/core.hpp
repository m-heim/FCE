#pragma once
enum Color {
    WHITE,
    BLACK,
    COLOR_COUNT,
    NO_COLOR
};

enum Piece {
    NO_PIECE,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    PIECE_COUNT
};

char piece_to_char(Piece piece);

enum Castle {
    QUEENSIDE,
    KINGSIDE,
    CASTLE_COUNT,
    NO_CASTLE
};