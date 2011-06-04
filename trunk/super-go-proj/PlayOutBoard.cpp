#include "PlayOutBoard.h"

PlayOutBoard::PlayOutBoard(int size) : boardSize(size) {
	moves = new MOVE[(size+2) * (size+2)];
}

void PlayOutBoard::execute(MOVE move, COLOR toPlay) {

}

MOVE PlayOutBoard::nextMove(COLOR toPlay) {
	return -1;
}

void PlayOutBoard::rollBack(int level) {

}

PlayOutBoard::~PlayOutBoard() {
	delete[] moves;
}