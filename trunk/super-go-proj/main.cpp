#include <fstream>
#include "GoBoard.h"
#include "SgPoint.h"
#include "SgBlackWhite.h"

using namespace std;

void testGoBoard() {
	ofstream out("sb.txt");
	GoBoard board(13);
	using namespace SgPointUtil;
	board.Play(Pt(1, 1), SG_BLACK);
	board.printAll(out);
	board.Play(Pt(1, 2), SG_WHITE);
	board.printAll(out);
	board.Play(Pt(2, 1), SG_WHITE);
	board.printAll(out);
}

int main() {
	testGoBoard();
	return 0;
}
