#include "GoBook.h"
#include "SgPoint.h"
#include "GoBoard.h"
#include "SgBlackWhite.h"
#include "SgMove.h"
#include "Poco/Random.h"
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;
using Poco::Random;

inline SgPoint point(int pos, char ch) {
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
						moves.push_back(strategy);
						++GoBook::capacity;
					}
				}
				else cerr << "Load error!" << endl;
			}
		}
	}
	fin.close();

	cerr << GoBook::capacity << endl;
	cerr << moves.size() << endl;


	bool print = false;
	ifstream fin2("patternPlus.db");
	while(! fin2.eof() ) {
		GoBook::localPattern temp;
		fin2 >> temp.row;
		fin2 >> temp.col;
		//cerr << temp.row << " " << temp.col << endl;
		int Srow = 0;
		int Scol = 0;
		for(int i = 0; i < temp.row; ++i) {
			for(int j = 0; j < temp.col; ++j) {
				fin2 >> temp.pattern[i][j];
				//cerr << temp.pattern[i][j] << endl;
				if(temp.pattern[i][j] == '*') {
					Srow = i + 1;
					Scol = j + 1;
				}
			}
		}
		temp.strategy = SgPointUtil::Pt(Scol, Srow);
		GoBook::local.push_back(temp);

		if(print) {
			for(int i = 0; i < temp.row; ++i) {
				for(int j = 0; j < temp.col; ++j) {
					cerr << temp.pattern[i][j];
				}
				cerr << endl;
			}
			cerr << endl;
		}

		// Start transformation
		// 1. Rotate 90 anticlockwise
		GoBook::localPattern temp90;
		temp90.row = temp.col;
		temp90.col = temp.row;
		for(int i = 0; i < temp.col; ++i) {
			for(int j = 0; j < temp.row; ++j) {
				if(temp.pattern[j][temp.col - 1 - i] == '-')
					temp90.pattern[i][j] = '|';
				else {
					if(temp.pattern[j][temp.col - 1 - i] == '|')
						temp90.pattern[i][j] = '-';
					else
						temp90.pattern[i][j] = temp.pattern[j][temp.col - 1 - i];
				}
				if(temp90.pattern[i][j] == '*') {
					Srow = i + 1;
					Scol = j + 1;
				}
			}
		}
		temp90.strategy = SgPointUtil::Pt(Scol, Srow);
		GoBook::local.push_back(temp90);

		if(print) {
			for(int i = 0; i < temp90.row; ++i) {
				for(int j = 0; j < temp90.col; ++j) {
					cerr << temp90.pattern[i][j];
				}
				cerr << endl;
			}
			cerr << endl;
		}




		// 2. Rotate 180 anticlockwise
		GoBook::localPattern temp180;
		temp180.row = temp.row;
		temp180.col = temp.col;
		for(int i = 0; i < temp.row; ++i) {
			for(int j = 0; j < temp.col; ++j) {
				temp180.pattern[i][j] = temp.pattern[temp.row - i - 1][temp.col - 1 - j];
				if(temp180.pattern[i][j] == '*') {
					Srow = i + 1;
					Scol = j + 1;
				}
			}
		}
		temp180.strategy = SgPointUtil::Pt(Scol, Srow);
		GoBook::local.push_back(temp180);

		if(print) {
			for(int i = 0; i < temp180.row; ++i) {
				for(int j = 0; j < temp180.col; ++j) {
					cerr << temp180.pattern[i][j];
				}
				cerr << endl;
			}
			cerr << endl;
		}




		// 3. Rotate 270 anticlockwise
		GoBook::localPattern temp270;
		temp270.row = temp.col;
		temp270.col = temp.row;
		for(int i = 0; i < temp.col; ++i) {
			for(int j = 0; j < temp.row; ++j) {
				if(temp.pattern[temp.row - 1 - j][i] == '-')
					temp270.pattern[i][j] = '|';
				else {
					if(temp.pattern[temp.row - 1 - j][i] == '|')
						temp270.pattern[i][j] = '-';
					else
						temp270.pattern[i][j] = temp.pattern[temp.row - 1 - j][i];
				}
				if(temp270.pattern[i][j] == '*') {
					Srow = i + 1;
					Scol = j + 1;
				}
			}
		}
		temp270.strategy = SgPointUtil::Pt(Scol, Srow);
		GoBook::local.push_back(temp270);

		if(print) {
			for(int i = 0; i < temp270.row; ++i) {
				for(int j = 0; j < temp270.col; ++j) {
					cerr << temp270.pattern[i][j];
				}
				cerr << endl;
			}
			cerr << endl;
		}


	}
	cerr << local.size() << endl;
	fin2.close();
	cerr << "ok\n" << endl;
}

