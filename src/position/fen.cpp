#include "fen.hpp"
#include "position.hpp"
#include "util.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <square.hpp>

/*
 @brief Parse a fen from a string, exits on error
*/
Position parseFen(const std::string &fen) {
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

    uint8_t row = Rank::RANK_8;
    uint8_t col = File::FILE_A;
    for (char curr : board) {
        if (curr == '/') {
            row -= 1;
            col = 0;
        } else if (curr >= '0' && curr <= '9') {
            int moveCols = curr - '0';
            col += moveCols;
        } else {
            auto square = SquareInfo(curr);
            position.setSquare((row * Square::SQUARE_A2) + col, square.color, square.piece);
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

    if (en_passant.at(0) == '-') {
        position.en_passant = SquareIndex::Squares::SQUARE_NONE;
    } else if (SquareIndex::validate(en_passant)) {
        position.en_passant = SquareIndex(en_passant);
    } else {
        fce_error("Couldn\'t read fen enpassant", 1);
    }
    position.plies_since_capture = std::stoi(move_since_capture);
    position.plies = std::stoi(move);
    position.hash = position.computeHash();
    return position;
}

/*
 @brief read a fen from a file, does not check for validity
*/
std::string readFen(const std::string &fenPath) {
    std::ifstream fen_file_stream(fenPath, std::ios::in);
    if (!fen_file_stream.is_open()) {
        fce_error("Failed to open fen", 1);
    }
    fen_file_stream.seekg(0, std::ios_base::end);
    auto fen_size = fen_file_stream.tellg();
    std::string fen(fen_size, '\0');
    fen_file_stream.seekg(0);
    fen_file_stream.read(fen.data(), fen_size);
    fen_file_stream.close();
    return fen;
}