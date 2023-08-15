#include <fen.hpp>
#include <fstream>
#include <iostream>
#include <position.hpp>
#include <bitboard.hpp>
#include <util.hpp>

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
  Bitboard test;
  Bitboard test_init = magics.knightAttacks[Square::SQUARE_D4];
  // std::cout << test_init << std::endl;
  return 0;
}