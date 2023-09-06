#include "transposition.hpp"

std::array<PositionInfo, TRANSPOSITION_TABLE_N> TranspositionTable::transpositionTable{
    PositionInfo()};