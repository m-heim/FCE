#include <bitboard.hpp>
#include <bits/getopt_core.h>
#include <fen.hpp>
#include <fstream>
#include <iostream>
#include <position.hpp>
#include <vector>

int main(int argc, char **argv) {
  std::cout << "Fce is starting now ..." << std::endl;
  char option;
  uint8_t depth = 0;
  std::string fen_file;
  while ((option = getopt(argc, argv, "d:f:")) != -1) {
    switch (option) {
    case 'd':
      depth = std::atoi(optarg);
      break;
    case 'f':
      fen_file.append(optarg);
      break;
    default:
      exit(2);
    }
  }
  std::ifstream fen_file_stream(fen_file, std::ios::in);
  if (!fen_file_stream.is_open()) {
    fce_error("Failed to open fen", 1);
  }
  fen_file_stream.seekg(0, std::ios_base::end);
  auto fen_size = fen_file_stream.tellg();
  std::string fen(fen_size, '\0');
  fen_file_stream.seekg(0);
  fen_file_stream.read(fen.data(), fen_size);
  fen_file_stream.close();

  initGlobals();

  Position position = parse_fen(fen);

  std::vector<Move> moves = position.generateMoves();

  std::cout << "Found" << std::to_string(moves.size()) << std::endl;

  std::cout << "Position\n" << position.stringify_board() << std::endl;

  SearchInfo bestMove = search(&position, depth);
  std::cout << "BestMove" << squareStringify(moveGetFrom(bestMove.first))
            << squareStringify(moveGetTo(bestMove.first)) << std::endl
            << "Eval" << std::to_string(bestMove.second) << std::endl;
  ;

  // printBitboard(bishopMagics[Square::SQUARE_D3].getAttack(fileB));
  return 0;
}