#include "UCTSearchRunner.h"
#include "SuperGoGame.h"
#include "Poco/Timestamp.h"

using Poco::Timestamp;
UCTSearchRunner::UCTSearchRunner(SuperGoGame* game) : playOutBoard(SuperGoGame::BOARD_SIZE) {
	this->game = game;
}


void UCTSearchRunner::run() {
	searchBoard(game->searchTime, game->numPlayOut, &game->board, game->tree, game->treeLock);
}

void UCTSearchRunner::searchBoard(int timeLimit, int numPlayOut, Board* board, UCTTree* tree, RWLock* treeLock ){
	long limit = (long)timeLimit * 1000;
	Timestamp timer;

	for(int i=0;;++i) {
		// break the buffer
		if (i % 100 == 0) {
			if (timer.elapsed() >= limit) break;
		}

		vector<int> seq;
		searchInTree(seq);
	}
}
