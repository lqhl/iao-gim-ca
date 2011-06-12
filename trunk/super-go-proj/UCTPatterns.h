#ifndef UCTPATTERNS_H
#define UCTPATTERNS_H

#include "GoBoard.h"
#include "GoUctBoard.h"
#include "Poco/Debugger.h"
#include "SgPoint.h"
#include "GoBoardUtil.h"
using Poco::Debugger;

using SgPointUtil::Pt;
using GoBoardUtil::getGo8Neighbors;

class UctPatterns {
public:
	static const int BLACK_PATTERN = 0, WHITE_PATTERN = 1, BOTH_PATTERN = 2,
			NO_PATTERN = 3;

	static const int POWER3_9 = 3 * 3 * 3 * 3 * 3 * 3 * 3 * 3 * 3;
	static const int POWER3_6 = 3 * 3 * 3 * 3 * 3 * 3;

	static bool centerCode[2][POWER3_9];
	static bool edgeCode[2][POWER3_9];

	static const int BOARD_SIZE = 13;

	static int up[SG_MAXPOINT];

	static int pos[SG_MAXPOINT];

	static int line[SG_MAXPOINT];

	static int Up(SgPoint p) {
		return up[p];
	}

	static int Pos(SgPoint p) {
		return pos[p];
	}

	static int Line(SgPoint p) {
		return line[p];
	}

	static int OtherDir(int dir) {
		if (dir == SG_NS || dir == -SG_NS)
			return SG_WE;

		if (dir == SG_WE || dir == -SG_WE)
			return SG_NS;

		poco_assert(false);
		return -1;
	}
	static void init_board_const(int boardSize) {
		for (int i = 1; i <= boardSize; ++i) {
			for (int j = 1; j <= boardSize; ++j) {
				SgPoint p = Pt(i, j);
				int line1 = min(i, boardSize + 1 - i);
				int line2 = min(j, boardSize + 1 - j);
				line[p] = min(line1, line2);
				pos[p] = max(line1, line2);

				up[p] = 0;

				if (line[p] == 1 && pos[p] != 1) {
					if (i == 1)
						up[p] = SG_NS;
					else if (i == boardSize)
						up[p] = -SG_NS;
					else if (j == 1)
						up[p] = SG_WE;
					else if (j == boardSize)
						up[p] = -SG_WE;

					poco_assert(up[p] != 0);
				}
			}
		}
	}

	static bool initialized;

	static void init() {
		if (initialized)
			return;
		init_board_const(BOARD_SIZE);

		for (int i = 0; i < 2; ++i) {
			for (int j = 0; j < POWER3_9; ++j) {
				centerCode[i][j] = false;
				edgeCode[i][j] = false;
			}
		}

		int p[9];
		for (int i = 0; i < POWER3_9; ++i) {

			int k = i;
			for (int j = 0; j < 9; ++j) {
				p[j] = k % 3;
				k = k / 3;
			}

			int pattern = NO_PATTERN;
			int t = NO_PATTERN;

			t = matchHane(p);
			if (t != NO_PATTERN) {
				poco_assert(pattern == NO_PATTERN);
				pattern = t;
			}

			t = matchCut1(p);
			if (t != NO_PATTERN) {
				poco_assert(pattern == NO_PATTERN);
				pattern = t;
			}

			t = matchCut2(p);
			if (t != NO_PATTERN) {
				poco_assert(pattern == NO_PATTERN);
				pattern = t;
			}

			addCenterPattern(p, pattern);

		}

		for (int i = 6; i < 9; ++i)
			p[i] = SG_EMPTY;

		for (int i = 0; i < POWER3_6; ++i) {
			int k = i;
			for (int j = 0; j < 6; ++j) {
				p[j] = k % 3;
				k /= 3;
			}

			int pattern = matchEdge(p);

			addEdgePattern(p, pattern);
		}

		initialized = true;
	}

	// p and q maybe the same
	// #pragma warning(disable : 4996)
	static void rotate(int* p, int* q) {
		int t[9];
		for (int i = 0; i < 9; ++i) {
			int line = i / 3;
			int col = i % 3;
			t[col * 3 + (2 - line)] = p[i];
		}
		copy(t, t + 9, q);
	}

	static void mirror(int* p, int* q) {
		int t[9];
		for (int i = 0; i < 9; ++i) {
			int line = i / 3;
			t[line * 3 + 2 - i % 3] = p[i];
		}

		copy(t, t + 9, q);
	}

