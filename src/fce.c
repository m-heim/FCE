#include "fce.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char *argv[]) {
    printf("Initializing board...\n");
    FILE *standard_setup_file = fopen("../assets/standard_setup", "r");
    char standard_setup[100];
    fce_board *board = make_fce_board();
    init_fce_board(board);
    printf("Printing board...\n");
    print_fce_board(board);
}

fce_board *make_fce_board() {
    fce_board *board = calloc(1, sizeof(fce_board));
    return board;
}
void init_fce_board(fce_board *board) {
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            switch(j) {
                case 1:
                board->field[i][j].type = pawn;
                board->field[i][j].color = white;
                case 6:
                board->field[i][j].type = pawn;
                board->field[i][j].color = black;
            }
        }
    }
}

void print_fce_board(fce_board *board) {
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