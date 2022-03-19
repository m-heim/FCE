#ifndef FCE_H
#define FCE_H
#define QUEEN_WORTH = 9
#define ROOK_WORTH = 5
#define PAWN_WORTH = 1
#define BISHOP_WORTH = 3
#define KNIGHT_WORTH = 3
typedef char *fen;

typedef enum {
    white,
    black
} colors;
typedef enum {
    empty = 0,
    pawn,
    knight,
    bishop,
    queen,
    king,
    rook,
} piece_types;
typedef int bool2;
typedef struct {
    piece_types type;
    bool2 color; // 0 if black 1 if white
} fce_piece;
typedef struct {
    fce_piece field[8][8];
} fce_board;
fce_board *make_fce_board();
char get_symbol(piece_types type);
void print_fce_board(fce_board *board);
void init_fce_board(fce_board *board);
#endif