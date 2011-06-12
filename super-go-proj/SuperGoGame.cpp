#include "SuperGo.h"
#include "SuperGoGame.h"
#include "util/util.h"
#include <assert.h>

SuperGoGame::SuperGoGame() :
	board(BOARD_SIZE) {

	numNode = Util::getInt("NumNode");

	numPlayOut = Util::getInt("NumPlayOut");

	CFinal = Util::getDouble("CFinal");

	CInitial = Util::getDouble("CInitial");

	CUCT = Util::getDouble("CUCT");

	komi = Util::getInt("Komi");

	firstPlayValue = Util::getDouble("FirstPlayUrgencey");

	timeLimit = Util::getInt("TimeLimit");

	numThread = Util::getInt("NumThread");

	treeLock = new RWLock();

	searchTime = timeLimit - 500;

}

void SuperGoGame::setPlayer(int player) {
	assert(player == SG_WHITE || player == SG_BLACK);
	this->player = player;
	this->opponent = player == SG_WHITE ? SG_BLACK : SG_WHITE;
}

void SuperGoGame::execute(SgPoint move, SgBlackWhite color) {
	board.Play(move, color);
	tree->update(move);
}

SgPoint SuperGoGame::genMoveUCT() {
	if (numThread == 1) {
		workers[0]->run();
	} else {
		for (int i = 0; i < numThread; ++i) {
			workers[i]->start();
		}
		for (int i = 0; i < numThread; ++i) {
			workers[i]->thread.join();
		}
	}

	UCTNode* next = workers[0]->selectChildrenCount(tree, tree->rootNode());
	return next->move;
}


void SuperGoGame::init() {
	for (int i = 0; i < numThread; ++i) {
		workers.push_back(new UCTSearchRunner(this));
	}

	tree = new UCTTree(this, numNode);
}
