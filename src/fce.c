#include "fce.h"
#include <memory.h>

int main(int argc, char *argv[]) {
    fce_board *board = make_fce_board();
}

fce_board *make_fce_board() {
    fce_board *board = malloc(sizeof(fce_board));
    return board;
}