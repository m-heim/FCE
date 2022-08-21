#include "fce_bitboard.h"
void init_bitboard(struct bitboard *board) {
    board->board[PAWN_BB] = second_rank | seventh_rank;
    board->board[KING_BB] = (first_rank | eighth_rank) & (e_file);
    board->board[QUEEN_BB] = (first_rank | eighth_rank) & (d_file);
    board->board[BISHOP_BB] = (first_rank | eighth_rank) & (c_file | f_file);
    board->board[KNIGHT_BB] = (first_rank | eighth_rank) & (b_file | g_file);
    board->board[ROOK_BB] = (first_rank | eighth_rank) & (a_file | h_file);
    board->board[WHITE_BB]= first_rank | second_rank;
    board->board[BLACK_BB] = seventh_rank | eighth_rank;

}
struct bitboard *make_bitboard() {
    struct bitboard *board = calloc(1, sizeof(struct bitboard));
    if (board == NULL) {
        fce_error("Couldn't create board");
    }
    return board;
}