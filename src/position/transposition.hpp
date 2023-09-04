#pragma once
#include "position.hpp"
#include <array>
#include <bitboard.hpp>
#include <chess.hpp>
#include <cstdint>

constexpr uint64_t TRANSPOSITION_TABLE_N = 1024 * 1024 * 256;

class PositionInfo {
  public:
    Bitboard hash;
    Evaluation eval;
    uint8_t depth;
    PositionInfo() {
        depth = 0;
    };
    PositionInfo(Bitboard hashVal, uint8_t depthVal, Evaluation evalVal, uint8_t typeVal) {
        hash = hashVal;
        depth = depthVal;
        eval = evalVal;
    }
};

extern std::array<PositionInfo, TRANSPOSITION_TABLE_N> transpositionTable;

void initTranspositionTable();

PositionInfo getPositionInfo(Bitboard hash);

void insertPositionInfo(PositionInfo position);