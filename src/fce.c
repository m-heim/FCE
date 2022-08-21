#include "fce.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>


int main(int argc, char *argv[]) {
    int read_fen = 0;
    printf("Initializing board...\n");
    struct state *state = state_make();
    state_init(state);
    printf("Printing board...\n");
    print_fce_board(state);
    struct state *state7 = state_make();
    read_fen_save_to_state("../assets/game.fen", state7);
    print_fce_board(state7);
    return 0;
}

void fce_error(const char *message) {
    printf("%s\n", message);
    exit(1);
}

struct 


struct state *state_make() {
    struct state *state = calloc(1, sizeof(struct state));
    if (state == NULL) {
        fce_error("Couldn't create board");
    }
    return state;
}

void state_init(struct state *state) {
    read_fen_save_to_state("../assets/start.fen", state);
}

void read_fen_save_to_state(const char* fen, struct state* state) {
    FILE *file = fopen(fen, "r");
    char position[100];
    char side;
    char castle[10];
    char en_passant[10];
    int moves_since;
    int moves;
    fscanf(file, "%s %c %s %s %d %d", position, &side, castle, en_passant, &moves_since, &moves);
    int rows = 0;
    int cols = 0;
    int token_n = 0;
    for (int i = 0; i < 100; i++) {
        char c = position[i];
        if (c == '\0') {
            break;
        }
        if (c == '/') {
            rows++;
            cols = 0;
            continue;
        }
        if (c >= '0' && c <= '9') {
            int n = c - '0';
            for (int j = 0; j < n; j++) {
                state->squares[rows][cols] = NULL;
            }
            cols += n;
            continue;
        }
        state->tokens[token_n].color = (c >= 'A' && c <= 'Z');
        state->tokens[token_n].rows = rows;
        state->tokens[token_n].cols = cols;
        state->squares[rows][cols] = &(state->tokens[token_n]);
        if (c == 'r' || c == 'R') {
            state->tokens[token_n].token = ROOK;
        } else if (c == 'b' || c == 'B') {
            state->tokens[token_n].token = BISHOP;
        } else if (c == 'k' || c == 'K') {
            state->tokens[token_n].token = KING;
        } else if (c == 'n' || c == 'N') {
            state->tokens[token_n].token = KNIGHT;
        } else if (c == 'Q' || c == 'q') {
            state->tokens[token_n].token = QUEEN;
        } else if (c == 'p' || c == 'P') {
            state->tokens[token_n].token = PAWN;
        } else {
            fce_error("Could\'t read fen position");
        }
        cols++;
        token_n++;
    }
    state->side = side == 'b';

    for (int i = 0; i < 10; i++) {
        char c = castle[i];
        if (c == '\0') {
            break;
        }
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
        state->en_passant = NULL;
    } else {
        state->en_passant = state->squares[en_passant[0] - 'a'][en_passant[1] - '1'];
    }

    state->moves_since_capture_or_pawn = moves_since;
    state->move = moves;
    fclose(file);
}

void print_fce_board(struct state *state) {
    for(char i = 7; i >= 0; i--) {
        for(char j = 7; j >= 0; j--) {
            if (state->squares[i][j] == NULL) {
                printf("   ");
                continue;
            }
            printf(" %c ", - (state->squares[i][j]->color * 32) + get_symbol(state->squares[i][j]->token));
        }
        printf("\n");
    }
    printf("Black can castle queen: %s\n", get_bool(state->black_queenside_castle));
    printf("Black can castle kingside: %s\n", get_bool(state->black_kingside_castle));
    printf("White can castle kingside: %s\n", get_bool(state->white_kingside_castle));
    printf("White can castle queen: %s\n", get_bool(state->white_queenside_castle));
    if (state->en_passant != NULL) {
        printf("Possible enpassant: %c%c\n", get_rows(state->en_passant->rows), get_cols(state->en_passant->cols));
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
char get_rows(int rows) {
    return 'a' + rows;
}
char get_cols(int cols) {
    return '1' + cols;
}

char get_symbol(enum token token) {
    switch (token) {
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
        default:
            return '?';
    }
}
