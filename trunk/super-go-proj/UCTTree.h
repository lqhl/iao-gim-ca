#ifndef UCT_TREE_H
#define UCT_TREE_H

#include "UCTNode.h"
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
};
#endif