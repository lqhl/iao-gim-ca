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

using Poco::RWLock;
using Poco::Thread;
using Poco::Random;

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

	UCTSearchRunner(SuperGoGame* game);

	void searchBoard(int timeLimit, int numPlayOut, GoBoard* board,
			UCTTree* tree, RWLock* treeLock);

	// search in tree, from the root to a terminal node
	// save the result in seq
	void searchInTree(UCTTree* tree, RWLock* treeLock, GoBoard* board, vector<
			GoPlayerMove>& seq);

	// returns the result of playOut
	// from the state of the board, start MC-simulation
	// saves the simulation sequence in seq
	int playOut(GoBoard* board, vector<GoPlayerMove>& seq);

	GoPlayerMove generateMove(GoUctBoard* board, SgBlackWhite toPlay);

	// seqIn: sequence in the tree, seqOut: seqOut of the tree
	// update the tree according the simulation result
	void upateStat(UCTTree* tree, RWLock* treeLock, int result, vector<
			GoPlayerMove>& seqIn, vector<GoPlayerMove>& seqOut);

	/* the following four functions select children in tree based on different heuristics*/
	int selectChildrenUCT(UCTTree* tree, int n);

	int selectChildrenCount(UCTTree* tree, int n);

	int selectChildrenMEAN(UCTTree* tree, int n);

	int selectChildrenRAVE(UCTTree* tree, int n);

	virtual void run();

	void start();

	/* for playing out policy*/

	bool generateNakadeMove(GoUctBoard* board, vector<SgPoint>& moves);

	bool generateAtariCaptureMove(GoUctBoard* board, vector<SgPoint>& moves);

	bool generateAtariDefenseMove(GoUctBoard* board, vector<SgPoint>& moves);

	bool generateLowLibertyMove(GoUctBoard* board, vector<SgPoint>& moves);

	bool generatePatternMove(GoUctBoard* board, vector<SgPoint>& moves);

	void generateCaptureMoves(GoBoard* board, vector<GoPlayerMove>& moves);

	SgPoint generateRandomMoves(GoUctBoard* board);

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

	void captureCandidatesUpdateOnPlay(GoBoard* board);

	void randomMoveUpdateOnPlay(GoBoard* board);

};

#endif
