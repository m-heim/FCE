#pragma once
#include "position.hpp"
#include <array>
#include <bitboard.hpp>
#include <chess.hpp>
#include <cstdint>

constexpr uint64_t TRANSPOSITION_TABLE_N = 1024 * 1024 * 128; // transposition table entries

class PositionInfo {
  public:
    Bitboard hash;
    Evaluation eval;
    uint8_t depth;
    uint8_t type;
    PositionInfo() {
        depth = 0;
    }
    PositionInfo(Bitboard hashVal, uint8_t depthVal, Evaluation evalVal, uint8_t typeVal) {
        hash = hashVal;
        depth = depthVal;
        eval = evalVal;
        type = typeVal;
    }
};

/**
 * @brief data structure that maps hash modulus table size to PositionInfo for fast access to
 * position evaluation
 *
 */
class TranspositionTable {
  public:
    static std::array<PositionInfo, TRANSPOSITION_TABLE_N> transpositionTable;
    static void init() {
        for (uint64_t index = 0; index < TRANSPOSITION_TABLE_N; index++) {
            transpositionTable[index] = PositionInfo();
        }
    }

    static PositionInfo get(Bitboard hash) {
        return transpositionTable[hash % TRANSPOSITION_TABLE_N];
    }

    static void insert(PositionInfo position) {
        transpositionTable[position.hash % TRANSPOSITION_TABLE_N] = position;
    }
};