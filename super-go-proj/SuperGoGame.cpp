#include "SuperGo.h"
#include "SuperGoGame.h"
#include "util/util.h"
#include <assert.h>

SuperGoGame::SuperGoGame() : board(BOARD_SIZE) {
	//board = Board(BOARD_SIZE);

	numNode = Util::getInt("NumNode");

	numPlayOut = Util::getInt("NumPlayOut");

	treeLock = new RWLock();

	searchTime = TIME_LIMIT - 500;

	for(int i=0; i<NUM_THREAD; ++i) {
		workers.push_back(UCTSearchRunner(this));
	}

	tree = new UCTTree(this, numNode);
}

void SuperGoGame::setPlayer(int player) {
	assert(player == WHITE || player == BLACK);
	this->player = player;
	this->opponent = player == WHITE ? BLACK : WHITE;
}

void SuperGoGame::genMoveUCT(MOVE move) {
	board.execute(move);
}