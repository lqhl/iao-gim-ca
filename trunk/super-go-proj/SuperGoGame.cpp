#include "SuperGo.h"
#include "SuperGoGame.h"
#include "util/util.h"
#include <assert.h>

SuperGoGame::SuperGoGame(const string& fileName) : board(BOARD_SIZE) {
	Util::init(fileName);

	numNode = Util::getInt("NumNode");

	numPlayOut = Util::getInt("NumPlayOut");

	CFinal = Util::getDouble("CFinal");

	CInitial = Util::getDouble("CInitial");

	CUCT = Util::getDouble("CUCT");

	komi = Util::getInt("Komi");

	firstPlayValue = Util::getDouble("FirstPlayUrgencey");

	treeLock = new RWLock();

	searchTime = TIME_LIMIT - 500;

}

void SuperGoGame::setPlayer(int player) {
	assert(player == WHITE || player == BLACK);
	this->player = player;
	this->opponent = player == WHITE ? BLACK : WHITE;
}

MOVE SuperGoGame::genMoveUCT(MOVE move) {
	if (move == NULL_MOVE) {
		assert(board.getLevel() == 0 && player == BLACK);
	}
	else {
		board.execute(move);
	}
	for(int i=0; i<NUM_THREAD; ++i) {
		workers[i].start();
	}
	for(int i=0; i<NUM_THREAD; ++i) {
		workers[i].thread.join();
	}

	int n = workers[0].selectChildrenCount(tree, tree->root);
	return tree->node[n].move;
}

void SuperGoGame::init() {
	for(int i=0; i<NUM_THREAD; ++i) {
		workers.push_back(UCTSearchRunner(this));
	}

	tree = new UCTTree(this, numNode);
}