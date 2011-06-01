#ifndef UCT_NODE_H
#define UCT_NODE_H

#include <vector>
#include "SuperGo.h"

using std::vector;

#define UNUSED -1
#define RESERVED -2

struct UCTNode {
public:

	// we'll construct a large array of UCTNode, and this vector will point to the children in the array
	vector<int> children; 

	COUNT raveCount, visitCount;

	VALUE raveValue, visitValue;

	int level; // the first move at level 0, second at level 1, etc.

	MOVE move;

	void clear();

	UCTNode();

};
#endif