#include "SuperGo.h"
#include "SuperGoGame.h"
#include "util/util.h"
#include <cstdio>
#include <assert.h>

SuperGoGame::SuperGoGame() :
	board(BOARD_SIZE) {

	numNode = Util::getInt("NumNode");

	numPlayOut = Util::getInt("NumPlayOut");

	CFinal = Util::getDouble("CFinal");

	CInitial = Util::getDouble("CInitial");

	CUCT = Util::getDouble("CUCT");

	komi = Util::getDouble("Komi");

	firstPlayValue = Util::getDouble("FirstPlayUrgency");

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

	UCTNode* next = workers[0]->selectChildrenMEAN(tree, tree->rootNode());
	if (Util::SearchDebugEnabled()) {
		static int i = 0;
		UCTNode* n = tree->rootNode();
		fprintf(Util::LogFile(), "children of root:\n");
		for(vector<UCTNode*>::iterator it = n->children.begin(); it != n->children.end(); ++it) {
			fprintf(Util::LogFile(), "\t");
			(*it)->print(Util::LogFile());
			fprintf(Util::LogFile(), "\n");
		}

		if (next != NULL) {
			fprintf(Util::LogFile(), "** CHOICE ** (i = %d)", i);
			next->print(Util::LogFile());
			fprintf(Util::LogFile(), "\n");
		}
		else {
			fprintf(Util::LogFile(), "** CHOICE ** (i = %d) PASS\n", i);
		}
		++i;

		fflush(Util::LogFile());
	}

	return next != NULL ? next->move : SG_PASS;
}


void SuperGoGame::init() {
	for (int i = 0; i < numThread; ++i) {
		workers.push_back(new UCTSearchRunner(this));
	}

	tree = new UCTTree(this, numNode);
}
