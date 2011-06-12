#include "UCTNode.h"

void UCTNode::clear() {
	this->raveCount = 0;
	this->raveValue = 0;
	this->visitCount = 0;
	this->visitValue = 0;
	this->level = UNUSED;
	this->children.clear();
	this->fullyExpanded = false;
}

UCTNode::UCTNode() {
	clear();
}
