#include "engine.hpp"
#include "eval.hpp"
#include "fen.hpp"
#include "hash.hpp"
#include "move.hpp"
#include "position.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <util.hpp>

bool cmpPos(Position &pos1, Position &pos2, Move m) {
    for (int i = 0; i < Square::SQUARE_COUNT; i++) {
        if (pos1.board[i].color != pos2.board[i].color ||
            pos1.board[i].piece != pos2.board[i].piece) {
            std::cout << "Position changed after unmake move" << std::endl;
            std::cout << m.stringify() << std::endl;
            pos1.print_board();
            pos2.print_board();
            exit(1);
            return 0;
        }
    }
    return 1;
}

void EngineInstance::printPlyPositions() {
    int firstPos = -1;
    for (int i = 0; i < SEARCH_DEPTH_N; i++) {
        if (positionsEvaluated[i]) {
            if (firstPos == -1) {
                firstPos = i;
            }
            std::cout << "Depth" << std::to_string(i - firstPos) << " "
                      << std::to_string(positionsEvaluated[i]) << std::endl;
        }
    }
}

Stack<Bitboard> hashStack;
Stack<Move> moveStack;
size_t len;
/* pvArray:
/  M1 M2 M3 M4 M5
/  M1 M2 M3 M4
/  M1 M2 M3
/  M1 M2 -> node above copies pv if alpha > score
/  M1 -> deepest node sets pvSubindex[0]
*/
std::array<Move, (MOVE_LIMIT_N * MOVE_LIMIT_N + MOVE_LIMIT_N) / 2> pvArray;
bool is3foldRepetition(Bitboard hash) {
    return hashStack.containsCount(hash) >= 2;
}

void movcpy(Move *pTarget, const Move *pSource, int n) {
    while (n-- && (*pTarget++ = *pSource++))
        ;
}

SearchInfo EngineInstance::iterativeDeepeningSearch(const std::string &fenFile, uint16_t depth) {
    Position position(&transpositionTable, &z);
    std::string fen = readFen(fenFile);
    len = depth;
    parseFen(fen, position);
    Move moveHint = no_move;
    SearchInfo bestMove = SearchInfo(no_move, EvaluationLiterals::NEG_INF);
    for (int i = 1; i <= depth; i++) {
        Position pos2 = position;
        bestMove = search(pos2, i, moveHint);
        moveHint = bestMove.first;
    }
    for (int pvMoveIndex = 0; pvMoveIndex < depth; pvMoveIndex++) {
        if (pvArray[pvMoveIndex] == no_move) {
            break;
        }
        UndoInfo info;
        position.makeMove(pvArray[pvMoveIndex], info);
        std::cout << "Ply: " + std::to_string(position.plies)
                  << ", Move: " << pvArray[pvMoveIndex].stringify() << std::endl;
        position.print_board();
    }
    return bestMove;
}

SearchInfo EngineInstance::search(const std::string &fenFile, uint16_t depth) {
    Position position(&transpositionTable, &z);
    std::string fen = readFen(fenFile);
    parseFen(fen, position);
    return search(position, depth);
}

SearchInfo EngineInstance::search(Position &position, uint16_t depth, const Move &moveHint) {
    auto start = std::chrono::high_resolution_clock::now();
    MoveList moves[2];
    Move bestMove = no_move;
    Evaluation alpha = EvaluationLiterals::NEG_INF;
    Evaluation beta = EvaluationLiterals::POS_INF;
    UndoInfo undoInfo;
    position.generateMoves(moves[0], moves[0]);
    positionsEvaluated.fill(0);
    hashStack.push(position.hash);
    pvArray[0] = no_move;
    size_t nextIndex = 0 + SEARCH_DEPTH_N - position.plies;
    if (moveHint != no_move && moveHint != moves[0].get(0)) {
        for (uint8_t i = 0; i < moves[0].count; i++) {
            if (moves[0].get(i) == moveHint) {
                moves[0].swap(0, i);
                break;
            }
        }
    }
    for (uint8_t index = 0; index < moves[0].count; index++) {
        positionsEvaluated[depth + QUIESCE_DEPTH_N] += 1;
        Move move = moves[0].get(index);
        std::cout << "Searching move" << move.stringify() << std::endl;
        position.makeMove(move, undoInfo);
        position.print_board();
        if (position.inCheck(position.opponentPlayerColor)) {
            // we left ourselves in check
            position.unmakeMove(undoInfo);
            continue;
        }

        Evaluation current = -alphaBeta(position, -beta, -alpha, depth - 1, nextIndex);
        position.unmakeMove(undoInfo);
        if (current > alpha) {
            alpha = current;
            bestMove = move;
            pvArray[0] = move;
            movcpy(&pvArray.at(1), &pvArray.at(nextIndex), SEARCH_DEPTH_N - position.plies - 1);
        }
    }
    hashStack.pop();
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(
                  std::chrono::high_resolution_clock::now() - start)
                  .count();
    printPlyPositions();
    size_t positions = std::accumulate(positionsEvaluated.begin(), positionsEvaluated.end(), 0);
    double rateMps = (double)positions / (double)us;
    std::cout << "PV: " << std::endl;
    for (size_t i = 0; i < depth; i++) {
        Move move = pvArray[i];
        std::cout << move.stringify() << std::endl;
    }
    std::cout << "Total of:\n"
              << std::to_string(positions) << "\nat:\n"
              << std::to_string(rateMps) << "MP/s" << std::endl;
    return {bestMove, alpha};
}

