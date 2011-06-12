#ifndef SUPER_GO_GAME_H
#define SUPER_GO_GAME_H

#include "SuperGo.h"
#include "UCTSearchRunner.h"
#include "GoBoard.h"
#include "UCTTree.h"
#include "Poco/RWLock.h"
#include "GoBoardUtil.h"
#include <vector>
#include <string>

using std::string;
using std::vector;
using Poco::RWLock;

using GoBoardUtil::getGoNeighbors;


class SuperGoGame;

typedef void (SuperGoGame::*GenMoveFunc)();


class SuperGoGame {
public:
	static const int BOARD_SIZE = 13;
//	static const int NS = BOARD_SIZE+1, WE = 1;

	int numThread;

	int timeLimit; // in milliseconds

	/* some configuration parameters */
	// for each play-in-tree, how many play-out will be simulated
	int numPlayOut;

	// the number of nodes stored in this game
	int numNode;

	// time used for searching in the threads, in milliseconds
	int searchTime;

	// komi
	int komi;

	// parameters for the RAVE
	double CInitial, CFinal;

	// parameters for the UCT
	double CUCT;

	// first play urgency
	double firstPlayValue;

	GenMoveFunc genMove;

	vector<UCTSearchRunner*> workers;

	RWLock* treeLock;

	UCTTree* tree;

	int player, opponent;

	GoBoard board;

	SuperGoGame();

	void setPlayer(int player);

	void init();

	void execute(SgPoint move, SgBlackWhite color);

	// given a move, generate a new move

	SgPoint genMoveUCT();


	COUNT evaluate(GoUctBoard* board) {
		int white = 0, black = 0;
		bool marked[SG_MAXPOINT];
		fill(marked, marked+SG_MAXPOINT, false);
		for(GoUctBoard::Iterator it(*board); it; ++it) {
			SgBoardColor c = board->GetColor(*it);
			poco_assert(c != SG_BORDER);
			if (c == SG_BLACK) black += 2;
			else if (c == SG_WHITE) white += 2;
			else if (marked[*it]) continue;

			// flood

			SgPoint stack[SG_MAXPOINT];
			int k = 0;
			int num = 0;
			bool whiteNb = false, blackNb = false;
			marked[*it] = true;
			stack[k++] = *it;

			vector<SgPoint> nb;
			while(k > 0) {
				SgPoint cur = stack[--k];
				nb.clear();
				//nb.reserve(4);
				getGoNeighbors(*board, *it, nb);
				for(vector<SgPoint>::iterator j = nb.begin(); j != nb.end(); ++j) {
					if (board->GetColor(*j) == SG_BLACK) blackNb = true;
					else if (board->GetColor(*j) == SG_WHITE) whiteNb = true;
					else {
						poco_assert(board->GetColor(*j) == SG_EMPTY);
						if (!marked[*j]) {
							marked[*j] = true;
							stack[k++] = *j;
							++num;
						}
					}
				}

			}

			if (blackNb) black += num;
			if (!whiteNb) black += num;
			if (whiteNb) white += num;
			if (!blackNb) white += num;
		}
		return (black - white) / 2.0;
	}

	void testRun(int num, ostream& out) {
		for(int i=0; i<num; ++i) {
			SgPoint move = genMoveUCT();
			execute(move, i % 2 == 0 ? SG_BLACK : SG_WHITE);

			board.printBoard(out);
		}
	}
};

#endif
