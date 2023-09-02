#include "transposition.hpp"

std::array<PositionInfo, TRANSPOSITION_TABLE_N> transpositionTable;

void initTranspositionTable() {
    for (uint64_t index = 0; index < TRANSPOSITION_TABLE_N; index++) {
        transpositionTable.at(index) = PositionInfo();
    }
}

PositionInfo getPositionInfo(Bitboard hash) {
    return transpositionTable[hash % TRANSPOSITION_TABLE_N];
}

void insertPositionInfo(PositionInfo position) {
    transpositionTable[position.hash % TRANSPOSITION_TABLE_N] = position;
}