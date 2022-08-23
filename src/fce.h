#ifndef FCE_H
#define FCE_H
#include "stdint.h"
#include <stdbool.h>
#define QUEEN_WORTH 9
#define ROOK_WORTH 5
#define PAWN_WORTH 1
#define BISHOP_WORTH 3
#define KNIGHT_WORTH 3
#define BOARD_SIZE_88  64
#define BOARD_COLS 8
#define BOARD_ROWS 8
#define BOARD_SIZE 64
#define FIGURE_LIST_FIGURE_COUNT 6
#define FIGURE_LIST_ARRAY_LENGTH 16
#define NO_FIELD 255

enum color{
    WHITE = 0,
    BLACK = 1
};

enum token {
    PAWN = 0,
    KNIGHT = 1,
    BISHOP = 2,
    ROOK = 3,
    QUEEN = 4,
    KING = 5,
    NO_PIECE = 6
};

struct move {
    uint8_t from;
    uint8_t to;
};

struct figure {
    enum token token;
    enum color color;
};

struct figure_list {
    uint8_t figures[FIGURE_LIST_ARRAY_LENGTH];
    uint8_t length;
};

struct state {
    // all the quares of the board, pointer to the corresponding piece/square
    struct figure board[BOARD_SIZE];
    // store pieces in a list which makes move generation and searching for specific pieces easier
    struct figure_list figure_lists[FIGURE_LIST_FIGURE_COUNT];
    int16_t eval;
    enum color side;
    bool white_queenside_castle;
    bool white_kingside_castle;
    bool black_kingside_castle;
    bool black_queenside_castle;
    uint16_t move;
    uint16_t moves_since_capture_or_pawn;
    uint8_t en_passant;
};

void fce_error(const char *message);

void state_print_all_data(struct state *state);
void state_init(struct state *state);
void state_read_insert_fen(const char* fen, struct state* state);
void state_print_board_data(struct state*state);
char get_symbol(enum token token, enum color side);
char *get_bool(bool vals);
char get_rows(int rows);
char get_cols(int cols);
char *get_color(enum color color);
#endif