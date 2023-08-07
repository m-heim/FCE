#include "fen.hpp"

Game parse_fen(std::string fen) {
    FILE *file = fopen(fen, "r");
    char position[100];
    char side;
    char castle[10];
    char en_passant[10];
    int moves_since;
    int moves;
    fscanf(file, "%s %c %s %s %d %d", position, &side, castle, en_passant, &moves_since, &moves);
    int row = 0;
    int col = 0;
    for (int i = 0; i < strlen(position); i++) {
        char c = position[i];
        if (c == '/') {
            row++;
            col = 0;
            continue;
        }
        if (c >= '0' && c <= '9') {
            int n = c - '1';
            col += n;
            continue;
        }
        enum token figure;
        state->board[row * 8 + col].color = (c >= 'A' && c <= 'Z');
        if (c == 'r' || c == 'R') {
            figure = ROOK;
        } else if (c == 'b' || c == 'B') {
            figure = BISHOP;
        } else if (c == 'k' || c == 'K') {
            figure = KING;
        } else if (c == 'n' || c == 'N') {
            figure = KNIGHT;
        } else if (c == 'Q' || c == 'q') {
            figure = QUEEN;
        } else if (c == 'p' || c == 'P') {
            figure = PAWN;
        } else {
            fce_error("Could\'t read fen position");
        }
        state_insert_figure(state, figure, (c >= 'A' && c <= 'Z'), row * 8 + col);
        col++;
    }
    state->side = side == 'b';

    for (int i = 0; i < strlen(castle); i++) {
        char c = castle[i];
        if (c == 'K') {
            state->black_kingside_castle = true;
        } else if (c == 'k') {
            state->white_kingside_castle = true;
        } else if (c == 'q') {
            state->white_queenside_castle = true;
        } else if (c == 'Q') {
            state->black_queenside_castle = true;
        } else {
            fce_error("Could\'t read fen");
        }
    }

    if (en_passant[0] == '-') {
        state->en_passant = NO_FIELD;
    } else {
        state->en_passant = en_passant[0] - 'a' + 8 * en_passant[1] - '1';
    }

    state->moves_since_capture_or_pawn = moves_since;
    state->move = moves;
    fclose(file);
}