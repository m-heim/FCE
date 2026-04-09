#include "move.hpp"

std::string MoveList::stringify() {
    std::string result;
    for (uint8_t i = 0; i < count; i++) {
        Move m = moves[i];
        result += m.stringify() + "\n";
    }
    return result;
}