	static void reverseColor(int* p, int* q) {
		int t[9];
		for (int i = 0; i < 9; ++i) {
			if (p[i] == SG_EMPTY)
				t[i] = p[i];
			else
				t[i] = 1 - p[i];
		}
		copy(t, t + 9, q);
	}

	static void addCenterPattern(int* p, int pattern) {
		int q[9];
		if (pattern == NO_PATTERN)
			return;

		for (int i = 0; i < 8; ++i) {
			if (i % 2 == 0)
				rotate(p, p);
			else
				mirror(p, p);

			if (pattern == BOTH_PATTERN || pattern == BLACK_PATTERN)
				setCenterPattern(SG_BLACK, p);
			if (pattern == BOTH_PATTERN || pattern == WHITE_PATTERN)
				setCenterPattern(SG_WHITE, p);

			reverseColor(p, q);

			if (pattern == BOTH_PATTERN || pattern == BLACK_PATTERN)
				setCenterPattern(SG_WHITE, q);
			if (pattern == BOTH_PATTERN || pattern == WHITE_PATTERN)
				setCenterPattern(SG_BLACK, q);

		}
	}

	static void setCenterPattern(SgBlackWhite color, int* p) {
		int k = 0;
		for (int i = 0; i < 9; ++i)
			k = k * 3 + p[i];

		centerCode[color][k] = true;
	}

	// actually the same as mirror
	static void mirror2(int* p, int* q) {
		int t[9];
		for (int i = 0; i < 9; ++i) {
			int col = i % 3;
			int line = i / 3;
			t[line * 3 + 2 - col] = p[i];
		}

		copy(t, t + 9, q);
	}

	static void addEdgePattern(int* p, int pattern) {
		if (pattern == NO_PATTERN)
			return;
		int q[9];
		for (int i = 0; i < 2; ++i) {
			mirror2(p, p);

			if (pattern == BOTH_PATTERN || pattern == BLACK_PATTERN)
				setEdgePattern(SG_BLACK, p);
			if (pattern == BOTH_PATTERN || pattern == WHITE_PATTERN)
				setEdgePattern(SG_WHITE, p);

			reverseColor(p, q);
			if (pattern == BOTH_PATTERN || pattern == BLACK_PATTERN)
				setEdgePattern(SG_WHITE, q);
			if (pattern == BOTH_PATTERN || pattern == WHITE_PATTERN)
				setEdgePattern(SG_BLACK, q);

		}
	}

	static void setEdgePattern(SgBlackWhite color, int* p) {
		int k = 0;
		for (int i = 0; i < 6; ++i) {
			k = k * 3 + p[i];
		}

		edgeCode[color][k] = true;
	}

	template<typename BOARD>
	static bool matchAny(BOARD* bd, SgPoint p) {
		poco_assert(bd->Size() == BOARD_SIZE);

		if (Line(p) > 1) {
			int q[9];
			q[0] = bd->GetColor(p - SG_NS - SG_WE);
			q[1] = bd->GetColor(p - SG_NS);
			q[2] = bd->GetColor(p - SG_NS + SG_WE);
			q[3] = bd->GetColor(p - SG_WE);
			q[4] = bd->GetColor(p);
			q[5] = bd->GetColor(p + SG_WE);
			q[6] = bd->GetColor(p + SG_NS - SG_WE);
			q[7] = bd->GetColor(p + SG_NS);
			q[8] = bd->GetColor(p + SG_NS + SG_WE);

			return matchCenter(q, bd->ToPlay());
		} else if (Pos(p) > 1) {
			int q[6];
			int up = Up(p);
			int left = OtherDir(up);
			q[0] = bd->GetColor(p + up - left);
			q[1] = bd->GetColor(p + up);
			q[2] = bd->GetColor(p + up + left);
			q[3] = bd->GetColor(p - left);
			q[4] = bd->GetColor(p);
			q[5] = bd->GetColor(p + left);

			return matchEdge(q, bd->ToPlay());
		}

		return false;
	}

	static bool matchCenter(SgPoint *p, SgBlackWhite color) {
		int k = 0;
		for (int i = 0; i < 9; ++i) {
			k = k * 3 + p[i];
		}
		return centerCode[color][k];
	}

