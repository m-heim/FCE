#include <fen.hpp>
#include <fstream>
#include <iostream>
#include <position.hpp>
#include <bitboard.hpp>
#include <util.hpp>
#include <vector>

int main(int argc, char **argv) {
  std::cout << "Fce is starting now ..." << std::endl;
  if (argc <= 1) {
    fce_error("Please provide a fen", 2);
  }
  std::string fen_file = std::string(argv[1]);
  std::ifstream fen_file_stream(fen_file, std::ios::in);
  if (!fen_file_stream.is_open()) {
    fce_error("Failed to open fen", 1);
  }
  fen_file_stream.seekg(0, std::ios_base::end);
  auto fen_size = fen_file_stream.tellg();
  std::string fen(fen_size, '\0');
  fen_file_stream.seekg(0);
  fen_file_stream.read(&fen[0], fen_size);
  fen_file_stream.close();

  Position position = parse_fen(fen);

  std::cout << position.stringify_board() << std::endl;

  MagicBitboards magics{};
  //printBitboard(position.bitboards[Color::WHITE][Piece::PAWN]);
  //printBitboard(position.occupation[Color::BLACK]);
  //printBitboard(magics.pawnAttacks[Color::WHITE][Square::SQUARE_A1]);

  std::vector<move> moves = position.generateMoves(magics);
  //position.makeMove(moves.at(0));
  std::cout << "found" << moves.size() << std::endl;
  std::cout << position.stringify_board() << std::endl;
  return 0;
}