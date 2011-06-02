#ifndef SUPER_GO_GAME_H
#define SUPER_GO_GAME_H

#include "SuperGo.h"
#include "UCTSearchRunner.h"
#include "Board.h"
#include "UCTTree.h"
#include "Poco/RWLock.h"
#include <vector>
#include <string>

using std::string;
using std::vector;
using Poco::RWLock;


class SuperGoGame;

typedef void (SuperGoGame::*GenMoveFunc)();


class SuperGoGame {
public:
	static const int BOARD_SIZE = 13;
	static const int NS = BOARD_SIZE+1, WE = 1;

	static const int NUM_THREAD = 2;

	static const int TIME_LIMIT = 10000; // in milliseconds

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

	vector<UCTSearchRunner> workers;

	RWLock* treeLock;

	UCTTree* tree;

	int player, opponent;

	Board board;

	SuperGoGame(const string& fileName);

	void setPlayer(int player);

	void init();

	// given a move, generate a new move
	MOVE genMoveUCT(MOVE move);


};

#endif