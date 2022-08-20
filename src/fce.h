#ifndef FCE_H
#define FCE_H
#include "stdint.h"
#include <cstdint>
#include <stdint.h>
#define QUEEN_WORTH 9
#define ROOK_WORTH 5
#define PAWN_WORTH 1
#define BISHOP_WORTH 3
#define KNIGHT_WORTH 3
#define BOARD_SIZE_88  64
#define BOARD_COLS 8
#define BOARD_ROWS 8

const uint64_t a_file = 0x0101010101010101;
const uint64_t b_file = a_file << 1;
const uint64_t c_file = b_file << 1;
const uint64_t d_file = c_file << 1;
const uint64_t e_file = d_file << 1;
const uint64_t f_file = e_file << 1;
const uint64_t g_file = f_file << 1;
const uint64_t h_file = g_file << 1;

const uint64_t first_rank = 0xFF;
const uint64_t second_rank = first_rank << (1 * 8);
const uint64_t third_rank = second_rank << (1 * 8);
const uint64_t fourth_rank = third_rank << (1 * 8);
const uint64_t fifth_rank = fourth_rank << (1 * 8);
const uint64_t sixth_rank = fifth_rank << (1 * 8);
const uint64_t seventh_rank = sixth_rank << (1 * 8);
const uint64_t eighth_rank = seventh_rank << (1 * 8);

const uint64_t queenside = a_file | b_file | c_file | d_file;
const uint64_t kingside = e_file | f_file | g_file | h_file;
const uint64_t center = (d_file | e_file) & (fourth_rank | fifth_rank);
const uint64_t extended_center = (c_file | d_file | e_file | f_file) & (third_rank | fourth_rank | fifth_rank | sixth_rank);

const uint64_t white_side = first_rank | second_rank | third_rank | fourth_rank;

const uint64_t black_side = fifth_rank | sixth_rank | seventh_rank | eighth_rank;

enum color{
    WHITE,
    BLACK
};

enum board_type{
    PAWN_BB,
    KNIGHT_BB,
    BISHOP_BB,
    ROOK_BB,
    QUEEN_BB,
    KING_BB,
    WHITE_BB,
    BLACK_BB
};

struct bitboard {
    uint64_t board[8];
};

enum token {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

struct square {
    enum token token;
    enum color color;
};

struct board {
    struct square squares[8][8];
    int64_t eval;
};

void fce_error(const char *message);

struct bitboard *make_bitboard();
struct board *board_make();
void print_fce_board(struct bitboard);
void init_bitboard(struct bitboard *board);
void board_init(struct board *board);
struct bitboard *move_piece();
#endif