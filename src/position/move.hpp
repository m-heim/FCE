#pragma once
#include "magic.hpp"
#include "square.hpp"
#include <array>
#include <bitboard.hpp>
#include <cstdint>

constexpr uint64_t MOVE_LIMIT_N = 256;
constexpr uint16_t no_move = 65;

enum MoveFlags : std::uint8_t {
    QUIET,
    CAPTURE,
    DOUBLE_PUSH,
    EN_PASSANT,
    CASTLE_QUEENSIDE,
    CASTLE_KINGSIDE,
    PROMOTE_QUEEN = 8,
    PROMOTE_ROOK = 9,
    PROMOTE_BISHOP = 10,
    PROMOTE_KNIGHT = 11,
    MASK = 0x0F
};

// typedefs
class Move {
  private:
    std::uint16_t move;

  public:
    Move() {
    }
    Move(int moveVal) {
        move = moveVal;
    }
    Move(SquareIndex from, SquareIndex to, MoveFlags flags) {
        move = (from << 6) | (flags << 12) | to;
    }
    operator int() const {
        return move;
    }
    SquareIndex getFrom() {
        return (move >> 6) & Square::SQUARE_H8;
    }
    SquareIndex getTo() {
        return move & Square::SQUARE_H8;
    }

    MoveFlags getFlags() {
        return static_cast<MoveFlags>(MoveFlags::MASK & (move >> 12));
    }
};

class MoveList {
  public:
    uint8_t count;
    std::array<Move, MOVE_LIMIT_N> moves{};
    MoveList() {
        count = 0;
    };
    Move get(uint8_t index) {
        return moves[index];
    }
    void push_back(Move move) {
        moves[count] = move;
        count++;
    }
    void addMoves(SquareIndex from, Bitboard board, MoveFlags flags) {
        while (board) {
            SquareIndex to = get_ls1b_index(board);
            push_back(Move(from, to, flags));
            board &= unmaskedSquare[to];
        }
    }
    std::string stringify() {
        std::string result;
        for (uint8_t i = 0; i < count; i++) {
            std::string fromSquare = moves[i].getFrom().stringify();
            std::string toSquare = moves[i].getTo().stringify();
            MoveFlags flag = moves[i].getFlags();
            result.append(std::to_string(i) + " " + fromSquare + " " + toSquare + " " +
                          std::to_string(flag) + "\n");
        }
        return result;
    }
};