inline double ambient(SgPoint pt, const GoBoard& board) {
	double score = 0.0;
	int row = SgPointUtil::Row(pt);
	int col = SgPointUtil::Col(pt);
	for(int i = -2; i <= 2; ++i) {
		for(int j = -2; j <= 2; ++j) {
			if(i != 0 && j != 0) {
				SgPoint test = SgPointUtil::Pt(col+j, row+i);
				if(board.GetColor(test) == SG_BLACK) {
					score += 0.12;
				}
				else {
					if(board.GetColor(test) == SG_WHITE) {
						score -= 0.12;
					}
				}
			}
		}
	}
	return score;
}




SgPoint GoBook::matchBook(const GoBoard& board, SgBlackWhite color) {
	
	
	// 1. Match the Fuego Book
	int cnt =  0;
	for(int i = 1; i < 13; ++i) {
		for(int j = 1; j < 13; ++j) {
			SgPoint pt = SgPointUtil::Pt(i, j);
			if(board.GetColor(pt) == SG_BLACK || board.GetColor(pt) == SG_WHITE)
				++cnt;
		}
	}
	//cerr << "Count: " << cnt << endl;

	for(int i = 0; i < GoBook::capacity; ++i) {
		if(GoBook::patterns[i].size() != cnt)
			continue;

		bool success = true;
		for(int j = 0; j < GoBook::patterns[i].size(); ++j) {
			if(j % 2 == 0) {
				if(board.GetColor(patterns[i][j]) == SG_BLACK) {
				}
				else {
					success = false;
					break;
				}
			}
			if(j % 2 == 1) {
				if(board.GetColor(patterns[i][j]) == SG_WHITE) {
				}
				else {
					success = false;
					break;
				}
			}
		}
		if(success) {
			if(board.IsEmpty(moves[i])) {
				return moves[i];
			}
		}
	}

	// 2. Human matching
	int i = 4;
	int j = 10;

	Random rand;

	SgPoint leftLow = SgPointUtil::Pt(i, i);
	if(board.IsEmpty(leftLow)) {
		double s = ambient(leftLow, board);
		if((s <= 0.0 && color == SG_BLACK) || (s >= 0.0 && color == SG_WHITE))
			return leftLow;
		else {
			double r = rand.nextDouble();
			if(color = SG_WHITE) {
				s = 0.0 - s;
			}
			if(r < s) {
				return leftLow;
			}
		}
	}


	SgPoint leftUpper = SgPointUtil::Pt(i, j);
	if(board.IsEmpty(leftUpper)) {
		double s = ambient(leftUpper, board);
		if((s <= 0.0 && color == SG_BLACK) || (s >= 0.0 && color == SG_WHITE))
			return leftUpper;
		else {
			double r = rand.nextDouble();
			if(color = SG_WHITE) {
				s = 0.0 - s;
			}
			if(r < s) {
				return leftUpper;
			}
		}
	}


	SgPoint rightLow = SgPointUtil::Pt(j, i);
	if(board.IsEmpty(rightLow)) {
		double s = ambient(rightLow, board);
		if((s <= 0.0 && color == SG_BLACK) || (s >= 0.0 && color == SG_WHITE))
			return rightLow;
		else {
			double r = rand.nextDouble();
			if(color = SG_WHITE) {
				s = 0.0 - s;
			}
			if(r < s) {
				return rightLow;
			}
		}
	}


	SgPoint rightUpper = SgPointUtil::Pt(j, j);
	if(board.IsEmpty(rightUpper)) {
		double s = ambient(rightUpper, board);
		if((s <= 0.0 && color == SG_BLACK) || (s >= 0.0 && color == SG_WHITE))
			return rightUpper;
		else {
			double r = rand.nextDouble();
			if(color = SG_WHITE) {
				s = 0.0 - s;
			}
			if(r < s) {
				return rightUpper;
			}
		}
	}


	int mid = 7;
	SgPoint leftMid = SgPointUtil::Pt(i, mid);
	if(board.IsEmpty(leftMid)) {
		double s = ambient(leftMid, board);
		if(color == SG_BLACK) {
			if(rand.nextDouble() < s)
				return leftMid;
		}
		else {
			if(color == SG_WHITE) {
				s = 0.0 - s;
				if(rand.nextDouble() < s)
					return leftMid;
			}
		}
	}
	SgPoint upperMid = SgPointUtil::Pt(mid, j);
	if(board.IsEmpty(upperMid)) {
		double s = ambient(upperMid, board);
		if(color == SG_BLACK) {
			if(rand.nextDouble() < s)
				return upperMid;
		}
		else {
			if(color == SG_WHITE) {
				s = 0.0 - s;
				if(rand.nextDouble() < s)
					return upperMid;
			}
		}
	}
	SgPoint rightMid = SgPointUtil::Pt(j, mid);
	if(board.IsEmpty(rightMid)) {
		double s = ambient(rightMid, board);
		if(color == SG_BLACK) {
			if(rand.nextDouble() < s)
				return rightMid;
		}
		else {
			if(color == SG_WHITE) {
				s = 0.0 - s;
				if(rand.nextDouble() < s)
					return rightMid;
			}
		}
	}
	SgPoint lowerMid = SgPointUtil::Pt(mid, i);
	if(board.IsEmpty(lowerMid)) {
		double s = ambient(lowerMid, board);
		if(color == SG_BLACK) {
			if(rand.nextDouble() < s)
				return lowerMid;
		}
		else {
			if(color == SG_WHITE) {
				s = 0.0 - s;
				if(rand.nextDouble() < s)
					return lowerMid;
			}
		}
	}


	return SG_NULLMOVE;
}


