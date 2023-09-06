#pragma once
#include "chess.hpp"
#include <cstdint>
#include <iostream>
#include <string>

// enumerations
enum Square : uint8_t {
    SQUARE_A1,
    SQUARE_B1,
    SQUARE_C1,
    SQUARE_D1,
    SQUARE_E1,
    SQUARE_F1,
    SQUARE_G1,
    SQUARE_H1,
    SQUARE_A2,
    SQUARE_B2,
    SQUARE_C2,
    SQUARE_D2,
    SQUARE_E2,
    SQUARE_F2,
    SQUARE_G2,
    SQUARE_H2,
    SQUARE_A3,
    SQUARE_B3,
    SQUARE_C3,
    SQUARE_D3,
    SQUARE_E3,
    SQUARE_F3,
    SQUARE_G3,
    SQUARE_H3,
    SQUARE_A4,
    SQUARE_B4,
    SQUARE_C4,
    SQUARE_D4,
    SQUARE_E4,
    SQUARE_F4,
    SQUARE_G4,
    SQUARE_H4,
    SQUARE_A5,
    SQUARE_B5,
    SQUARE_C5,
    SQUARE_D5,
    SQUARE_E5,
    SQUARE_F5,
    SQUARE_G5,
    SQUARE_H5,
    SQUARE_A6,
    SQUARE_B6,
    SQUARE_C6,
    SQUARE_D6,
    SQUARE_E6,
    SQUARE_F6,
    SQUARE_G6,
    SQUARE_H6,
    SQUARE_A7,
    SQUARE_B7,
    SQUARE_C7,
    SQUARE_D7,
    SQUARE_E7,
    SQUARE_F7,
    SQUARE_G7,
    SQUARE_H7,
    SQUARE_A8,
    SQUARE_B8,
    SQUARE_C8,
    SQUARE_D8,
    SQUARE_E8,
    SQUARE_F8,
    SQUARE_G8,
    SQUARE_H8,
    SQUARE_COUNT = SQUARE_H8 - SQUARE_A1 + 1,
    SQUARE_NONE
};

enum Direction : int8_t {
    SOUTH = -8,
    NORTH = 8,
    WEST = -1,
    EAST = 1,
    SOUTH_WEST = SOUTH + WEST,
    SOUTH_EAST = SOUTH + EAST,
    NORTH_EAST = NORTH + EAST,
    NORTH_WEST = NORTH + WEST,
    SSW = SOUTH + SOUTH + WEST,
    SSE = SOUTH + SOUTH + EAST,
    EES = EAST + EAST + SOUTH,
    EEN = EAST + EAST + NORTH,
    NNE = NORTH + NORTH + EAST,
    NNW = NORTH + NORTH + WEST,
    WWN = WEST + WEST + NORTH,
    WWS = WEST + WEST + SOUTH
};

class SquareIndex {
  public:
    using Squares = Square;
    operator int() const {
        return square;
    };
    SquareIndex &operator++() {
        square++;
        return *this;
    }
    SquareIndex() {
        square = Squares::SQUARE_NONE;
    }
    SquareIndex operator++(int) {
        SquareIndex old = *this;
        operator++();
        return old;
    }
    SquareIndex(int index) {
        square = index;
    };
    explicit SquareIndex(const std::string &strs) {
        square = ((strs.at(1) - '1') * Square::SQUARE_A2) + (strs.at(0) - 'a');
    }
    std::string stringify();
    void print();
    static bool validate(const std::string &strs) {
        return strs.length() == 2 &&
               (strs[0] >= 'a' && strs[0] <= 'h' && strs[1] >= '1' && strs[1] <= '8');
    }

  private:
    uint8_t square;
};

// classes
class SquareInfo {
  public:
    Color color;
    Piece piece;
    SquareInfo() {
        color = Color::NO_COLOR;
        piece = Piece::NO_PIECE;
    }
    SquareInfo(Color colorVal, Piece pieceVal) {
        color = colorVal;
        piece = pieceVal;
    }
    SquareInfo(char pieceAsChar) {
        color = (pieceAsChar >= 'a' && pieceAsChar <= 'z' ? Color::BLACK : Color::WHITE);
        if (pieceAsChar == 'p' || pieceAsChar == 'P') {
            piece = Piece::PAWN;
        } else if (pieceAsChar == 'n' || pieceAsChar == 'N') {
            piece = Piece::KNIGHT;
        } else if (pieceAsChar == 'b' || pieceAsChar == 'B') {
            piece = Piece::BISHOP;
        } else if (pieceAsChar == 'r' || pieceAsChar == 'R') {
            piece = Piece::ROOK;
        } else if (pieceAsChar == 'q' || pieceAsChar == 'Q') {
            piece = Piece::QUEEN;
        } else if (pieceAsChar == 'k' || pieceAsChar == 'K') {
            piece = Piece::KING;
        } else {
            std::cerr << "Invalid input when converting character to SquareInfo" << std::endl;
            exit(1);
        }
    }
};