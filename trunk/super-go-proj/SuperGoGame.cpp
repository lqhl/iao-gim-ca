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

	useBook = Util::getBoolean("UseBook");

	useBias = Util::getBoolean("UseBias");

	preprocessChildren = Util::getBoolean("PreprocessChildren");

	largeWinBonus = Util::getDouble("LargeWinBonus");

	patternWeight = Util::getDouble("PatternWeight");

	treeLock = new RWLock();

	searchTime = timeLimit - 500;

	book = NULL;

	openPhase = true;

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

static int numStep = 0;

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

	UCTNode* next = NULL;
	if (useBias) next = workers[0]->selectChildrenMEANBiased(tree, tree->rootNode());
	else next = workers[0]->selectChildrenMEAN(tree, tree->rootNode());
	if (Util::SearchDebugEnabled()) {
		UCTNode* n = tree->rootNode();
		fprintf(Util::LogFile(), "children of root:\n");
		for(vector<UCTNode*>::iterator it = n->children.begin(); it != n->children.end(); ++it) {
			fprintf(Util::LogFile(), "\t");
			(*it)->print(Util::LogFile());
			fprintf(Util::LogFile(), "\n");
		}

		if (next != NULL) {
			fprintf(Util::LogFile(), "** CHOICE ** (i = %d) ", numStep);
			next->print(Util::LogFile());
			fprintf(Util::LogFile(), "\n");
		}
		else {
			fprintf(Util::LogFile(), "** CHOICE ** (i = %d) PASS\n", numStep);
		}
		++numStep;

		fflush(Util::LogFile());
	}

	return next != NULL ? next->move : SG_PASS;
}

SgPoint SuperGoGame::genMove() {
	
	if (openPhase && book != NULL) {
		SgPoint p = book->matchBook(board, board.ToPlay());
		if (p != SG_NULLMOVE) {
			fprintf(Util::LogFile(), "** BOOK ** (i = %d) (%d %d)\n", numStep, Row(p), Col(p));
			++numStep;
			return p;
		}
	}
	openPhase = false; // not useful anymore
	return genMoveUCT();

}


void SuperGoGame::init() {
	if (useBook)
		book = new GoBook();
	for (int i = 0; i < numThread; ++i) {
		workers.push_back(new UCTSearchRunner(this));
	}

	tree = new UCTTree(this, numNode);

	openPhase = true;
}
