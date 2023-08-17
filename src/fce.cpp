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

  Position position = parse_fen(fen);

  std::cout << "Position" << position.stringify_board() << std::endl;

  MagicBitboards magics{};
  //move bestMove = negaMaxRoot(position, 6, magics);
  //SquareIndex from = moveGetFrom(bestMove);
  //SquareIndex to = moveGetTo(bestMove);
  //MoveFlags flags = moveGetFlags(bestMove);
  //std::cout << "Best move" << "\n" << std::to_string(from) << "\n" << std::to_string(to) << std::endl;;

  Evaluation eval = alphaBeta(&position, EvaluationLiterals::NEG_INF, EvaluationLiterals::POS_INF, depth, magics, true);
  std::cout << "Eval" << eval << std::endl;;

  std::vector<move> moves = position.generateMoves(magics);
  std::cout << "found" << moves.size() << std::endl;
  return 0;
}