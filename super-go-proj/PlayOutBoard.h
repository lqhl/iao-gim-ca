#ifndef PLAY_OUT_BOARD_H
#define PLAY_OUT_BOARD_H

#include "SuperGo.h"


// board for play-out phase
class PlayOutBoard{
public:
	int boardSize;

	int steps; // how many steps have been executed

	MOVE *moves; // don't know if an array would be more efficient

	PlayOutBoard(int size);

	void execute(MOVE move, COLOR toPlay);

	MOVE nextMove(COLOR toPlay);

	void rollBack(int level);

	~PlayOutBoard();

};

#endif
