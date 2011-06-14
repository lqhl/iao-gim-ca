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
	GoBook();
	SgPoint matchBook(const GoBoard& board, SgBlackWhite color);
	~GoBook();

private:
	vector<SgPoint> patterns[5000];
	vector<SgPoint> moves;
	int capacity;

};

#endif