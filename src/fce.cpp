#include <bitboard.hpp>
#include <bits/getopt_core.h>
#include <chess.hpp>
#include <fen.hpp>
#include <fstream>
#include <iostream>
#include <position.hpp>
#include <vector>

int main(int argc, char **argv) {
    std::cout << "Fce is starting now ..." << std::endl;
    char option = 'h';
    uint8_t depth = 0;
    std::string fen_file;
    while ((option = getopt(argc, argv, "d:f:h")) != -1) {
        switch (option) {
        case 'd':
            depth = std::atoi(optarg);
            break;
        case 'f':
            fen_file.append(optarg);
            break;
        case 'h':
            std::cout << "fce -f [file] -d [depth]" << std::endl;
            break;
        default:
            std::cerr << "Please provide an argument -h for more info" << std::endl;
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
    Position position = parseFen(fen);

    MoveList moves;
    position.generateMoves(moves);

    std::cout << "Found" << std::to_string(moves.count) << std::endl;

    std::cout << "Position\n" << position.stringify_board() << std::endl;

    // SearchInfo bestMove = search(&position, depth);
    // std::cout << "BestMove" << squareStringify(moveGetFrom(bestMove.first))
    //<< squareStringify(moveGetTo(bestMove.first)) << std::endl
    //<< "Eval" << std::to_string(bestMove.second) << std::endl;
    Evaluation eval =
        alphaBeta(&position, EvaluationLiterals::NEG_INF, EvaluationLiterals::POS_INF, depth);
    std::cout << std::to_string(eval) << std::endl;
    ;
    while (true) {
        std::string input;
        std::cout << "s - search, m - make a move, d - debug, e - exit" << std::endl;
        std::cin >> input;
        if (input == "s") {
            int innerDepth = 0;
            std::string bufDepth;
            std::cout << "depth" << std::endl;
            std::cin >> bufDepth;
            innerDepth = std::atoi(bufDepth.c_str());
            SearchInfo bestMove = search(&position, innerDepth);
            std::cout << "BestMove" << squareIndexStringify(moveGetFrom(bestMove.first))
                      << squareIndexStringify(moveGetTo(bestMove.first)) << std::endl
                      << "Eval" << std::to_string(bestMove.second) << std::endl;
        } else if (input == "m") {
            std::string buf;
            std::cin >> buf;
        } else if (input == "e") {
            exit(0);
        } else if (input == "d") {
            std::cout << "n - knight attacks" << std::endl;
            std::string buf;
            std::cin >> buf;
            if (buf == "n") {
                printKnightAttacks();
            } else if (buf == "b") {
                std::string squareBuf;
                std::cout << "Square" << std::endl;
                std::cin >> squareBuf;
                std::cout << squareBuf << std::endl;
                SquareIndex index = stringToSquareIndex(squareBuf);
                printBitboard(bishopMagics.at(index).getAttack(fileC | rank2 |
                                                               maskedSquare[Square::SQUARE_D1]));
            }
        } else {
            exit(1);
        }
    }
    // printRays(Square::SQUARE_F2);
    return 0;
}