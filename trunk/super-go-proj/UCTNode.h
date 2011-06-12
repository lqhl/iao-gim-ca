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

	void clear();

	UCTNode();

	bool hasChildren() {
		return !children.empty();
	}

	void updateVisit(VALUE value) {
		visitCount += 1;
		visitValue += value;
	}

	void updateRave(COUNT weight, VALUE value) {
		raveCount += weight;
		raveValue += value;
	}

	bool fullyExpanded;

};
#endif
