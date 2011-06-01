#ifndef BOARD_H
#define BOARD_H

#include "SuperGo.h"

class Board{
public:
	int boardSize;

	int steps; // how many steps have been executed

	MOVE* moves;

	int* board;

	Board();

	Board(int size);

	void execute(MOVE move);

	void rollBack(MOVE move);

};

#endif
