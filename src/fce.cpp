#include <bitboard.hpp>
#include <bits/getopt_core.h>
#include <fen.hpp>
#include <fstream>
#include <iostream>
#include <position.hpp>
#include <util.hpp>
#include <vector>

int main(int argc, char **argv) {
  std::cout << "Fce is starting now ..." << std::endl;
  char c;
  uint8_t depth = 0;
  std::string fen_file = "";
  while ((c = getopt(argc, argv, "d:f:")) != -1) {
    switch (c) {
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
  fen_file_stream.read(&fen[0], fen_size);
  fen_file_stream.close();

  initMagics();

  Position position = parse_fen(fen);

  std::vector<Move> moves = position.generateMoves();
  std::cout << "found" << moves.size() << std::endl;

  std::cout << "Position\n" << position.stringify_board() << std::endl;

  //Move bestMove = negaMaxRoot(position, 6, magics);
  //SquareIndex from = moveGetFrom(bestMove);
  //SquareIndex to = moveGetTo(bestMove);
  //MoveFlags flags = moveGetFlags(bestMove);
  //std::cout << "Best Move" << "\n" << std::to_string(from) << "\n" << std::to_string(to) << std::endl;;

  SearchInfo bestMove = search(&position, depth);
  std::cout << "BestMove" << squareStringify(moveGetFrom(bestMove.first)) << squareStringify(moveGetTo(bestMove.first)) <<std::endl << "Eval" << std::to_string(bestMove.second) << std::endl;;
  return 0;
}