// alpha = the best score that the maximizing player can guarantee at this point or above
// beta = the best score that the minimizing player can guarantee at this point or above

Evaluation EngineInstance::alphaBeta(Position &position, Evaluation alpha, Evaluation beta,
                                     uint16_t depth, int pvInd) {
    // This search is based on the fact that if we can choose between multiple moves and one of
    // those moves yields in a better position than what our opponent can achieve we can ignore this
    // whole subtree
    hashStack.push(position.hash);
    Evaluation eval = position.evaluate();
    Bitboard key = position.hash;
    MoveList moves[2];
    uint8_t legalMoves;
    UndoInfo undoInfo;
    Position pos2 = position;
    pvArray[pvInd] = no_move;
    int nextInd = pvInd + SEARCH_DEPTH_N - position.plies;
    PositionInfo infoLookup = position.transpositionTable->get(key);
    bool razor = depth <= 3 && !position.inCheck(position.playerColor) &&
                 (eval + MARGIN >= beta); // fail high
    if (is3foldRepetition(position.hash)) {
        alpha = EvaluationLiterals::EVEN;
        goto alphaBetaE;
    }
    /*if (infoLookup.hash == key && (infoLookup.depth >= depth + QUIESCE_DEPTH_N) &&
        infoLookup.bound == Bound::EXACT) {
        // std::cout << std::to_string(infoLookup.hash) << std::endl;
        alpha = infoLookup.eval;
        goto alphaBetaE;
    }*/
    if (depth == 0 || razor) {
        alpha = quiesce(position, alpha, beta, QUIESCE_DEPTH_N);
        goto alphaBetaE;
    }
    if (depth >= 3 &&
        !position.inCheck(
            position.playerColor)) { // if we are at a high depth and not in check, do a null move
                                     // search to try to find a beta cutoff faster
        UndoInfo undoInfo2;
        position.makeMove(Move(0, 0, MoveFlags::NULL_MOVE), undoInfo2);
        eval = -alphaBeta(position, -beta, -beta + 1, depth - 2, nextInd);
        position.unmakeMove(undoInfo2);
        if (eval >= beta) {
            alpha = beta;
            goto alphaBetaE;
        }
    }
    position.generateMoves(moves[0], moves[1]);
    legalMoves = moves[0].count + moves[1].count;
    positionsEvaluated[depth + QUIESCE_DEPTH_N] += 1;
    for (int j = 1; j >= 0; j--) {
        for (uint8_t i = 0; i < moves[j].count; i++) {
            Move move = moves[j].get(i);
            position.makeMove(move, undoInfo);
            if (position.inCheck(position.opponentPlayerColor)) {
                // we left ourselves in check
                position.unmakeMove(undoInfo);
                legalMoves -= 1;
                continue;
            }
            eval = -alphaBeta(position, -beta, -alpha, depth - 1, nextInd);
            position.unmakeMove(undoInfo);
            assert(cmpPos(position, pos2, move));
            if (eval > alpha) {
                alpha = eval;
                pvArray[pvInd] = move;
                movcpy(&pvArray.at(pvInd + 1), &pvArray.at(nextInd),
                       SEARCH_DEPTH_N - position.plies - 1);
            }
            if (alpha >= beta) {
                position.transpositionTable->insert(
                    PositionInfo(position.hash, depth + QUIESCE_DEPTH_N, beta, Bound::LOWERBOUND));
                alpha = beta;
                goto alphaBetaE; // fail hard beta-cutoff
            }
        }
    }
    if (legalMoves == 0 && position.inCheck(position.playerColor)) {
        alpha = EvaluationLiterals::MATE + position.plies;
    } else if (legalMoves == 0) {
        alpha = EvaluationLiterals::EVEN;
    }
    position.transpositionTable->insert(PositionInfo(position.hash, depth + QUIESCE_DEPTH_N, alpha,
                                                     Bound::EXACT)); // fix this to be valid
alphaBetaE:
    hashStack.pop();
    return alpha;
}

Evaluation EngineInstance::quiesce(Position &position, Evaluation alpha, Evaluation beta,
                                   uint8_t depth) {
    positionsEvaluated.at(depth) += 1;
    hashStack.push(position.hash);
    Evaluation eval = position.evaluate();
    UndoInfo undoInfo;
    MoveList moves[2]; // 0 = quiet moves, 1 = captures
    if (is3foldRepetition(position.hash)) {
        alpha = EvaluationLiterals::EVEN;
        goto quiesceE;
    }
    if (depth == 0) {
        alpha = eval;
        goto quiesceE;
    }
    if (alpha < eval) {
        alpha = eval;
    }
    if (alpha >= beta) {
        alpha = beta;
        goto quiesceE; // fail hard beta-cutoff
    }
    position.generateMoves(moves[0], moves[1]);
    for (uint8_t index = 0; index < moves[1].count; index++) {
        Move move = moves[1].get(index);
        if (position.inCheck(position.playerColor)) {
            position.makeMove(move, undoInfo);
            if (position.inCheck(position.opponentPlayerColor)) {
                position.unmakeMove(undoInfo);
                continue;
            }
            Evaluation score = -quiesce(position, -beta, -alpha, depth - 1);
            position.unmakeMove(undoInfo);
            if (score > alpha) {
                alpha = score;
            }
            if (alpha >= beta) {
                alpha = beta;
                goto quiesceE; // fail hard beta-cutoff
            }
        }
    }
quiesceE:
    hashStack.pop();
    return alpha;
}