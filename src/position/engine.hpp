#pragma once
#include "position.hpp"
class EngineInstance {
  public:
    EngineInstance() {
    }
    ~EngineInstance() {
    }
    void init();
    std::array<std::uint64_t, SEARCH_DEPTH_N> positionsEvaluated{0ULL};
    Zobrist z = Zobrist();
    TranspositionTable transpositionTable = TranspositionTable(TRANSPOSITION_TABLE_N_PRESET);
    SearchInfo search(const std::string &fenFile, uint16_t depth);
    SearchInfo search(Position &position, uint16_t depth, const Move &moveHint = no_move);
    SearchInfo iterativeDeepeningSearch(const std::string &fenFile, uint16_t depth);
    Evaluation alphaBeta(Position &position, Evaluation alpha, Evaluation beta, uint16_t depth,
                         int pvInd);
    Evaluation quiesce(Position &position, Evaluation alpha, Evaluation beta, uint8_t depth);
    void printPlyPositions(void);
};