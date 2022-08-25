#include "fce.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>


int main(int argc, char *argv[]) {
    int read_fen = 0;
    printf("Initializing board...\n");
    struct state state;
    state_init(&state);
    printf("Printing board...\n");
    state_print_all_data(&state);
    state_print_board_data(&state);
    struct state state7;
    
    //read_fen_save_to_state("../assets/game.fen", state7);
    //print_fce_board(&state7);
    return 0;
}

void fce_error(const char *message) {
    printf("%s\n", message);
    exit(1);
}

void state_init(struct state *state) {
    for (int i = 0; i < FIGURE_COUNT; i++) {
        state->figure_lists[WHITE][i].length = 0;
        state->figure_lists[BLACK][i].length = 0;
    }
    for (int i = 0; i < BOARD_SIZE; i++) {
        state->board[i].token = NO_PIECE;
    }
    state->en_passant = NO_FIELD;
    state->black_kingside_castle = false;
    state->black_queenside_castle = false;
    state->white_kingside_castle = false;
    state->white_queenside_castle = false;
    state_read_insert_fen("../assets/start.fen", state);
}

void state_generate_all_moves(struct moves *moves, struct state* state, enum color side) {
    struct figure_list *lists = state->figure_lists[side];
    for(int i = 0; i < lists[PAWN].length; i++) {
        uint8_t field = lists[PAWN].figures[i];
        // normal move
        if (state->board[field + 8].token == NO_PIECE) {
            moves_append(moves, field, field + 8);
        // sidewards capture
        } else if (state->board[field + 7].color != side && state->board[field + 7].token != NO_PIECE) {
            moves_append(moves, field, field + 7);
        } else if (state->board[field + 9].color != side && state->board[field + 8].token != NO_PIECE) {
            moves_append(moves, field, field + 9);
        // en passant
        } else if (field+7 == state->en_passant || field + 9 == state->en_passant) {
            moves_append(moves, field, state->en_passant);
        // double step
        } else if (side == WHITE && field >= 8 && field < 16 && state->board[field+8].token != NO_PIECE && state->board[field+16].token != NO_PIECE) {
            moves_append(moves, field, field + 16);
        } else if (side == BLACK && field < 56 && field >= 48 && state->board[field-8].token != NO_PIECE && state->board[field-16].token != NO_PIECE) {
            moves_append(moves, field, field + 16);
        }
    }
    for(int i = 0; i < lists[KNIGHT].length; i++) {
        int8_t knight_moves[8] = {-21, -19, -12, -8, 8, 12, 19, 21};
        int8_t field = (int) lists[KNIGHT].figures[i];
        for (int j = 0; j < 8; j++) {
            int8_t to = field + knight_moves[j];
            if ((to >= 0 && to < 64 && state->board[to].token == NO_PIECE) || (to >= 0 && to < 64 && state->board[to].color != side)) {
                moves_append(moves, field, to);
            }
        }
    }
    for(int i = 0; i < lists[BISHOP].length; i++) {
        int8_t bishop_moves[8] = {-21, -19, -12, -8, 8, 12, 19, 21};
        int8_t field = (int) lists[KNIGHT].figures[i];
        for (int j = 0; j < 8; j++) {
            int8_t to = field + knight_moves[j];
            if ((to >= 0 && to < 64 && state->board[to].token == NO_PIECE) || (to >= 0 && to < 64 && state->board[to].color != side)) {
                moves_append(moves, field, to);
            }
        }
    }
}

void moves_append(struct moves *moves, uint8_t from, uint8_t to) {
    moves->from[moves->length] = from;
    moves->to[moves->length] = to;
    moves->length++;
}

bool state_is_king_checked(struct state *state, enum color side) {
    uint8_t king_position = state->figure_lists[side][KING].figures[0];
    struct moves moves;
    state_generate_all_moves(&moves, state, side);
    for (int i = 0; i < moves.length; i++) {
        if(moves.to[i] == king_position) {
            return true;
        }
    }
    return false;
}

void state_insert_figure(struct state *state, enum token figure, enum color side, uint8_t position) {
    state->board[position].color = side;
    state->board[position].token = figure;
    state->figure_lists[side][figure].figures[state->figure_lists[side][figure].length] = position;
    state->figure_lists[side][figure].length++;
}

void state_read_insert_fen(const char* fen, struct state* state) {
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

void state_print_board_data(struct state*state) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%d color %d, piece %d\n", i, state->board[i].color, state->board[i].token);
    }
}

void state_print_all_data(struct state *state) {
    for(int row = 7; row >= 0; row--) {
        for(int col = 0; col <= 7; col++) {
            printf(" %c ", get_symbol(state->board[row * 8 + col].token, state->board[row * 8 + col].color));
        }
        printf("\n");
    }
    printf("Black can castle queen: %s\n", get_bool(state->black_queenside_castle));
    printf("Black can castle kingside: %s\n", get_bool(state->black_kingside_castle));
    printf("White can castle kingside: %s\n", get_bool(state->white_kingside_castle));
    printf("White can castle queen: %s\n", get_bool(state->white_queenside_castle));
    if (state->en_passant ==  NO_FIELD) {
        printf("Possible enpassant: %c%c\n", get_rows(state->en_passant / 8), get_cols(state->en_passant % 8));
    }
    printf("%s to move\n", get_color(state->side));
    printf("Move: %d\n", state->move);
    printf("Moves since capture: %d\n", state->moves_since_capture_or_pawn);
}

char *get_bool(bool vals) {
    if (vals) {
        return "true";
    } else {
        return "false";
    }
}
char *get_color(enum color color) {
    if (color) {
        return "black";
    } else {
        return "white";
    }
}
char get_cols(int rows) {
    return 'a' + rows;
}
char get_rows(int cols) {
    return '1' + cols;
}

char get_symbol(enum token token, enum color side) {
    if (side == BLACK) {
        switch (token) {
            case PAWN:
                return 'P';
                break;
            case KNIGHT:
                return 'N';
                break;
            case BISHOP:
                return 'B';
                break;
            case KING:
                return 'K';
                break;
            case ROOK:
                return 'R';
                break;
            case QUEEN:
                return 'Q';
                break;
            case NO_PIECE:
                return ' ';
        }
    } else if (side == WHITE) {
        switch(token) {
            case PAWN:
                return 'p';
                break;
            case KNIGHT:
                return 'n';
                break;
            case BISHOP:
                return 'b';
                break;
            case KING:
                return 'k';
                break;
            case ROOK:
                return 'r';
                break;
            case QUEEN:
                return 'q';
                break;
            case NO_PIECE:
                return ' ';
        }
    } else {
        return ' ';
    }
}
