#ifndef FCE_H
#define FCE_H
#define QUEEN_WORTH = 9
#define ROOK_WORTH = 5
#define PAWN_WORTH = 1
#define BISHOP_WORTH = 3
#define KNIGHT_WORTH = 3

typedef struct {
    char piece[8][8];

} fce_board;

fce_board *make_fce_board();
#endif