#pragma once
#include "eval.hpp"
#include <bitboard.hpp>
#include <chess.hpp>
#include <cstdint>

constexpr uint64_t TRANSPOSITION_TABLE_N_PRESET = 1024 * 1024 * 35; // transposition table entries

typedef enum : uint8_t {
    EXACT = 0,
    LOWERBOUND = 1,
    UPPERBOUND = 2
} Bound;

/*
 * @brief Structure to store information about a position in the transposition table
 */
class PositionInfo {
  public:
    Bitboard hash;
    uint8_t depth;
    Evaluation eval;
    Bound bound;
    PositionInfo() : depth(0) {
    }
    PositionInfo(Bitboard hashVal, uint8_t depthVal, Evaluation evalVal, Bound boundVal)
        : hash(hashVal), depth(depthVal), eval(evalVal), bound(boundVal) {
    }
};

/**
 * @brief data structure that maps hash modulus table size to PositionInfo for fast access to
 * position evaluation
 *
 */
class TranspositionTable {
  public:
    TranspositionTable() : TranspositionTable(0) {
    }
    TranspositionTable(size_t size) : mSize(size) {
        init();
    }
    ~TranspositionTable() {
        if (transpositionTable != nullptr) {
            delete[] transpositionTable;
        }
    }
    PositionInfo *transpositionTable = nullptr; // pointer to the transposition table array
    void setSize(size_t size) {
        mSize = size;
    }
    void init() {
        if (transpositionTable == nullptr && mSize > 0) {
            transpositionTable = new PositionInfo[mSize]();
        } else {
            for (uint64_t index = 0; index < mSize; index++) {
                transpositionTable[index] = PositionInfo();
            }
        }
    }

    PositionInfo get(Bitboard hash) {
        return transpositionTable[hash % mSize];
    }

    void insert(PositionInfo position) {
        transpositionTable[position.hash % mSize] = position;
    }

  private:
    size_t mSize;
};

/*
 @link https://www.chessprogramming.org/Zobrist_Hashing
 @brief Allows to hash a position into a 64 Bit number for transposition purposes
 */

class Zobrist {
  public:
    Zobrist() {
        init();
    }
    static constexpr uint64_t ZOBRIST_SEED =
        0x1234123412341234ULL; // choose a random seed, should not matter what is chosen here as
                               // long as it is constant

    std::array<std::array<std::array<Bitboard, Square::SQUARE_COUNT>, Piece::KING + 1>,
               Color::BLACK + 1>
        zobristKeys;
    Bitboard zobristSide;
    std::array<Bitboard, File::FILE_COUNT> zobristEnPassant;
    std::array<std::array<std::array<Bitboard, 2>, Castle::CASTLE_COUNT>, Color::BLACK + 1>
        zobristCastle;
    void init();
};