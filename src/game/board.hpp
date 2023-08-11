#pragma once
#include "core.hpp"
#include <array>
#include <cstdint>
#include <iostream>

#define FILES 8
#define RANKS 8
#define BOARD_SIZE 64


typedef std::uint8_t board_index;
constexpr board_index no_board_index = 255;



class Square {
    public:
        Color color;
        Piece piece;
        Square();
};

class Board {
    public:
    std::array<Square, 64> board;
    void set_square(uint8_t square, Color color, Piece piece);
    static std::uint8_t square_to_index(std::uint8_t rank, std::uint8_t file);
    std::string stringify_board();
};
