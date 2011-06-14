#include "GoBook.h"
#include "SgPoint.h"
#include "GoBoard.h"
#include "SgBlackWhite.h"
#include "SgMove.h"
#include <fstream>
#include <string>
#include <vector>

using namespace std;

SgPoint point(int pos, char ch) {
	int col = 0;
	switch(ch) {
		case 'A':
			col = 1;
			break;
		case 'B':
			col = 2;
			break;
		case 'C':
			col = 3;
			break;
		case 'D':
			col = 4;
			break;
		case 'E':
			col = 5;
			break;
		case 'F':
			col = 6;
			break;
		case 'G':
			col = 7;
			break;
		case 'H':
			col = 8;
			break;
		case 'J':
			col = 9;
			break;
		case 'K':
			col = 10;
			break;
		case 'L':
			col = 11;
			break;
		case 'M':
			col = 12;
			break;
		case 'N':
			col = 13;
			break;
	}
	return SgPointUtil::Pt(col, pos);
}

GoBook::GoBook() {
	ifstream fin("book.dat");
	string str;
	int size = 0;
	GoBook::capacity = 0;
	while(!fin.eof()) {
		fin >> str;
		// 1. This is a size number.
		if(str[0] >= '0' && str[0] <= '9') {
			size = 0;
			for(int i = 0; i < str.size(); ++i) {
				size = size * 10 + (str[i] - '0');
			}
		}
		else {
			// 2. This is a pattern element.
			if(str[0] >= 'A' && str[0] <= 'Z') {
				char ch = str[0];
				int pos = 0;
				for(int i = 1; i < str.size(); ++i) {
					pos = pos * 10 + (str[i] - '0');
				}
				if(size  == 19) {
					switch(pos) {
						case 5: case 6: case 7:
							pos = 5;
							break;
						case 8: case 9:
							pos = 6;
							break;
						case 10:
							pos = 7;
							break;
						case 11: case 12:
							pos = 8;
							break;
						case 13: case 14: case 15:
							pos = 9;
							break;
						case 16: case 17: case 18: case 19:
							pos = pos - 6;
							break;
						default:
							break;
					}
					switch(ch) {
						case 'E': case 'F': case 'G':
							ch = 'E';
							break;
						case 'H': case 'J':
							ch = 'F';
							break;
						case 'K':
							ch = 'G';
							break;
						case 'L': case 'M':
							ch = 'H';
							break;
						case 'N': case 'O': case 'P':
							ch = 'J';
							break;
						case 'Q': case 'R': case 'S': case 'T':
							ch = ch - 6;
							break;
						default:
							break;
					}
					SgPoint pt = point(pos, ch);
					patterns[GoBook::capacity].push_back(pt);
				}
			}
			else {
				// 3. This is a delimiter followed by a move.
				if(str[0] == '|' && str.size() == 1) {
					string res;
					fin >> res;
					int pos = 0;
					char ch = res[0];
					for(int i = 1; i < res.size(); ++i) {
						pos = pos * 10 + (res[i] - '0');
					}
					if(size  == 19) {
						switch(pos) {
							case 5: case 6: case 7:
								pos = 5;
								break;
							case 8: case 9:
								pos = 6;
								break;
							case 10:
								pos = 7;
								break;
							case 11: case 12:
								pos = 8;
								break;
							case 13: case 14: case 15:
								pos = 9;
								break;
							case 16: case 17: case 18: case 19:
								pos = pos - 6;
								break;
						}
						switch(ch) {
							case 'E': case 'F': case 'G':
								ch = 'E';
								break;
							case 'H': case 'J':
								ch = 'F';
								break;
							case 'K':
								ch = 'G';
								break;
							case 'L': case 'M':
								ch = 'H';
								break;
							case 'N': case 'O': case 'P':
								ch = 'J';
								break;
							case 'Q': case 'R': case 'S': case 'T':
								ch = ch - 6;
								break;
						}
						SgPoint strategy = point(pos, ch);
						++GoBook::capacity;
					}
				}
				else cout << "Load error!" << endl;
			}
		}
	}
	fin.close();

	cerr << GoBook::capacity << endl;
	cerr << moves.size() << endl;
}


SgPoint GoBook::matchBook(const GoBoard& board, SgBlackWhite color) {
	int cnt =  0;
	for(int i = 1; i < 13; ++i) {
		for(int j = 1; j < 13; ++j) {
			SgPoint pt = SgPointUtil::Pt(i, j);
			if(board.GetColor(pt) == SG_BLACK || board.GetColor(pt) == SG_WHITE)
				++cnt;
		}
	}


	for(int i = 0; i < GoBook::capacity; ++i) {
		if(GoBook::patterns[i].size() != cnt)
			continue;

		bool success = true;
		for(int j = 0; j < GoBook::patterns[i].size(); ++j) {
			if(j % 2 == 0 && board.GetColor(patterns[i][j]) == SG_BLACK) {
			
			}
			else {
				success = false;
				break;
			}
			if(j % 2 == 1 && board.GetColor(patterns[i][j]) == SG_WHITE) {
			
			}
			else {
				success = false;
				break;
			}
		}
		if(success) return moves[i];
	}
	return SG_NULLMOVE;
}




GoBook::~GoBook() {
	moves.clear();
}