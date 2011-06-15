#ifndef UCT_NODE_H
#define UCT_NODE_H

#include <vector>
#include <algorithm>
#include "SuperGo.h"
#include "UCTPatterns.h"
using std::vector;
using std::fill;

#define UNUSED -1
#define RESERVED -2

class UCTNode {
public:

	// we'll construct a large array of UCTNode, and this vector will point to the children in the array
	vector<UCTNode*> children;

	COUNT raveCount, visitCount;

	VALUE raveValue, visitValue;

	int level; // the first move at level 0, second at level 1, etc.

	SgPoint move;

	BoardState state;

	void clear() {
		this->raveCount = 0;
		this->raveValue = 0.5;
		this->visitCount = 0;
		this->visitValue = 0.5;
		this->level = UNUSED;
		this->children.clear();
		this->fullyExpanded = false;
		state = NOT_PROVEN;
	}

	UCTNode() {
		clear();
	}
	bool hasChildren() {
		return !children.empty();
	}

	void updateVisit(COUNT weight, VALUE value) {
		VALUE t = visitValue * visitCount + value * weight;
		visitCount += weight;
		visitValue = t / visitCount;
	}

	void updateRave(COUNT weight, VALUE value) {
		//poco_assert(raveCount <= 50);
		VALUE t = raveValue * raveCount + value * weight;
		raveCount += weight;
		raveValue = t / raveCount;
	}

	bool fullyExpanded;

	void print(FILE* file);

};
#endif
