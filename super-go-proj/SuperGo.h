#ifndef SUPERGO_H_
#define SUPERGO_H_

typedef int MOVE;

typedef double COUNT;

typedef double VALUE;

#define EMPTY -1
#define BLACK 1
#define WHITE 2

#define PASS -1
#define NULL_MOVE -2

enum GameState {
	WIN, LOSS, UN_PROVEN
};

#endif