	static bool matchEdge(SgPoint *p, SgBlackWhite color) {
		int k = 0;
		for (int i = 0; i < 6; ++i) {
			k = k * 3 + p[i];
		}
		return edgeCode[color][k];
	}

	static int matchHane(SgBlackWhite* p) {
		int f1 = p[0] == SG_BLACK && p[1] == SG_WHITE && p[2] == SG_BLACK
				&& p[3] == SG_EMPTY && p[4] == SG_EMPTY && p[5] == SG_EMPTY;
		if (f1)
			return BOTH_PATTERN;
		bool f2 = p[0] == SG_BLACK && p[1] == SG_WHITE && p[2] == SG_EMPTY
				&& p[3] == SG_EMPTY && p[4] == SG_EMPTY && p[5] == SG_EMPTY
				&& p[7] == SG_EMPTY;
		if (f2)
			return BOTH_PATTERN;

		bool f3 = p[0] == SG_BLACK && p[1] == SG_WHITE && p[3] == SG_BLACK
				&& p[4] == SG_EMPTY && p[5] == SG_EMPTY && p[7] == SG_EMPTY;
		if (f3)
			return BOTH_PATTERN;

		bool f4 = p[0] == SG_BLACK && p[1] == SG_WHITE && p[2] == SG_WHITE
				&& p[3] == SG_EMPTY && p[4] == SG_EMPTY && p[5] == SG_EMPTY
				&& p[7] == SG_EMPTY;

		if (f4)
			return BLACK_PATTERN;

		return NO_PATTERN;

	}

	static int matchCut1(SgBlackWhite* p) {
		bool f1 = p[0] == SG_BLACK && p[1] == SG_WHITE && p[3] == SG_WHITE
				&& p[4] == SG_EMPTY;
		bool f2 = p[0] == SG_BLACK && p[1] == SG_WHITE && p[3] == SG_WHITE
				&& p[4] == SG_EMPTY && p[5] == SG_WHITE && p[7] == SG_EMPTY;
		bool f3 = p[0] == SG_BLACK && p[1] == SG_WHITE && p[3] == SG_WHITE
				&& p[4] == SG_EMPTY && p[5] == SG_EMPTY && p[7] == SG_WHITE;
		if (f1 && !f2 && !f3)
			return BOTH_PATTERN;

		return NO_PATTERN;
	}

	static int matchCut2(SgBlackWhite* p) {
		bool f = p[1] == SG_BLACK && p[3] == SG_WHITE && p[4] == SG_EMPTY
				&& p[5] == SG_WHITE && p[6] != SG_WHITE && p[7] != SG_WHITE
				&& p[8] != SG_WHITE;
		return f ? BOTH_PATTERN : NO_PATTERN;
	}

	static int matchEdge(SgBlackWhite* p) {
		bool f1 = p[0] == SG_BLACK && p[1] == SG_EMPTY && p[3] == SG_WHITE
				&& p[4] == SG_EMPTY;
		if (f1)
			return BOTH_PATTERN;

		bool f2 = p[1] == SG_BLACK && p[3] != SG_BLACK && p[4] == SG_EMPTY
				&& p[5] == SG_WHITE;
		if (f2)
			return BOTH_PATTERN;

		bool f3 = p[1] == SG_BLACK && p[2] == SG_WHITE && p[4] == SG_EMPTY;
		if (f3)
			return BLACK_PATTERN;

		bool f4 = p[1] == SG_BLACK && p[2] == SG_WHITE && p[4] == SG_EMPTY
				&& p[5] != SG_BLACK;
		if (f4)
			return WHITE_PATTERN;

		bool f5 = p[1] == SG_BLACK && p[2] == SG_WHITE && p[3] == SG_WHITE
				&& p[4] == SG_EMPTY && p[5] == SG_BLACK;
		if (f5)
			return WHITE_PATTERN;

		return NO_PATTERN;

	}

	static void test() {
		init();

		int p[9] = { SG_WHITE, SG_BLACK, SG_WHITE, SG_EMPTY, SG_EMPTY,
				SG_EMPTY, SG_EMPTY, SG_EMPTY, SG_EMPTY };

		poco_assert(matchCenter(p, SG_WHITE));
		poco_assert(matchCenter(p, SG_WHITE));

		int q[9], s[9];

		rotate(p, q);
		poco_assert(matchCenter(q, SG_WHITE));

		mirror(p, s);
		poco_assert(matchCenter(q, SG_BLACK));

		int e[6] =
				{ SG_WHITE, SG_EMPTY, SG_EMPTY, SG_BLACK, SG_EMPTY, SG_BLACK };

		poco_assert(matchEdge(e, SG_BLACK));
		poco_assert(matchEdge(e, SG_WHITE));

	}

