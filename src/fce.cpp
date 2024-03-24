#include <bitboard.hpp>
#include <bits/getopt_core.h>
#include <chess.hpp>
#include <fen.hpp>
#include <fstream>
#include <iostream>
#include <logger.hpp>
#include <position.hpp>
#include <transposition.hpp>
#include <zobrist.hpp>

int main(int argc, char **argv) {
    fce_log(LOG_LEVEL::DEBUG, "Fce is starting now ...");
    initGlobals();
    fce_log(LOG_LEVEL::DEBUG, "Initialized precomputed patterns.");
    initZobrist(ZOBRIST_SEED);
    fce_log(LOG_LEVEL::DEBUG, "Initialized zobrist coefficients.");
    TranspositionTable::init();
    fce_log(LOG_LEVEL::DEBUG, "Initialized transposition table.");
    char option = 'h';
    uint8_t depth = 0;
    std::string fen_file;
    while ((option = (char)getopt(argc, argv, "d:f:h")) != -1) {
        switch (option) {
        case 'd':
            depth = std::atoi(optarg);
            break;
        case 'f':
            fen_file.append(optarg);
            break;
        case 'h':
            fce_log(LOG_LEVEL::REGULAR, "fce -f [file] -d [depth]");
            break;
        default:
            fce_error("Please provide an argument -h for more info", 2);
            break;
        }
    }
    std::string fen = readFen(fen_file);

    Position position = parseFen(fen);

    std::cout << "Position\n" << position.stringify_board() << std::endl;

    while (true) {
        std::string input;
        fce_log(LOG_LEVEL::REGULAR, "s - search, m - make a move, d - debug, l - eval, e - exit");
        std::cin >> input;
        if (input == "s") {
            int innerDepth = 0;
            std::string bufDepth;
            std::cout << "depth" << std::endl;
            std::cin >> bufDepth;
            innerDepth = std::atoi(bufDepth.c_str());
            SearchInfo bestMove = search(&position, innerDepth);
            std::cout << "BestMove" << bestMove.first.getFrom().stringify()
                      << bestMove.first.getTo().stringify() << std::endl
                      << "Eval" << std::to_string(bestMove.second) << std::endl;
            TranspositionTable::init();
        } else if (input == "m") {
            std::string buf;
            std::cin >> buf;
        } else if (input == "e") {
            exit(0);
        } else if (input == "l") {
            std::string buf;
            std::cout << "Depth:" << std::endl;
            std::cin >> buf;
            depth = std::atoi(buf.c_str());
            Evaluation eval = alphaBeta(&position, EvaluationLiterals::NEG_INF,
                                        EvaluationLiterals::POS_INF, depth);
            std::cout << std::to_string(eval) << std::endl;
        } else if (input == "d") {
            std::cout << "n - knight attacks, m - moves, b - bishop magics, r - rook magics"
                      << std::endl;
            std::string buf;
            std::cin >> buf;
            if (buf == "n") {
                printKnightAttacks();
            } else if (buf == "m") {
                MoveList moves_debug;
                position.generateMoves(moves_debug);
                std::cout << moves_debug.stringify() << std::endl;

            } else if (buf == "b") {
                std::string squareBuf;
                std::cout << "Square" << std::endl;
                std::cin >> squareBuf;
                std::cout << squareBuf << std::endl;
                SquareIndex index(squareBuf);
                std::array<Bitboard, BITBOARD_SUBSETS_N> subsets =
                    getBitboardSubsets(bishopMasks.at(index));
                for (int i = 0; i < bitboardGetHW(bishopMasks.at(index)); i++) {
                    printBitboard(subsets.at(i));
                    printBitboard(bishopMagics.at(index).getAttack(subsets.at(i)));
                }
            } else if (buf == "r") {
                std::string squareBuf;
                std::cout << "Square" << std::endl;
                std::cin >> squareBuf;
                std::cout << squareBuf << std::endl;
                SquareIndex index(squareBuf);
                std::array<Bitboard, BITBOARD_SUBSETS_N> subsets =
                    getBitboardSubsets(rookMasks.at(index));
                for (int i = 0; i < bitboardGetHW(rookMasks.at(index)); i++) {
                    printBitboard(subsets.at(i));
                    printBitboard(rookMagics.at(index).getAttack(subsets.at(i)));
                }
            }
        } else {
            exit(1);
        }
    }
    // printRays(Square::SQUARE_F2);
    return 0;
}