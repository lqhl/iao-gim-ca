#include "Board.h"
#include <assert.h>

Board::Board() {
	boardSize = -1;
}
Board::Board(int size) {
	int n = (size+1) * (size+1);
	this->boardSize = size;
	this->moves = new MOVE[n];
	this->board = new int[n];
	this->steps = 0;
	for(int i=0; i<n; ++i) board[i] = EMPTY;
}

void Board::execute(MOVE move) {
	// BLACK moves at even steps while WHITE moves at odd steps
	assert(board[move] == EMPTY);

	if((steps & 1) == 0) {
		board[move] = BLACK;
	}
	else {
		board[move] = WHITE;
	}

	moves[steps] = move;
	++steps;

	// update the info
}

void Board::rollBack(MOVE move) {
	while (steps > move) {
		--steps;
		int m = moves[steps];
		assert(board[m] == (((steps & 1) == 0) ? BLACK : WHITE));
		board[m] = EMPTY;
	}

}
