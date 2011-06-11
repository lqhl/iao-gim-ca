#include <fstream>
#include "GoBoard.h"
#include "SgPoint.h"
#include "SgBlackWhite.h"
#include "GoUctBoard.h"
#include "UCTPatterns.h"
#include "TestMCSimulation.h"
#include "util/util.h"
#include "testHashTable.h"

using namespace std;

void testGoBoard() {
	ofstream out("test-go-board.txt");
	GoBoard board(13);
	using namespace SgPointUtil;
	board.Play(Pt(1, 1), SG_BLACK);
	board.printAll(out);
	board.Play(Pt(1, 2), SG_WHITE);
	board.printAll(out);
	board.Play(Pt(2, 1), SG_WHITE);
	board.printAll(out);
}

void testGoUctBoard() {
	ofstream out("test-go-uct-board.txt");
	GoBoard board(13);
	using namespace SgPointUtil;
	board.Play(Pt(1, 1), SG_BLACK);
	board.printAll(out);
	board.Play(Pt(1, 2), SG_WHITE);
	board.printAll(out);
	board.Play(Pt(2, 1), SG_WHITE);
	board.printAll(out);

	GoUctBoard* uctBoard = new GoUctBoard(board);
	uctBoard->printAll(out);
}

int main() {
	// !!! IMPORTANT
	//Util::init("super-go.config");
	//UctPatterns::init();

	//testHashTable();
	testVector();

//	Logger::get("SimulationLogger").error("Hello World! Error Just Occurred.");
	
	//testGoUctBoard();
	//testMCSimulation2();

	//UctPatterns::test();
	return 0;
}
