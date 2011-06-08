#ifndef UCT_SEARCH_RUNNER_H
#define UCT_SEARCH_RUNNER_H

#include <iostream>
#include "Poco/Thread.h"
#include "Poco/Runnable.h"
#include "Poco/RWLock.h"
#include "GoBoard.h"
#include "GoUctBoard.h"
#include "UCTTree.h"

using Poco::RWLock;
using Poco::Thread;

class SuperGoGame;

class UCTSearchRunner;

typedef int (UCTSearchRunner::*SearchGenMove)(UCTTree* tree, int n);

//typedef int (UCTSearchRunner::*PlayOutNextMove)(int n);


class UCTSearchRunner: public Poco::Runnable
{
public:
	SuperGoGame* game;

	Thread thread;

	GoUctBoard playOutBoard;

	SearchGenMove selectChildren;

	SearchGenMove playOutNextMove;

	double ravePara1, ravePara2, CUCT;

	UCTSearchRunner(SuperGoGame* game);

	void searchBoard(int timeLimit, int numPlayOut, Board* board, UCTTree* tree, RWLock* treeLock);

	// search in tree, from the root to a terminal node
	// save the result in seq
	void searchInTree(UCTTree* tree, RWLock* treeLock, Board* board, vector<int>& seq);

	// returns the result of playOut
	// from the state of the board, start MC-simulation
	// saves the simulation sequence in seq
	int playOut(GoBoard* board, vector<int>& seq);

	// seqIn: sequence in the tree, seqOut: seqOut of the tree
	// update the tree according the simulation result
	void upateStat(UCTTree* tree, RWLock* treeLock, int result, vector<int>& seqIn, vector<int>& seqOut);
	

	/* the following four functions select children in tree based on different heuristics*/
	int selectChildrenUCT(UCTTree* tree, int n);

	int selectChildrenCount(UCTTree* tree, int n);

	int selectChildrenMEAN(UCTTree* tree, int n);

	int selectChildrenRAVE(UCTTree* tree, int n);

	virtual void run();

	void start();

};

#endif
