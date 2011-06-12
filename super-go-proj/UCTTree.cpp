#include "UCTTree.h"
#include "Poco/Hashmap.h"
#include "SuperGoGame.h"
#include <assert.h>

using Poco::HashMap;

UCTTree::UCTTree(SuperGoGame* game, int numNodes) {
	this->game = game;
	this->numNodes = numNodes;
	node = new UCTNode[numNodes];
	freeList.clear();
	freeList.reserve(numNodes);
	for(int i=0; i<numNodes; ++i) {
		freeList.push_back(i);
		node[i].level = RESERVED;
	}

	UCTNode* n = allocateNode();
	n->level = 0;
}


void UCTTree::prune(int count) {
	for(int i=0; i<numNodes; ++i) {
		if (node[i].level != RESERVED && node[i].visitCount < count) {
			node[i].level = RESERVED;
			freeList.push_back(i);
		}
	}
}


void UCTTree::deleteSubtree(UCTNode* n) {
	for(int i=0; i<numNodes; ++i) {
		deleteSubtree(n->children[i]);
		recycleNode(n);
	}
}

// result is #black-stone - #white-stone
void UCTTree::updateStat(vector<SgPoint>& seqIn, vector<SgPoint>& seqOut, COUNT result) {
	// TODO assert the color

	HashMap<int, int> moveDepth(game->BOARD_SIZE * game->BOARD_SIZE * 2);
	int k = 1;
	vector<SgPoint>::iterator it;
	for(it = seqIn.begin(); it != seqIn.end(); ++it) {
		if (moveDepth.find(*it) == moveDepth.end()) {
			moveDepth.insert(HashMap<int, int>::PairType(*it, k));
		}
		++k;
	}
	for(it = seqOut.begin(); it != seqOut.end(); ++it) {
		if (moveDepth.find(*it) == moveDepth.end()) {
			moveDepth.insert(HashMap<int, int>::PairType(*it, k));
		}
		++k;
	}


	COUNT delta = 0.5;
	if (result > game->komi) delta = 1;
	else if (result < game->komi) delta = 0;

	// from root go down
	UCTNode* p = rootNode();
	p->updateVisit(delta);
	for(vector<int>::iterator it = seqIn.begin(); it != seqIn.end(); ++it) {
		vector<UCTNode*>& children = p->children;
		vector<UCTNode*>::iterator j = children.begin();
		UCTNode* next = NULL;


		for(; j != children.end(); ++j) {
			UCTNode& c = *(*j);
			if (c.move == *it) {
				next = &c;
				c.updateVisit(delta);
			}
			else {
				HashMap<int, int>::Iterator d = moveDepth.find(c.move);
				if (d != moveDepth.end())
					c.updateRave(2 - d->second / k, delta);
			}
		}
		poco_assert(next != NULL);

		p = next;
	}

}

void UCTTree::update(SgPoint move) {
	UCTNode* next = NULL;
	UCTNode* n = rootNode();
	vector<UCTNode*>& children = n->children;
	for(vector<UCTNode*>::iterator it = children.begin(); it != children.end(); ++it) {
		if ((*it)->move == move) {
			next = *it;
			continue;
		}
		deleteSubtree(*it);
	}

	if (next == NULL) {
		poco_assert(n->level % 2 != game->player);
		next = allocateNode();
		next->level = n->level + 1;
	}
	recycleNode(n);

	root = next - node;
}
