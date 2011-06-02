#include "UCTTree.h"
#include <assert.h>

UCTTree::UCTTree(SuperGoGame* game, int numNodes) {
	this->game = game;
	this->numNodes = numNodes;
	node = new UCTNode[numNodes];
	for(int i=0; i<numNodes; ++i) {
		freeList.push_back(i);
		node[i].level = RESERVED;
	}
}

UCTNode* UCTTree::allocateNode() {
	assert(freeList.size() > 0);
	int i = freeList.back();
	freeList.pop_back();
	assert(node[i].level == RESERVED);

	node[i].clear();
	return &node[i];
}

void UCTTree::prune(int count) {
	for(int i=0; i<numNodes; ++i) {
		if (node[i].level != RESERVED && node[i].visitCount < count) {
			node[i].level = RESERVED;
			freeList.push_back(i);
		}
	}
}

void UCTTree::expandNode(int n, Board* board) {

}

UCTNode* UCTTree::rootNode() {
	return &node[root];
}

void UCTTree::update(MOVE move) {
	UCTNode* n = rootNode();
	for(int i=0; i<numNodes; ++i) {
		int k = n->children[i];
		if (node[k].move != move) {
			deleteSubtree(k);
		}
	}
}


void UCTTree::deleteSubtree(int r) {
	for(int i=0; i<numNodes; ++i) {
		deleteSubtree(node[r].children[i]);
		recycleNode(r);
	}
}

void UCTTree::recycleNode(int n) {
	assert(node[n].level != RESERVED && node[n].level != UNUSED);
	node[n].level = RESERVED;
	freeList.push_back(n);
}