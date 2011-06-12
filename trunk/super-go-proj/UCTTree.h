#ifndef UCT_TREE_H
#define UCT_TREE_H

#include "UCTTree.h"
#include "UCTNode.h"
#include "GoBoard.h"
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

	bool tryExpand(GoBoard* board, UCTNode* node) {
		if (node->fullyExpanded)
			return true;
		bool asChild[SG_MAXPOINT];
		fill(asChild, asChild + SG_MAXPOINT, false);
		vector<UCTNode*>& children = node->children;
		for (vector<UCTNode*>::iterator it = children.begin(); it != children.end(); ++it) {
			asChild[(*it)->move] = true;
		}

		children.reserve(board->Size() * board->Size() - node->level);

		for (GoBoard::Iterator it(*board); it; ++it) {
			if (!asChild[*it] && board->GetColor(*it) == SG_EMPTY
				&& board->IsLegal(*it)) {
					UCTNode* n = allocateNode();
					if (n == NULL) return false;

					n->level = node->level+1;
					n->move = *it;
					preprocess(board, n);
					children.push_back(n);
			}
		}
		node->fullyExpanded = true;
		return true;
	}

	void preprocess(GoBoard* board, UCTNode* node) {
		poco_assert(board->Size() == UctPatterns::BOARD_SIZE);
		SgPoint p = node->move;

		if (UctPatterns::Line(p) == 3)
			node->updateRave(6.67, 1.0);
		if (UctPatterns::Line(p) == 1)
			node->updateRave(6.67, -1.0);
		if (UctPatterns::matchAny(board, p))
			node->updateRave(5.0, 1.0);
		if (UctPatterns::match2BadKogeima(board, p))
			node->updateRave(2.5, -1.0);
		if (UctPatterns::match2EmptyTriangle(board, p))
			node->updateRave(5, -1.0);

	}
};
#endif
