#pragma once
#include "eval.hpp"
#include "hash.hpp"
#include "move.hpp"
#include <bitboard.hpp>
#include <chess.hpp>
#include <cstdint>
#include <cstring>
#include <square.hpp>

// typedefs
typedef std::pair<Move, Evaluation> SearchInfo;

// constants
constexpr uint16_t SEARCH_DEPTH_N = 256;
constexpr uint16_t QUIESCE_DEPTH_N = 100;

class Position {
  public:
    Position();
    Position(TranspositionTable *tt, Zobrist *z);
    ~Position() {
    }
    SquareInfoBoard board; // array of all squares on the board, each square has a piece and a color
    std::array<PiecesBitboards, Color::COLOR_COUNT_VALID + 1>
        bitboards; // bitboards[color][piece] gives a bitboard of all pieces of the given color and
                   // type
    std::array<Bitboard, Color::COLOR_COUNT_VALID + 1>
        occupation; // occupation[color] gives a bitboard of all pieces of the given color
    std::array<CastleRights, Castle::CASTLE_COUNT_VALID + 1> castle_rights;
    Color playerColor;         // side to move
    Color opponentPlayerColor; // opponent of side to move
    SquareIndex en_passant;    // if en passant is possible, this is the square that can be captured
                               // on, otherwise it is SQUARE_NONE
    SquareIndex
        previous_en_passant; // the en passant square before the last move, used for unmaking moves
    SquareInfo previousSquareInfo; // the piece and color of the square before the last move, used
                                   // for unmaking moves
    uint16_t previousPliesSinceCapture; // the number of plies since the last capture before the
                                        // last move, used for unmaking moves
    uint16_t plies;                     // how many plies have been played in the game so far
    uint16_t plies_since_capture; // how many plies have been played since the last capture, used
                                  // for 50 move rule
    Bitboard hash;                // zobrist hash of the position, used for transposition table
    void setSquare(SquareIndex square, Color color,
                   Piece piece); // sets the square to the given piece and color, also updates
                                 // bitboards and occupation
    std::string
    stringify_board();         // returns a string representation of the board, used for debugging
    void print_board();        // prints the board to the console, used for debugging
    Evaluation evaluate(void); // evaluates the position
    Evaluation evaluateMaterial(void); // evaluates the material balance of the position
    Evaluation evaluatePosition(
        void); // evaluates the position based on piece placement and other factors, not material
    TranspositionTable
        *transpositionTable; // transposition table for storing previously evaluated positions
    Zobrist *zobrist;        // zobrist hashing for fast position hashing
    void makeMove(Move m,
                  UndoInfo &undoInfo);  // makes the move on the position, also updates bitboards,
                                        // occupation, castling rights, en passant square and hash
    void unmakeMove(UndoInfo undoInfo); // unmakes the move on the position, also updates bitboards,
                                        // occupation, castling rights, en passant square and hash
    bool kingExists();
    void generatePawnMoves(MoveList &moves);
    void generatePieceMoves(MoveList &moves, MoveList &captures);
    void generateMoves(MoveList &moves, MoveList &captures);
    bool inCheck(Color side);
    Bitboard computeHash();
};