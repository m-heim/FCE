#pragma once
#include "bitboard.hpp"
#include "chess.hpp"
#include "position.hpp"
#include <array>
#include <cstdint>

constexpr uint64_t TRANSPOSITION_TABLE_N = 1024 * 1024 * 128;

class PositionInfo {
  public:
    bool valid;
    Bitboard hash;
    uint8_t depth;
    Evaluation eval;
    uint8_t type;
    PositionInfo() {
        valid = false;
        depth = 0;
    };
    PositionInfo(Bitboard hashVal, uint8_t depthVal, Evaluation evalVal, uint8_t typeVal) {
        valid = true;
        hash = hashVal;
        depth = depthVal;
        eval = evalVal;
        type = typeVal;
    }
};

extern std::array<PositionInfo, TRANSPOSITION_TABLE_N> transpositionTable;

inline PositionInfo getPositionInfo(Bitboard hash) {
    return transpositionTable[hash % TRANSPOSITION_TABLE_N];
}

inline void insertPositionInfo(PositionInfo position) {
    transpositionTable[position.hash % TRANSPOSITION_TABLE_N] = position;
}