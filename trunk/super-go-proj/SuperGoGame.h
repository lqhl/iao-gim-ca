#ifndef SUPER_GO_GAME_H
#define SUPER_GO_GAME_H

#include "SuperGo.h"
#include "UCTSearchRunner.h"
#include "GoBoard.h"
#include "UCTTree.h"
#include "Poco/RWLock.h"
#include "GoBoardUtil.h"
#include "GoBook.h"
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
	double komi;

	// parameters for the RAVE
	double CInitial, CFinal;

	// parameters for the UCT
	double CUCT;

	// first play urgency
	double firstPlayValue;

	bool useBook;

	bool preprocessChildren;

	bool useBias;

	double largeWinBonus;

	vector<UCTSearchRunner*> workers;

	RWLock* treeLock;

	UCTTree* tree;

	GoBook* book;

	int player, opponent;

	GoBoard board;

	bool openPhase;

	double patternWeight;

	SuperGoGame();

	void setPlayer(int player);

	void init();

	void execute(SgPoint move, SgBlackWhite color);

	// given a move, generate a new move

	SgPoint genMove();
	SgPoint genMoveUCT();

	template<typename BOARD>
	COUNT evaluate(BOARD* board) {
		int white = 0, black = 0;
		bool marked[SG_MAXPOINT];
		fill(marked, marked + SG_MAXPOINT, false);

		//if (Util::UctDebugEnabled()) {
		//	board->printBoard(cerr);
		//}
		for (BOARD::Iterator it(*board); it; ++it) {
			SgBoardColor c = board->GetColor(*it);
			poco_assert(c != SG_BORDER);
			if (c == SG_BLACK)
				black += 2;
			else if (c == SG_WHITE)
				white += 2;
			else if (marked[*it])
				continue;

			// flood

			SgPoint stack[SG_MAXPOINT];
			int k = 0;
			int num = 0;
			bool whiteNb = false, blackNb = false;
			marked[*it] = true;
			stack[k++] = *it;

			SgArrayList<SgPoint, 4> nb;
			while (k > 0) {
				SgPoint cur = stack[--k];
				nb.Clear();
				//nb.reserve(4);
				getGoNeighbors(*board, *it, nb);
				for (SgArrayList<SgPoint, 4>::Iterator j(nb); j; ++j) {
					if (board->GetColor(*j) == SG_BLACK)
						blackNb = true;
					else if (board->GetColor(*j) == SG_WHITE)
						whiteNb = true;
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

			if (blackNb)
				black += num;
			if (!whiteNb)
				black += num;
			if (whiteNb)
				white += num;
			if (!blackNb)
				white += num;
		}
		if (Util::UctDebugEnabled() && false) {
			fprintf(Util::LogFile(),
					"simulation result: %.1f, black = %.1f, white = %.1f\n",
					(black - white) / 2.0, black / 2.0, white / 2.0);
		}
		return (black - white) / 2.0;
	}

	template<typename BOARD>
	BoardState evaluateState(BOARD* board) {
		COUNT res = evaluate(board);
		if (res > komi)
			return BLACK_WIN;
		else if (res < komi)
			return WHITE_WIN;
		else
			return DRAW;
	}

	void testRun(int num, ostream& out) {
		int k = 0;
		for (int i = 0; i < num; ++i) {
			cerr << "i = " << i << endl;
			SgPoint move = genMove();
			if (move == SG_PASS) {
				out << "PASS MOVE\n" << endl;
				if (++k == 2) {
					out << "Game ends in two passes\n";
					break;
				}
			} else
				k = 0;
			execute(move, i % 2 == 0 ? SG_BLACK : SG_WHITE);

			out << "i = " << i << endl;
			board.printBoard(out);
		}
	}

	COUNT resultWeight(double res) {
		res -= komi;
		return 1.0 + (res > 0 ? res : -res) * largeWinBonus;
	}
};

#endif
