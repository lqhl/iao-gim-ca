#ifndef UCT_SEARCH_RUNNER_H
#define UCT_SEARCH_RUNNER_H

#include <iostream>
#include "Poco/Thread.h"
#include "Poco/Runnable.h"
#include "Poco/RWLock.h"
#include "PlayOutBoard.h"
#include "Board.h"
#include "UCTTree.h"

using Poco::RWLock;

class SuperGoGame;

class UCTSearchRunner;

typedef int (UCTSearchRunner::*SearchGenMove)(int n);

//typedef int (UCTSearchRunner::*PlayOutNextMove)(int n);


class UCTSearchRunner: public Poco::Runnable
{
public:
	SuperGoGame* game;

	PlayOutBoard playOutBoard;

	SearchGenMove selectChildren;

	SearchGenMove playOutNextMove;

	
	//UCTSearchRunner();

	UCTSearchRunner(SuperGoGame* game);

	void searchBoard(int timeLimit, Board* board, UCTTree* tree, RWLock* treeLock);

	


	virtual void run();


};

#endif