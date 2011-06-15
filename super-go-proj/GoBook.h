#ifndef GO_BOOK_H
#define GO_BOOK_H

#include "SgBlackWhite.h"
#include "GoBoard.h"
#include "SgPoint.h"
#include <string>
#include <vector>

using namespace std;

class GoBook 
{
public:
	struct localPattern {
		int row, col;
		char pattern[20][20];
		SgPoint strategy;
	};

	GoBook();
	SgPoint matchBook(const GoBoard& board, SgBlackWhite color);
	double evaluate(const GoBoard& board, SgBlackWhite color, SgPoint point);
	~GoBook();

private:

	vector<SgPoint> patterns[3000];
	vector<SgPoint> moves;
	vector<localPattern> local;
	int capacity;

};

#endif