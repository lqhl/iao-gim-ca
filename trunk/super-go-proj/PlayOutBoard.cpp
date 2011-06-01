#include "PlayOutBoard.h"

PlayOutBoard::PlayOutBoard(int size) : boardSize(size) {
	moves = new MOVE[(size+2) * (size+2)];
}

void PlayOutBoard::execute(MOVE move) {

}

void PlayOutBoard::rollBack(MOVE move) {

}

PlayOutBoard::~PlayOutBoard() {
	delete[] moves;
}