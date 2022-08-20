#include "fce.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char *argv[]) {
    printf("Initializing board...\n");
    FILE *standard_setup_file = fopen("../assets/standard_setup", "r");
    char standard_setup[100];
    struct bitboard *board = make_bitboard();
    init_fce_board(board);
    printf("Printing board...\n");
    print_fce_board(board);
}

void fce_error(const char *message) {
    printf("%s\n", message);
    exit(1);
}

struct bitboard *make_bitboard() {
    struct bitboard *board = calloc(1, sizeof(struct bitboard));
    if (board == NULL) {
        fce_error("Couldn't create board");
    }
    return board;
}
void init_fce_board(struct bitboard *board) {
    board->board[PAWN_BB] = second_rank | seventh_rank;
    board->board[KING_BB] = (first_rank | eighth_rank) & (e_file);
    board->board[QUEEN_BB] = (first_rank | eighth_rank) & (d_file);
    board->board[BISHOP_BB] = (first_rank | eighth_rank) & (c_file | f_file);
    board->board[KNIGHT_BB] = (first_rank | eighth_rank) & (b_file | g_file);
    board->board[ROOK_BB] = (first_rank | eighth_rank) & (a_file | h_file);
    board->board[WHITE_BB]= first_rank | second_rank;
    board->board[BLACK_BB] = seventh_rank | eighth_rank;

}

void print_fce_board(struct bitboard *board) {
    for(char i = 0; i < 8; i++) {
        for(char j = 0; j < 8; j++) {
            printf("%c", get_symbol(board->field[i][j].type));
        }
        printf("\n");
    }
}

char get_symbol(piece_types type) {
    switch (type) {
        case pawn:
            return 'p';
        case knight:
            return 'n';
        case bishop:
            return 'b';
        case king:
            return 'k';
        case rook:
            return 'r';
        case queen:
            return 'q';
        case empty:
            return ' ';
    }
}
fce_board *get_fce_board(fen fen_string) {

}