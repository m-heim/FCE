#pragma once
#include "core.hpp"
#include <array>

#define BOARD_SIZE 64

class Square {
    public:
        Color color;
        Piece piece;
};

class Board {
    std::array<Square, BOARD_SIZE> board;
};