	static void printPattern(SgPoint* p, ostream& out) {
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				out << SgEBW(p[i * 3 + j]);
			}
			out << endl;
		}
	}

	inline static bool match2PeepConnect(GoBoard* board, SgPoint p) {
		return false;
	}

	inline static bool match2Hane(GoBoard* board, SgPoint p) {
		return false;
	}

	inline static bool match2Connect(GoBoard* board, SgPoint p) {
		return false;
	}

	inline static bool match2Wall(GoBoard* board, SgPoint p) {
		return false;
	}

	inline static bool match2BadKogeima(GoBoard* board, SgPoint p) {
		SgBlackWhite me = board->ToPlay(), opponent = 1 - me;
		bool partner = false;
		int nb[3];
		fill(nb, nb + 3, 0);

		vector<SgPoint> neighbors;
		neighbors.reserve(8);
		getGo8Neighbors(*board, p, neighbors);
		SgPoint x;
		for (vector<SgPoint>::iterator it = neighbors.begin(); it
				!= neighbors.end(); ++it) {
			++nb[board->GetColor(*it)];
			if (board->GetColor(*it) == opponent)
				x = *it;
		}



		if (!(nb[me] == 0 && nb[opponent] == 1))
			return false;

		int row = Row(p), col = Col(p);

		if (x - p == SG_NS || x - p == -SG_NS) {
			if (col > 2 && board->GetColor(x - SG_WE) == SG_EMPTY
					&& board->GetColor(x - SG_WE - SG_WE) == me)
				return true;
			if (col + 1 < BOARD_SIZE && board->GetColor(x + SG_WE)
					== SG_EMPTY && board->GetColor(x + SG_WE + SG_WE) == me)
				return true;
		}
		else if (x - p == SG_WE || x - p == -SG_WE) {
			if (row > 2 && board->GetColor(x - SG_NS) == SG_EMPTY
					&& board->GetColor(x - SG_NS - SG_NS) == me)
				return true;
			if (row + 1 < BOARD_SIZE && board->GetColor(x + SG_NS)
					== SG_EMPTY && board->GetColor(x + SG_NS + SG_NS) == me)
				return true;
		}

		return false;
	}

	inline static bool match2EmptyTriangle(GoBoard* board, SgPoint p) {
		SgBlackWhite me = board->ToPlay(), opponent = 1 - me;
		int row = Row(p), col = Col(p);


		if (row > 1 && col > 1) {
			int nb[3];
			fill(nb, nb+3, 0);
			++nb[board->GetColor(p)];
			++nb[board->GetColor(p - SG_NS)];
			++nb[board->GetColor(p - SG_WE)];
			++nb[board->GetColor(p - SG_NS - SG_WE)];
			if (nb[me] == 3 && nb[SG_EMPTY] == 1) return true;

		}

		if (row > 1 && col < BOARD_SIZE) {
			int nb[3];
			fill(nb, nb+3, 0);
			++nb[board->GetColor(p)];
			++nb[board->GetColor(p - SG_NS)];
			++nb[board->GetColor(p + SG_WE)];
			++nb[board->GetColor(p - SG_NS + SG_WE)];
			if (nb[me] == 3 && nb[SG_EMPTY] == 1) return true;

		}

		if (row < BOARD_SIZE && col > 1) {
			int nb[3];
			fill(nb, nb+3, 0);
			++nb[board->GetColor(p)];
			++nb[board->GetColor(p + SG_NS)];
			++nb[board->GetColor(p - SG_WE)];
			++nb[board->GetColor(p + SG_NS - SG_WE)];
			if (nb[me] == 3 && nb[SG_EMPTY] == 1) return true;

		}

		if (row < BOARD_SIZE && col < BOARD_SIZE) {
			int nb[3];
			fill(nb, nb+3, 0);
			++nb[board->GetColor(p)];
			++nb[board->GetColor(p + SG_NS)];
			++nb[board->GetColor(p + SG_WE)];
			++nb[board->GetColor(p + SG_NS + SG_WE)];
			if (nb[me] == 3 && nb[SG_EMPTY] == 1) return true;

		}

		return false;
	}

};

#endif
