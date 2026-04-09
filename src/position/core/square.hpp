#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <util.hpp>
#include <array>
#include <core.hpp>


/**
 * @brief a SquareIndex identifies a Square (uint8_t) with some utility functions
 *
 */
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
    std::string stringify() {
        int col = *this % File::FILE_COUNT;
        int row = *this / Rank::RANK_COUNT;
        return std::string({(char)(col + 'a'), (char)(row + '1')});
    }

    void print() {
        std::cout << this->stringify() << std::endl;
    }
    static bool validate(const std::string &strs) {
        return strs.length() == 2 &&
               (strs[0] >= 'a' && strs[0] <= 'h' && strs[1] >= '1' && strs[1] <= '8');
    }

  private:
    uint8_t square;
};

/**
 * @brief A square consists of a piece and a color. It does not have any information about its own
 * position on the board
 *
 */
class SquareInfo {
  public:
    Color color;
    Piece piece;
    SquareInfo() : color(Color::NO_COLOR), piece(Piece::NO_PIECE) {}
    SquareInfo(Color colorVal, Piece pieceVal) : color(colorVal), piece(pieceVal) {}
    SquareInfo(char pieceAsChar) : SquareInfo((pieceAsChar >= 'a' && pieceAsChar <= 'z') ? Color::BLACK : Color::WHITE, Piece::NO_PIECE) {
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
            fce_error("Invalid input when converting character to SquareInfo", 1);
        }
    }
};

typedef std::array<SquareInfo, Square::SQUARE_COUNT> SquareInfoBoard;