#ifndef SUPERGO_H_
#define SUPERGO_H_

#define VERSION_STRING "1.0"

typedef double COUNT;

typedef double VALUE;

enum BoardState {
	BLACK_WIN = 0, WHITE_WIN = 1, DRAW = 2, NOT_PROVEN = 3
};

const int board_size = 13;

int pk_main();

#endif
