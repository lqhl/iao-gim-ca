#ifndef UCT_SEARCH_RUNNER_H
#define UCT_SEARCH_RUNNER_H

#include <iostream>
#include "Poco/Thread.h"
#include "Poco/Runnable.h"
#include "Poco/RWLock.h"
#include "Poco/Debugger.h"
#include "GoBoard.h"
#include "GoUctBoard.h"
#include "UCTTree.h"
#include "GoPlayerMove.h"
#include "Poco/Random.h"
#include "Poco/Logger.h"

using Poco::RWLock;
using Poco::Thread;
using Poco::Random;
using Poco::Logger;
class SuperGoGame;

class UCTSearchRunner;

typedef int (UCTSearchRunner::*SearchGenMove)(UCTTree* tree, int n);

//typedef int (UCTSearchRunner::*PlayOutNextMove)(int n);


class UCTSearchRunner: public Poco::Runnable {
public:
	SuperGoGame* game;

	Thread thread;

	GoUctBoard playOutBoard;

	SearchGenMove selectChildren;

	SearchGenMove playOutNextMove;

	double ravePara1, ravePara2, CUCT;

	Logger& uctLogger;

	// just for debugging usage
	UCTSearchRunner(GoBoard& board) : playOutBoard(board), uctLogger(Logger::get("UctLogger")) {
		cerr << "Debugging UCTSearchRunner\n";

		rand.seed(0);
	}


	UCTSearchRunner(SuperGoGame* game);

	void searchBoard(int timeLimit, int numPlayOut, GoBoard* board,
			UCTTree* tree, RWLock* treeLock);

	// search in tree, from the root to a terminal node
	// save the result in seq
	bool searchInTree(UCTTree* tree, RWLock* treeLock, GoBoard* board, vector<
			SgPoint>& seq);

	// returns the result of playOut
	// from the state of the board, start MC-simulation
	// saves the simulation sequence in seq
	COUNT playOut(GoBoard* board, vector<SgPoint>& seq);

	inline GoPlayerMove generateMove() {
		return generateMove(&playOutBoard, playOutBoard.ToPlay());
	}

	void play(GoPlayerMove move) {
		poco_assert(move.Color() == playOutBoard.ToPlay());

		playOutBoard.Play(move.Point());
	}

	GoPlayerMove generateMove(GoUctBoard* board, SgBlackWhite toPlay);

	// seqIn: sequence in the tree, seqOut: seqOut of the tree
	// update the tree according the simulation result
	void upateStat(UCTTree* tree, RWLock* treeLock, COUNT result, vector<
			SgPoint>& seqIn, vector<SgPoint>& seqOut);

	/* the following four functions select children in tree based on different heuristics*/
	UCTNode* selectChildrenUCT(UCTTree* tree, UCTNode* node);

	UCTNode* selectChildrenCount(UCTTree* tree, UCTNode* node);

	UCTNode* selectChildrenMEAN(UCTTree* tree, UCTNode* node);

	UCTNode* selectChildrenRAVE(UCTTree* tree, UCTNode* node);

	virtual void run();

	void start();

	/* for playing out policy*/

	bool generateNakadeMove(GoUctBoard* board, vector<SgPoint>& moves);

	bool generateAtariCaptureMove(GoUctBoard* board, vector<SgPoint>& moves);

	bool generateAtariDefenseMove(GoUctBoard* board, vector<SgPoint>& moves);

	bool generateLowLibertyMove(GoUctBoard* board, vector<SgPoint>& moves);

	bool generatePatternMove(GoUctBoard* board, vector<SgPoint>& moves);

	void generateCaptureMoves(GoUctBoard* board, vector<SgPoint>& moves);

	SgPoint generateRandomMove(GoUctBoard* board);

	void selectLibertyMove(GoUctBoard* board, vector<SgPoint>& moves, SgPoint block);


	/* for random selection */
	Random rand;

	SgPoint randomSelect(vector<SgPoint>& moves) {
		return !moves.empty() ? moves[rand.next(moves.size())] : SG_NULLMOVE;
	}

	/* candidates for capture moves */
	vector<SgPoint> capture_candidates;

	/* candidates for random moves*/
	vector<SgPoint> random_candidates;

	void captureCandidatesUpdateOnPlay(GoUctBoard* board);

	void randomMoveUpdateOnPlay(GoUctBoard* board);

	void startPlayOut(GoBoard* board);

	void PushBack(vector<SgPoint>& moves, SgPoint p) {
		if (playOutBoard.IsLegal(p))
			moves.push_back(p);
	}

	void genNakade(SgPoint p, GoUctBoard* board, vector<SgPoint>& moves);

	void GeneratePatternMove(GoUctBoard* board, vector<SgPoint>& moves, SgPoint p);

	void GeneratePatternMove2(GoUctBoard* board, vector<SgPoint>& moves, SgPoint p);

};

#endif
