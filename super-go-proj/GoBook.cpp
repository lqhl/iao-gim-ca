#include "GoBook.h"
#include "SgPoint.h"
#include "GoBoard.h"
#include "SgBlackWhite.h"
#include "SgMove.h"
#include <fstream>
#include <string>


using namespace std;

string convert(int num) {
	string ret;
	int temp = num;
	if(temp < 10) {
		char tmp[2];
		tmp[0] = (temp + '0');
		tmp[1] = '\0';
		ret = tmp;
	}
	else {
		char tmp[3];
		tmp[0] = '1';
		tmp[1] = ((temp % 10) + '0');
		tmp[2] = '\0';
		ret = tmp;
	}
	return ret;
}

string convert(char ch) {
	string ret;
	char temp[2];
	temp[0] = ch;
	temp[1] = '\0';
	ret = temp;
	return ret;
}

SgPoint point(string move) {
	int row = 0;
	int col = 0;
	char ch = move[0];
	for(int i = 1; i < move.size(); ++i) {
		row = row * 10 + (move[i] - '0');
	}
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
	return SgPointUtil::Pt(col, row);
}

GoBook::GoBook() {
	ifstream fin("book.dat");
	string str;
	int size = 0;
	string parse;
	string move;
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
					if(!parse.empty()) parse += " ";
					parse = parse + convert(ch) + convert(pos);
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
						move = convert(ch) + convert(pos);

						patterns.push_back(parse);
						moves.push_back(move);
						parse.clear();
					}
				}
				else cout << "Load error!" << endl;
			}
		}
	}
	fin.close();

	cout << patterns.size() << endl;
	cout << moves.size() << endl;
}


SgPoint GoBook::matchBook(const GoBoard& board, SgBlackWhite color) {
	string tmp;
	for(int i = 1; i < 13; ++i) {
		for(int j = 1; j < 13; ++j) {
			SgPoint p = SgPointUtil::Pt(i, j);
			if(board.GetColor(p) == color) {
				if(!tmp.empty())
					tmp += " ";
				tmp += SgPointUtil::PointToString(p);
			}
		}
	}
	string result;
	for(int i = 0; i < patterns.size(); ++i) {
		if(patterns[i].rfind(tmp, 0) != string::npos) {
			result = moves[i];
		}
	}
	if(result.empty())
		return SG_NULLMOVE;
	return point(result);
}




GoBook::~GoBook() {
	patterns.clear();
	moves.clear();
}