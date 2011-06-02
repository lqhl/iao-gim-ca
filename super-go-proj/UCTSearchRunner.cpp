#include <math.h>
#include "UCTSearchRunner.h"
#include "SuperGoGame.h"
#include "Poco/Timestamp.h"

using Poco::Timestamp;
UCTSearchRunner::UCTSearchRunner(SuperGoGame* game) : playOutBoard(SuperGoGame::BOARD_SIZE) {
	this->game = game;
	this->ravePara1 = 1./game->CInitial;
	this->ravePara2 = 1./game->CFinal;
	this->CUCT = game->CUCT;
}

void UCTSearchRunner::start() {
	thread.start(*this);
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


		int level = board->getLevel();

		vector<int> seq;
		searchInTree(tree, treeLock, board, seq);
		for(int j=0; j<numPlayOut; ++j) {
			vector<int> seq2;
			int res = playOut(board, seq2);

			upateStat(tree, treeLock, res, seq, seq2);
		}

		board->rollBack(level);

	}
}

void UCTSearchRunner::searchInTree(UCTTree* tree, RWLock* treeLock, Board* board, vector<int>& seq) {

}

int UCTSearchRunner::playOut(Board* board, vector<int>& seq) {
	return -1;
}

void UCTSearchRunner::upateStat(UCTTree* tree, RWLock* treeLock, int result, vector<int>& seqIn, vector<int>& seqOut) {
	treeLock->writeLock();

	//update the RAVE values
	// first goes down seqOut and build a hash table
	// the goes down seqIn and update the rave values

	// update the move count
	// just goes down seqIn and update

	treeLock->unlock();
}

int UCTSearchRunner::selectChildrenUCT(UCTTree* tree, int n) {
	int k=-1;
	COUNT best = -1;
	UCTNode& node = tree->node[n];
	vector<int>& v = node.children;
	for(int i=0; i<v.size(); ++i) {
		UCTNode& m = tree->node[v[i]];
		double raveWeight = m.raveCount / (ravePara1 + ravePara2 * m.raveCount);
		double moveWeight = m.visitCount;
		double uct = CUCT * sqrt(log(node.visitCount) / (m.visitCount+1));
		double res = (raveWeight * m.raveValue + moveWeight * m.visitValue) / (raveWeight + moveWeight) + uct;



		if (k == -1 || res > best) {
			best = res;
			k = v[i];
		} 
	}
	return k;
}

int UCTSearchRunner::selectChildrenCount(UCTTree* tree, int n) {
	int k=-1;
	COUNT best = -1;
	UCTNode& node = tree->node[n];
	vector<int>& v = node.children;
	for(int i=0; i<v.size(); ++i) {
		UCTNode& m = tree->node[v[i]];
		if (m.visitCount > best) {
			best = m.visitCount;
			k = v[i];
		}
	}
	return k;
}

int UCTSearchRunner::selectChildrenMEAN(UCTTree* tree, int n) {
	int k=-1;
	COUNT best = -1;
	UCTNode& node = tree->node[n];
	vector<int>& v = node.children;
	for(int i=0; i<v.size(); ++i) {
		UCTNode& m = tree->node[v[i]];
		if (k == -1 || m.visitValue > best) {
			best = m.visitValue;
			k = v[i];
		}
	}
	return k;
}

int UCTSearchRunner::selectChildrenRAVE(UCTTree* tree, int n) {
	int k=-1;
	COUNT best = -1;
	UCTNode& node = tree->node[n];
	vector<int>& v = node.children;
	for(int i=0; i<v.size(); ++i) {
		UCTNode& m = tree->node[v[i]];
		double raveWeight = m.raveCount / (ravePara1 + ravePara2 * m.raveCount);
		double moveWeight = m.visitCount;
		double res = (raveWeight * m.raveValue + moveWeight * m.visitValue) / (raveWeight + moveWeight);
		if (k == -1 || res > best) {
			best = res;
			k = v[i];
		} 
	}
	return k;
}