#include "UCTNode.h"

void UCTNode::print(FILE* file) {
	fprintf(file, "move=(%d %d) visit=(%.1f %.3f), RAVE=(%.1f %.3f)", Row(move), Col(move), visitCount, visitValue, raveCount, raveValue);
}