inline bool match(const GoBoard& board, SgBlackWhite color, SgPoint point, GoBook::localPattern pattern) {
	int patternRow = SgPointUtil::Row(pattern.strategy) - 1;
	int patternCol = SgPointUtil::Col(pattern.strategy) - 1;
	int row = SgPointUtil::Row(point);
	int col = SgPointUtil::Col(point);
	SgBlackWhite myColor = color;
	SgBlackWhite yourColor = (color == SG_BLACK ? SG_WHITE:SG_BLACK);

	for(int i = 0; i < pattern.row; ++i) {
		for(int j = 0; j < pattern.col; ++j) {
			int colOffset = j - patternCol;
			int rowOffset = i - patternRow;
			switch(pattern.pattern[i][j]) {
				case '?': {
					break;
				}
				case '.': {
					if(col + colOffset <= 0 || col + colOffset > 13 || row + rowOffset <= 0 || row + rowOffset > 13)
						return false;
					SgPoint testPtDot = SgPointUtil::Pt(col+colOffset, row+rowOffset);
					if(!board.IsEmpty(testPtDot))
						return false;
					break;
				}
				case 'X': {
					if(col + colOffset <= 0 || col + colOffset > 13 || row + rowOffset <= 0 || row + rowOffset > 13)
						return false;
					SgPoint testPtBigX = SgPointUtil::Pt(col+colOffset, row+rowOffset);
					if(board.GetColor(testPtBigX) != yourColor)
						return false;
					break;
				}
				case 'O': {
					if(col + colOffset <= 0 || col + colOffset > 13 || row + rowOffset <= 0 || row + rowOffset > 13)
						return false;
					SgPoint testPtBigO = SgPointUtil::Pt(col+colOffset, row+rowOffset);
					if(board.GetColor(testPtBigO) != myColor)
						return false;
					break;
				}
				case 'x': {
					if(col + colOffset <= 0 || col + colOffset > 13 || row + rowOffset <= 0 || row + rowOffset > 13)
						return false;
					SgPoint testPtX = SgPointUtil::Pt(col+colOffset, row+rowOffset);
					if(board.GetColor(testPtX) != yourColor && !board.IsEmpty(testPtX))
						return false;
					break;
				}
				case 'o': {
					if(col + colOffset <= 0 || col + colOffset > 13 || row + rowOffset <= 0 || row + rowOffset > 13)
						return false;
					SgPoint testPtO = SgPointUtil::Pt(col+colOffset, row+rowOffset);
					if(board.GetColor(testPtO) != myColor && !board.IsEmpty(testPtO))
						return false;
					break;
				}
				case '|': {
					if(col + colOffset > 0 && col + colOffset <= 13)
						return false;
					break;
				}
				case '-': {
					if(row + rowOffset > 0 && row + rowOffset <= 13)
						return false;
					break;
				}
				case '+':
					// ignore.
					break;
			}
		}
	}
	return true;
}



inline int scoreFunction(int row, int col) {
	if(row <= 3 && col <= 3) {
		return 2;
	}
	else {
		if(row <= 10 && col <= 10) {
			return 4;
		}
		else return 5;
	}
}

double GoBook::evaluate(const GoBoard &board, SgBlackWhite color, SgPoint point) {
	double score = 0.0;
	for(int i = 0; i < local.size(); ++i) {
		if(match(board, color, point, local[i])) {
			//cerr << "Evaluation changed!" << endl;
			score += log(double(scoreFunction(local[i].row, local[i].col)));
		}
	}
	return score;
}


GoBook::~GoBook() {
	moves.clear();
}