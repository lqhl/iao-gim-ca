#ifndef UCT_TREE_H
#define UCT_TREE_H

#include "UCTNode.h"
#include "Board.h"
#include <vector>

using std::vector;

class SuperGoGame;

class UCTTree {
public:
	UCTNode *node;

	int numNodes;

	int root;

	vector<int> freeList;

	SuperGoGame* game;

	UCTTree(SuperGoGame* game, int numNodes);

	UCTNode* allocateNode();

	void prune(int count);

	UCTNode* rootNode();

	void update(MOVE move); // update the tree by executing a move

	void deleteSubtree(int r);

	void recycleNode(int n);

	// after play in tree, if win-loss is not proven, we need to expand a node
	// initialize the statistics based on patterns
	void expandNode(int n, Board* board);
};
#endif