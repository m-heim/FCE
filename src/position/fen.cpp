#include "fen.hpp"
#include "core.hpp"
#include "position.hpp"
#include <cstring>

Position parse_fen(const std::string &fen) {

    int start = 0;
    size_t next = fen.find(' ', start);
    std::string board = fen.substr(start, next - start);
    start = next + 1;
    next = fen.find(' ', start);
    std::string side = fen.substr(start, next - start);
    start = next + 1;
    next = fen.find(' ', start);
    std::string castling = fen.substr(start, next - start);
    start = next + 1;
    next = fen.find(' ', start);
    std::string en_passant = fen.substr(start, next - start);
    start = next + 1;
    next = fen.find(' ', start);
    std::string move_since_capture = fen.substr(start, next - start);
    start = next + 1;
    next = fen.find('\0', start);
    std::string move = fen.substr(start, next - start);

    Position position{};

    uint8_t row = 7;
    uint8_t col = 0;
    for (char curr : board) {
        if (curr == '/') {
            row -= 1;
            col = 0;
        } else if (curr >= '0' && curr <= '9') {
            int moveCols = curr - '0';
            col += moveCols;
        } else {
            SquareInfo square = charToSquareInfo(curr);
            position.setSquare((row << 3) + col, square.color, square.piece);
            col++;
        }
    }
    if (side == "w") {
        position.to_move = Color::WHITE;
        position.opponent = Color::BLACK;
    } else if (side == "b") {
        position.to_move = Color::BLACK;
        position.opponent = Color::WHITE;
    } else {
        fce_error("Couldn\'t parse side to move in fen", 1);
    }

    for (auto curr : castling) {
        if (curr == 'K') {
            position.castle_rights[Color::WHITE][Castle::KINGSIDE] = true;
        } else if (curr == 'k') {
            position.castle_rights[Color::BLACK][Castle::KINGSIDE] = true;
        } else if (curr == 'q') {
            position.castle_rights[Color::BLACK][Castle::QUEENSIDE] = true;
        } else if (curr == 'Q') {
            position.castle_rights[Color::WHITE][Castle::QUEENSIDE] = true;
        } else if (curr == '-') {
            position.castle_rights[Color::BLACK][Castle::QUEENSIDE] = false;
            position.castle_rights[Color::BLACK][Castle::KINGSIDE] = false;
            position.castle_rights[Color::WHITE][Castle::QUEENSIDE] = false;
            position.castle_rights[Color::WHITE][Castle::KINGSIDE] = false;
            break;
        } else {
            fce_error("Could\'t read fen castling", 1);
        }
    }

    if (en_passant[0] == '-') {
        position.en_passant = SQUARE_NONE;
    } else if (en_passant[0] >= 'a' && en_passant[0] <= 'h' && en_passant[1] >= '1' &&
               en_passant[1] <= '8') {
        position.en_passant = (SquareIndex)(en_passant[0] - 'a') + (en_passant[1] - '1') * 8;
    } else {
        fce_error("Couldn\'t read fen enpassant", 1);
    }
    position.plies_since_capture = std::stoi(move_since_capture);
    position.plies = std::stoi(move);
    return position;
}