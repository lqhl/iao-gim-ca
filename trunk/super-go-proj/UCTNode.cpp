#include "UCTNode.h"

void UCTNode::print(FILE* file) {
	fprintf(file, "move=(%d %d) visit=(%.1f %.3f), RAVE=(%.1f %.3f) ", Row(move), Col(move), visitCount, visitValue, raveCount, raveValue);
	if (this->state == BLACK_WIN) fprintf(file,  "state = BLACK_WIN");
	else if (this->state == WHITE_WIN) fprintf(file, "state = WHITE_WIN");
	else if (this->state == DRAW) fprintf(file, "state = DRAW");
	else if (this->state == NOT_PROVEN) fprintf(file, "state = NOT_PROVEN");
}


