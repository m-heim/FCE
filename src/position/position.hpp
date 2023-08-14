#pragma once
#include "bitboard.hpp"
#include "core.hpp"
#include <array>
#include <vector>
#include <cstdint>
#include <iostream>

#define FILES 8
#define RANKS 8
#define BOARD_SIZE 64

typedef std::uint8_t square_index;
typedef std::pair<square_index, square_index> move;
typedef uint8_t relative_move;
constexpr square_index no_square_index = 255;

class Square {
public:
  Color color;
  Piece piece;
  Square();
};

class Position {
public:
  std::array<Square, 64> board;
  std::array<std::array<Bitboard, Piece::PIECE_COUNT>, Color::COLOR_COUNT> bitboards;


  void set_square(uint8_t square, Color color, Piece piece);
  static std::uint8_t square_to_index(std::uint8_t rank, std::uint8_t file);
  std::string stringify_board();
  Color to_move;
  square_index en_passant;
  uint16_t plies;
  uint16_t plies_since_capture;
  std::array<std::array<bool, Castle::CASTLE_COUNT>, Color::COLOR_COUNT>
      castle_rights;
  double evaluate(void);
  std::vector<move> generateMoves(void);
  std::vector<move> generarePawnMoves(void);
};
