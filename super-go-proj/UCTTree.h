#ifndef UCT_TREE_H
#define UCT_TREE_H

#include "UCTTree.h"
#include "UCTNode.h"
#include "GoBoard.h"
#include "Poco/Random.h"
#include <vector>

using std::vector;
using Poco::Random;

class SuperGoGame;

class UCTTree {
public:
	UCTNode *node;

	// tree will be accessed by one thread at a time, so we share a random generator
	static Random rand;

	int numNodes;

	int root;

	vector<int> freeList;

	SuperGoGame* game;

	UCTTree(SuperGoGame* game, int numNodes);

	UCTNode* allocateNode() {
		//poco_assert(freeList.size() > 0);
		if (freeList.empty()) return NULL;
		int i = freeList.back();
		freeList.pop_back();
		poco_assert(node[i].level == RESERVED);

		node[i].clear();
		return &node[i];
	}


	void prune(int count);

	UCTNode* rootNode() {
		return &node[root];
	}

	void update(SgPoint move); // update the tree by executing a move

	void deleteSubtree(UCTNode* n);

	void recycleNode(UCTNode* n) {
		poco_assert(n->level != RESERVED && n->level != UNUSED);
		n->level = RESERVED;
		freeList.push_back(n - node);
	}

	void updateStat(vector<SgPoint>& seqIn, vector<SgPoint>& seqOut, COUNT result);

	inline UCTNode* getNode(int ind) {
		UCTNode* node = &node[ind];
		poco_assert(node->level >= 0);
		return node;
	}

	bool tryExpand(GoBoard* board, UCTNode* node, SuperGoGame* game, BoardState& state);

	void preprocess(GoBoard* board, UCTNode* node) {
		poco_assert(board->Size() == UctPatterns::BOARD_SIZE);
		SgPoint p = node->move;

		bool blackMove = board->ToPlay() == SG_BLACK;
		if (UctPatterns::Line(p) == 3)
			node->updateRave(6.67, blackMove ? 1.0 : 0.0);
		if (UctPatterns::Line(p) == 1)
			node->updateRave(6.67, blackMove ? 0.0 : 1.0);
		if (UctPatterns::matchAny(board, p))
			node->updateRave(5.0, blackMove ? 1.0 : 0.0);
		if (UctPatterns::match2BadKogeima(board, p))
			node->updateRave(2.5, blackMove ? 0.0 : 1.0);
		if (UctPatterns::match2EmptyTriangle(board, p))
			node->updateRave(5, blackMove ? 0.0 : 1.0);

	}
};
#endif
