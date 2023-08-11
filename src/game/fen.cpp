#include "fen.hpp"
#include "board.hpp"
#include "util.hpp"
#include <string.h>

Game parse_fen(std::string fen) {

    int start = 0;
    int next = fen.find(' ', start);
    std::string position = fen.substr(start, next-start);
    start = next + 1;
    next = fen.find(' ', start);
    std::string side = fen.substr(start, next-start);
    start = next + 1;
    next = fen.find(' ', start);
    std::string castling = fen.substr(start, next-start);
    start = next + 1;
    next = fen.find(' ', start);
    std::string en_passant = fen.substr(start, next-start);
    start = next + 1;
    next = fen.find(' ', start);
    std::string move_since_capture = fen.substr(start, next-start);
    start = next + 1;
    next = fen.find('\0', start);
    std::string move = fen.substr(start, next-start);

    Game game;
    int row = 0;
    int col = 0;

    for(int i = 0; i < RANKS; i++) {
        for (int j = 0; j < FILES; j++) {
            game.board.set_square(Board::square_to_index(i, j), Color::NO_COLOR, Piece::NO_PIECE);
        }
    }
    for (char c : position) {
        if (c == '/') {
            row++;
            col = 0;
            continue;
        }
        if (c >= '0' && c <= '9') {
            int n = c - '1';
            col += n;
            continue;
        } else {
            Piece piece;
            if (c == 'r' || c == 'R') {
                piece = ROOK;
            } else if (c == 'b' || c == 'B') {
                piece = BISHOP;
            } else if (c == 'k' || c == 'K') {
                piece = KING;
            } else if (c == 'n' || c == 'N') {
                piece = KNIGHT;
            } else if (c == 'Q' || c == 'q') {
                piece = QUEEN;
            } else if (c == 'p' || c == 'P') {
                piece = PAWN;
            } else {
                fce_error("Could\'t read fen position", 1);
            }
            Color color = (c >= 'A' && c <= 'Z') ? Color::BLACK : Color::WHITE;
            game.board.set_square(Board::square_to_index(row, col), color, piece);
            col++;
        }
    }
    if (side == "w") {
        game.to_move = Color::WHITE;
    } else if (side == "b") {
        game.to_move = Color::BLACK;
    } else {
        fce_error("Couldn\'t parse side to move in fen", 1);
    }

    for (auto c : castling) {
        if (c == 'K') {
            game.castle_rights[Color::WHITE][Castle::KINGSIDE] = true;
        } else if (c == 'k') {
            game.castle_rights[Color::BLACK][Castle::KINGSIDE] = true;
        } else if (c == 'q') {
            game.castle_rights[Color::BLACK][Castle::QUEENSIDE] = true;
        } else if (c == 'Q') {
            game.castle_rights[Color::WHITE][Castle::QUEENSIDE] = true;
        } else if (c == '-') {
            game.castle_rights[Color::BLACK][Castle::QUEENSIDE] = false;
            game.castle_rights[Color::BLACK][Castle::KINGSIDE] = false;
            game.castle_rights[Color::WHITE][Castle::QUEENSIDE] = false;
            game.castle_rights[Color::WHITE][Castle::KINGSIDE] = false;
            break;
        } else {
            fce_error("Could\'t read fen castling", 1);
        }
    }

    if (en_passant[0] == '-') {
        game.en_passant = no_board_index;
    } else if (en_passant[0] >= 'a' && en_passant[0] <= 'h' && en_passant[1] >= '1' && en_passant[1] <= '8') {
        game.en_passant = (en_passant[0] - 'a') + (en_passant[1] - '1') * 8;
    } else {
        fce_error("Couldn\'t read fen enpassant", 1);
    }
    game.moves_since_capture = stoi(move_since_capture);
    game.move = stoi(move);
    return game;
}