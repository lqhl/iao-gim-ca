#include <fstream>
#include "GoBoard.h"
#include "SgPoint.h"
#include "SgBlackWhite.h"
#include "GoUctBoard.h"
#include "UCTPatterns.h"
#include "TestMCSimulation.h"
#include "util/util.h"
#include "testHashTable.h"
#include "SuperGoGame.h"
#include "SuperGo.h"
#include <cstdio>

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


void testPatternMatch() {
	GoBoard* board = new GoBoard(13);
	GoBook* book = new GoBook();
	board->Play(SgPointUtil::Pt(10, 10), SG_BLACK);
	board->Play(SgPointUtil::Pt(11, 8), SG_WHITE);
	board->printBoard(cerr);
	book->evaluate(*board, SG_BLACK, SgPointUtil::Pt(11, 6));
}

int main() {
	 return pk_main();

	//testPatternMatch();
	//system("pause");
	// !!! IMPORTANT
	remove("super-go-log.txt");
	Util::init("super-go.config");
	UctPatterns::init();


	SuperGoGame* game = new SuperGoGame();
	game->init();
	UCTTree::rand.seed(100);
	ofstream out("super-go-test.txt");
	game->testRun(1000, out);
	


//	testHashTable();
	//testVector();

//	Logger::get("SimulationLogger").error("Hello World! Error Just Occurred.");
	
	//testGoUctBoard();
	//testMCSimulation2();

	//UctPatterns::test();
}
